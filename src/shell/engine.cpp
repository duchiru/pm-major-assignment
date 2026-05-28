/**
 * Engine implementation (Level 3 -- shell layer)
 *
 * Mô tả:
 *   Engine phối hợp pure functions (core/) với IO (interface/ + terminal/).
 *   Khác biệt so với level 2:
 *     - Logger được inject (logger_->log) thay vì global
 *     - RNG được inject (rng_) thay vì global
 *     - Board bất biến -- dùng core::applyTurn
 *     - Bot là hàm (BotFn) thay vì class hierarchy
 */

#include "engine.h"

/* ---------- Importing ---------- */

#include <chrono>
#include <format>
#include <sstream>
#include <thread>

#include "../core/bot_pure.h"
#include "../core/logic.h"
#include "../utils/helper.h"

/* ---------- Definitions ---------- */

Engine::Engine(const RunConfig* config,
               I_Renderer* iRenderer,
               I_Interaction* iInteraction,
               Logger* logger,
               Rng* rng)
    : config_(config),
      iRenderer_(iRenderer),
      iInteraction_(iInteraction),
      logger_(logger),
      rng_(rng) {}

Engine::~Engine() = default;

/* ---------- init ---------- */

void Engine::init() {
    if (logger_) logger_->log("Engine initializing . . .");

    if (iRenderer_)    iRenderer_->init(*config_);
    if (iInteraction_) iInteraction_->init(*config_);

    if (logger_) logger_->log("Engine initialized!");
}

bool Engine::sanity_check() {
    bool ok = true;
    if (!iRenderer_) {
        if (logger_) logger_->log("Renderer not implemented!", Logger::Level::WARNING);
        ok = false;
    }
    if (!iInteraction_) {
        if (logger_) logger_->log("Interaction not implemented!", Logger::Level::WARNING);
        ok = false;
    }
    if (!logger_) {
        ok = false;
    }
    if (!rng_) {
        if (logger_) logger_->log("RNG not provided!", Logger::Level::ERROR);
        ok = false;
    }
    return ok;
}

/* ---------- startGame ---------- */

/**
 * Mô tả: Setup game -- lấy size/goal/mode/bot levels từ user.
 * Dùng logger_->log thay vì Logger::log (DI).
 * Khởi tạo gameState_ bằng core::initBoard.
 */
void Engine::startGame() {
    if (logger_) logger_->log("[Engine] Starting game . . .");

    if (!sanity_check()) {
        if (logger_) logger_->log("[Engine] Game stopped!", Logger::Level::ERROR);
        return;
    }

    SelectType currentSelectMenu = SelectType::TITLE_UI;
    int context = NO_CONTEXT;
    bool settingUp = true, preservePrevFrame = false;

    while (settingUp) {
        if (config_->interactive) {
            if (preservePrevFrame) {
                preservePrevFrame = false;
            } else {
                iRenderer_->clearScreen();
            }

            iRenderer_->showSelectMenu(currentSelectMenu, context);
        }

        switch (currentSelectMenu) {
        case SelectType::TITLE_UI: {
            if (config_->interactive) {
                iInteraction_->pause();
            }
            currentSelectMenu = SelectType::SIZE_UI;
            break;
        }

        case SelectType::SIZE_UI: {
            if (iInteraction_->selectSize(&gameSetup_.size)) {
                currentSelectMenu = SelectType::GOAL_UI;
                context = gameSetup_.size;
            } else {
                if (config_->interactive) {
                    iRenderer_->showInvalidSelect(SelectType::SIZE_UI, gameSetup_.size);
                    preservePrevFrame = true;
                }
            }
            break;
        }

        case SelectType::GOAL_UI: {
            if (iInteraction_->selectGoal(&gameSetup_.goal, gameSetup_.size)) {
                currentSelectMenu = SelectType::GAME_MODE_UI;
                context = NO_CONTEXT;
            } else {
                if (config_->interactive) {
                    iRenderer_->showInvalidSelect(SelectType::GOAL_UI, gameSetup_.goal);
                    preservePrevFrame = true;
                }
            }
            break;
        }

        case SelectType::GAME_MODE_UI: {
            if (iInteraction_->selectGameMode(&gameSetup_.mode)) {
                if (gameSetup_.mode == GameMode::PVE) {
                    currentSelectMenu = SelectType::BOT_LEVEL_UI;
                    context = NO_CONTEXT;
                } else if (gameSetup_.mode == GameMode::EVE) {
                    currentSelectMenu = SelectType::MUL_BOT_LEVEL_UI;
                    context = 0;
                } else if (gameSetup_.mode == GameMode::PVP) {
                    settingUp = false;
                    context = NO_CONTEXT;
                }
            } else {
                if (config_->interactive) {
                    iRenderer_->showInvalidSelect(SelectType::GAME_MODE_UI,
                                                 static_cast<int>(gameSetup_.mode));
                    preservePrevFrame = true;
                }
            }
            break;
        }

        case SelectType::BOT_LEVEL_UI: {
            if (iInteraction_->selectBotLevel(gameSetup_.levels.data(), 1)) {
                settingUp = false;
            } else {
                if (config_->interactive) {
                    iRenderer_->showInvalidSelect(SelectType::BOT_LEVEL_UI,
                                                 static_cast<int>(gameSetup_.levels[1]));
                    preservePrevFrame = true;
                }
            }
            settingUp = false;
            context = NO_CONTEXT;
            break;
        }

        case SelectType::MUL_BOT_LEVEL_UI: {
            if (iInteraction_->selectBotLevel(gameSetup_.levels.data(), context)) {
                if (context == 1) {
                    settingUp = false;
                    context = NO_CONTEXT;
                } else {
                    context = 1;
                }
            } else {
                if (config_->interactive) {
                    iRenderer_->showInvalidSelect(SelectType::MUL_BOT_LEVEL_UI,
                                                 static_cast<int>(gameSetup_.levels[context]));
                    preservePrevFrame = true;
                }
            }
            break;
        }

        default:
            break;
        }
    }

    // Khởi tạo gameState_ bằng core::initBoard (immutable)
    gameState_.board = core::initBoard(gameSetup_.size);
    gameState_.currentPlayer = 0;
    gameState_.turn = 0;
    gameState_.winner = -1;
    gameState_.isFinished = false;

    if (logger_) logger_->log("Board initialized!");
    if (logger_) logger_->log("[Engine] Game started!");
}

/* ---------- playGame ---------- */

/**
 * Mô tả: Game loop chính.
 *   - Dùng core::applyTurn cho immutable state transitions
 *   - Dùng bot::makeBot (HOF factory) thay vì class hierarchy
 *   - Dùng logger_->log thay vì Logger::log
 */
GameResult Engine::playGame() {
    if (logger_) logger_->log("[Engine] Playing game . . .");

    GameResult gameResult{-1, false, 0};

    if (!sanity_check()) {
        if (logger_) logger_->log("[Engine] Game stopped!", Logger::Level::ERROR);
        return gameResult;
    }

    // Thiết lập bot flags
    bool is_bot[2] = {
        gameSetup_.mode == GameMode::EVE,
        gameSetup_.mode != GameMode::PVP,
    };

    // Tạo bot bằng FP factory (HOF)
    bot::BotFn bots[2];
    if (gameSetup_.mode == GameMode::EVE) {
        bots[0] = bot::makeBot(gameSetup_.levels[0], gameSetup_.goal);
        bots[1] = bot::makeBot(gameSetup_.levels[1], gameSetup_.goal);
    } else if (gameSetup_.mode == GameMode::PVE) {
        bots[0] = bot::BotFn{};  // player 0 là human
        bots[1] = bot::makeBot(gameSetup_.levels[1], gameSetup_.goal);
    }
    // PVP: cả hai đều null

    bool making_move = true;
    bool invalid_move = false;
    int row = gameSetup_.size / 2, col = gameSetup_.size / 2;

    // Game Loop
    while (!gameState_.isFinished) {
        int player = gameState_.currentPlayer;

        if (config_->interactive) {
            iRenderer_->clearScreen();
            iRenderer_->displayBoard(gameState_.board);

            if (invalid_move) {
                iRenderer_->showInvalidMove();
            }

            iRenderer_->showPlayer(player, is_bot[player]);
        }

        // 1. Get Move
        if (is_bot[player]) {
            if (bots[player]) {
                // Đo thời gian bot bằng HOF measureExecutionTime
                Move botMove = measureExecutionTime(
                    std::format("bot#{}", player),
                    [&]() { return bots[player](gameState_, *rng_); },
                    config_->verbose_flag,
                    [this](const std::string& m) {
                        logger_->log(m, Logger::Level::DEBUG);
                    }
                );

                row = botMove.row;
                col = botMove.col;
                making_move = false;
            }
        } else {
            if (making_move) {
                if (config_->interactive) {
                    iRenderer_->showSelectMenu(SelectType::PLAYER_UI,
                                              row * gameSetup_.size + col);
                }

                if (iInteraction_->getPlayerMove(&row, &col)) {
                    Move move{row, col};
                    if (core::isValidMove(gameState_.board, move)) {
                        making_move = false;
                        invalid_move = false;
                    } else {
                        invalid_move = true;
                    }
                }
            }
        }

        if (!making_move) {
            Move move{row, col};

            if (config_->interactive)
                iRenderer_->showMove(row, col);

            if (logger_) {
                logger_->log(std::format("player {} make move to ({}, {})",
                                         player + 1, row, col),
                             Logger::Level::DEBUG);
            }

            // Delay khi bot chơi (side-effect)
            if (is_bot[player] && config_->interactive) {
                iInteraction_->pause(SLEEP_TIME);
            }

            // Áp dụng nước đi bằng applyTurn (IMMUTABLE)
            gameState_ = core::applyTurn(gameState_, move, gameSetup_.goal);

            // Cập nhật game result
            gameResult.turns = gameState_.turn;
            if (gameState_.isFinished) {
                gameResult.winner = gameState_.winner;
                gameResult.isBot = (gameState_.winner >= 0) ? is_bot[gameState_.winner] : false;
            }

            // Reset cho nước đi tiếp
            making_move = true;
            invalid_move = false;
        }
    }

    if (logger_) logger_->log("[Engine] Game done!");
    return gameResult;
}

/* ---------- endGame ---------- */

/**
 * Mô tả: Hiển thị kết quả game.
 */
void Engine::endGame(const GameResult& gameResult) {
    if (logger_) logger_->log("[Engine] Ending game . . .");

    if (!sanity_check()) {
        if (logger_) logger_->log("[Engine] Game stopped!", Logger::Level::ERROR);
        return;
    }

    if (config_->interactive) {
        iRenderer_->clearScreen();
        iRenderer_->displayBoard(gameState_.board);

        char symbol = (gameResult.winner == 0) ? SYMBOL_X : SYMBOL_O;

        // Lấy đường thắng (pure function)
        auto winLine = core::getWinLine(gameState_.board, symbol,
                                         gameSetup_.goal);

        if (winLine && logger_) {
            std::string s = "[WinLine] cells: ";
            for (auto [r, c] : winLine->cells)
                s += std::format("({}, {}) ", r, c);
            logger_->log(s, Logger::Level::DEBUG);
        } else if (logger_) {
            logger_->log("[WinLine] none", Logger::Level::DEBUG);
        }

        iRenderer_->showResult(gameResult.winner, gameResult.isBot,
                               winLine ? &(*winLine) : nullptr);
        iInteraction_->pause();
    } else if (config_->judge_mode) {
        iRenderer_->printResult(gameResult);
    }

    // Log thống kê
    if (logger_) {
        logger_->log(std::format("after {} turns", gameResult.turns),
                     Logger::Level::DEBUG);

        std::string resultMsg = "game end with result: ";
        if (gameResult.winner == -1)
            resultMsg += "draw";
        else
            resultMsg += std::format("player {} ({}) win!",
                                     gameResult.winner + 1,
                                     gameResult.isBot ? "bot" : "human");
        logger_->log(resultMsg, Logger::Level::DEBUG);
    }

    if (logger_) logger_->log("[Engine] Game ended!");
}

/* ---------- close ---------- */

void Engine::close() {
    if (logger_) logger_->log("Engine closing . . .");

    if (iRenderer_)    iRenderer_->close();
    if (iInteraction_) iInteraction_->close();

    if (logger_) logger_->log("Engine closed!");
}
