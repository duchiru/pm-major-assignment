/**
 * Engine cpp implementation
 *
 */

#include "engine.h"

/* ---------- Importing ---------- */

#include <chrono>
#include <optional>
#include <sstream>
#include <thread>
#include <vector>

#include "../utils/helper.h"
#include "../utils/logger.h"
#include "bot/bot_factory.h"
#include "logic.h"
#include "setup.h"

/* ---------- Definitions ---------- */

/**
 * Mô tả:
 *   Constructor của Engine, nhận các dependency từ bên ngoài.
 *
 * Đầu vào:
 *   - _config: cấu hình chương trình
 *   - _iRenderer: renderer cụ thể (terminal/SDL)
 *   - _iInteraction: interaction cụ thể
 *
 * Đầu ra:
 *   - Không có
 *
 * Tác dụng phụ:
 *   - Gán con trỏ để sử dụng trong toàn bộ vòng đời engine
 */
Engine::Engine(const RunConfig *_config, I_Renderer *_iRenderer,
               I_Interaction *_iInteraction) {
  config = _config;
  iRenderer = _iRenderer;
  iInteraction = _iInteraction;
}

/**
 * Mô tả:
 *   Destructor của Engine.
 *
 * Đầu vào:
 *   - Không có
 *
 * Đầu ra:
 *   - Không có
 *
 * Tác dụng phụ:
 *   - Hiện tại chưa xử lý gì (cleanup ở nơi khác)
 */
Engine::~Engine() {}

/**
 * Mô tả:
 *   Khởi tạo các thành phần chính của engine (renderer + interaction).
 *
 * Đầu vào:
 *   - Không có
 *
 * Đầu ra:
 *   - Không có
 *
 * Tác dụng phụ:
 *   - Gọi init() của renderer và interaction
 */
void Engine::init() {
  Logger::log("Engine initializing . . .");

  iRenderer->init(*config);    // khởi tạo renderer theo config
  iInteraction->init(*config); // khởi tạo interaction theo config

  frame_duration = std::chrono::milliseconds(1000 / config->fps);

  Logger::log("Engine initialized!");
}

/**
 * Mô tả:
 *   Kiểm tra tính hợp lệ của các interface trước khi chạy game.
 *
 * Đầu vào:
 *   - Không có
 *
 * Đầu ra:
 *   - bool: true nếu hợp lệ, false nếu thiếu thành phần
 *
 * Tác dụng phụ:
 *   - Ghi log cảnh báo nếu thiếu renderer/interaction
 */
bool Engine::sanity_check() {
  bool isRendererGood = iRenderer;
  if (!isRendererGood) {
    Logger::log("Interface Renderer is not implemented!",
                Logger::Level::WARNING);
  }

  bool isInteractionGood = iInteraction;
  if (!isInteractionGood) {
    Logger::log("Interface Interaction is not implemented!",
                Logger::Level::WARNING);
  }

  return isRendererGood && isInteractionGood;
}

/**
 * Mô tả:
 *   Thiết lập game trước khi bắt đầu:
 *   - Lấy input từ user (size, goal, mode, bot level)
 *   - Khởi tạo board
 *
 * Đầu vào:
 *   - Không có
 *
 * Đầu ra:
 *   - Không có
 *
 * Tác dụng phụ:
 *   - Thay đổi gameSetup
 *   - Gọi render và interaction
 */
void Engine::startGame() {
  Logger::log("[Engine] Starting game . . .");

  if (!sanity_check()) {
    Logger::log("[Engine] Game stopped!", Logger::Level::ERROR);
    return;
  }

  SelectType currentSelectMenu = SelectType::TITLE_UI;
  int context = NO_CONTEXT;
  bool settingUp = true, preservePrevFrame = false;

  while (settingUp) {
    if (config->interactive) {
      if (preservePrevFrame) {
        preservePrevFrame = false;
      } else {
        iRenderer->clearScreen();
      }

      iRenderer->showSelectMenu(currentSelectMenu, context);
    }

    switch (currentSelectMenu) {
    case SelectType::TITLE_UI: {
      if (config->interactive) {
        iRenderer->renderFrame();
        iInteraction->pause();
      }

      currentSelectMenu = SelectType::SIZE_UI;

      break;
    }

    case SelectType::SIZE_UI: {
      if (config->interactive) {
        iRenderer->renderFrame();
      }

      if (iInteraction->selectSize(&gameSetup.size)) {
        currentSelectMenu = SelectType::GOAL_UI;
        context = gameSetup.size;
      } else {
        if (config->interactive) {
          iRenderer->showInvalidSelect(SelectType::SIZE_UI, gameSetup.size);
          preservePrevFrame = true;
        }
      }

      break;
    }

    case SelectType::GOAL_UI: {
      if (config->interactive) {
        iRenderer->renderFrame();
      }

      if (iInteraction->selectGoal(&gameSetup.goal, gameSetup.size)) {
        currentSelectMenu = SelectType::GAME_MODE_UI;
        context = NO_CONTEXT;
      } else {
        if (config->interactive) {
          iRenderer->showInvalidSelect(SelectType::GOAL_UI, gameSetup.goal);
          preservePrevFrame = true;
        }
      }

      break;
    }

    case SelectType::GAME_MODE_UI: {
      if (config->interactive) {
        iRenderer->renderFrame();
      }

      if (iInteraction->selectGameMode(&gameSetup.mode)) {
        if (gameSetup.mode == GameMode::PVE) {
          currentSelectMenu = SelectType::BOT_LEVEL_UI;
          context = NO_CONTEXT;
        } else if (gameSetup.mode == GameMode::EVE) {
          currentSelectMenu = SelectType::MUL_BOT_LEVEL_UI;
          context = 0;
        } else if (gameSetup.mode == GameMode::PVP) {
          settingUp = false;
          context = NO_CONTEXT;
        }
      } else {
        if (config->interactive) {
          iRenderer->showInvalidSelect(SelectType::GAME_MODE_UI,
                                       (int)gameSetup.mode);
          preservePrevFrame = true;
        }
      }

      break;
    }

    case SelectType::BOT_LEVEL_UI: {
      if (config->interactive) {
        iRenderer->renderFrame();
      }

      if (iInteraction->selectBotLevel(gameSetup.levels, 1)) {
        settingUp = false;
      } else {
        if (config->interactive) {
          iRenderer->showInvalidSelect(SelectType::BOT_LEVEL_UI,
                                       (int)gameSetup.levels[1]);
          preservePrevFrame = true;
        }
      }

      settingUp = false;
      context = NO_CONTEXT;

      break;
    }

    case SelectType::MUL_BOT_LEVEL_UI: {
      if (config->interactive) {
        iRenderer->renderFrame();
      }

      if (iInteraction->selectBotLevel(gameSetup.levels, context)) {
        if (context == 1) {
          settingUp = false;
          context = NO_CONTEXT;
        } else {
          context = 1;
        }
      } else {
        if (config->interactive) {
          iRenderer->showInvalidSelect(SelectType::MUL_BOT_LEVEL_UI,
                                       (int)gameSetup.levels[context]);
          preservePrevFrame = true;
        }
      }

      break;
    }
    }
  }

  // khởi tạo board rỗng
  Logic::initBoard(gameSetup.board, gameSetup.size);
  Logger::log("Board initialized!");

  Logger::log("[Engine] Game started!");
}

/**
 * Mô tả:
 *   Game loop chính:
 *   - Render board
 *   - Lấy input (bot hoặc human)
 *   - Thực hiện move
 *   - Kiểm tra thắng/thua/hòa
 *
 * Đầu vào:
 *   - Không có
 *
 * Đầu ra:
 *   - GameResult: kết quả cuối cùng
 *
 * Tác dụng phụ:
 *   - Thay đổi trạng thái board liên tục
 */
GameResult Engine::playGame() {
  Logger::log("[Engine] Playing game . . .");

  GameResult gameResult = GameResult(-1, false, 0);

  if (!sanity_check()) {
    Logger::log("[Engine] Game stopped!", Logger::Level::ERROR);
    return gameResult;
  }

  const char symbols[2] = {'X', 'O'};
  bool is_bot[2] = {
      gameSetup.mode == GameMode::EVE,
      gameSetup.mode != GameMode::PVP,
  };

  std::vector<std::unique_ptr<Bot>> bots;
  bots.clear();

  // khởi tạo bot theo mode
  if (gameSetup.mode == GameMode::PVE) {
    bots.emplace_back(std::unique_ptr<Bot>(nullptr)); // player 0 là human
    bots.emplace_back(std::unique_ptr<Bot>(
        BotFactory::createBot(gameSetup.levels[1], symbols[1])));
  } else if (gameSetup.mode == GameMode::EVE) {
    bots.emplace_back(std::unique_ptr<Bot>(
        BotFactory::createBot(gameSetup.levels[0], symbols[0])));
    bots.emplace_back(std::unique_ptr<Bot>(
        BotFactory::createBot(gameSetup.levels[1], symbols[1])));
  }

  int player = 0, turns = -1;
  bool is_running = true;

  int row = gameSetup.size / 2, col = gameSetup.size / 2; // default to center
  bool making_move = true; // whether wait for user
  bool invalid_move = false;

  // Game Loop
  while (is_running) {
    auto start_time = std::chrono::steady_clock::now();

    if (turns < gameResult.turns) {
      turns = gameResult.turns;
      Logger::debug(
          std::format("[Engine] starting turn #{}", gameResult.turns));
    }

    if (config->interactive) {
      iRenderer->clearScreen(); // clear screen (new frame)
      iRenderer->displayBoard(gameSetup.board, gameSetup.size); // render board

      if (invalid_move) {
        iRenderer->showInvalidMove();
      }

      iRenderer->showPlayer(player, is_bot[player]); // show current player
    }

    // 1. Get Move
    if (is_bot[player]) {
      if (config->interactive) {
        iRenderer->renderFrame();
      }
      // NOTE: đo thời gian thực thi bot
      pII point = measureExecutionTime(
          std::format("bot#{}->getMove()", player),
          [&]() {
            return bots[player]->getMove(gameSetup.board, gameSetup.size,
                                         gameSetup.goal);
          },
          TIME_ENABLED);

      row = point.first;
      col = point.second;

      making_move = false;
    } else {
      if (making_move) {
        if (config->interactive) {
          iRenderer->showSelectMenu(SelectType::PLAYER_UI,
                                    row * gameSetup.size + col);
          iRenderer->renderFrame();
        }

        if (iInteraction->getPlayerMove(&row, &col, gameSetup.size)) {
          // Player made a move
          if (Logic::isValidMove(gameSetup.board, gameSetup.size, row, col)) {
            // Found a valid move
            making_move = false;
            invalid_move = false;
          } else {
            // Invalid move
            invalid_move = true;
          }
        }
      }
    }

    if (!making_move) {
      // 2. Make move
      Logic::makeMove(gameSetup.board, row, col, symbols[player]);
      if (config->interactive)
        iRenderer->showMove(row, col);
      gameResult.turns += 1;

      std::stringstream ss;
      ss << "player " << player + 1 << " make move to (" << row << ", " << col
         << ")";
      Logger::log(ss.str(), Logger::Level::DEBUG);

      // delay khi bot chơi để dễ quan sát
      if (is_bot[player] and config->interactive) {
        iInteraction->pause(SLEEP_TIME);
      }

      // 3. Check result
      if (Logic::checkWin(gameSetup.board, gameSetup.size, symbols[player],
                          gameSetup.goal)) {
        gameResult.winner = player;
        gameResult.isBot = is_bot[player];
        is_running = false;
      } else if (Logic::checkDraw(gameSetup.board, gameSetup.size)) {
        gameResult.winner = -1;
        gameResult.isBot = false;
        is_running = false;
      }

      // 4. Switch player
      player = 1 - player;

      // Process this move done, wait for next move
      making_move = true;
    }

    if (gameResult.turns > turns)
      Logger::debug("[Engine] turn done!");

    auto end_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time);

    // If we finished early, sleep until the next frame should start
    if (elapsed < frame_duration) {
      std::this_thread::sleep_for(frame_duration - elapsed);
    }
  }

  Logger::log("[Engine] Game done!");

  return gameResult;
}

/**
 * Mô tả:
 *   Xử lý sau khi game kết thúc:
 *   - Hiển thị kết quả
 *   - Log thông tin game
 *
 * Đầu vào:
 *   - gameResult: kết quả game
 *
 * Đầu ra:
 *   - Không có
 *
 * Tác dụng phụ:
 *   - Render UI và log
 */
void Engine::endGame(const GameResult &gameResult) {
  Logger::log("[Engine] Ending game . . .");

  if (!sanity_check()) {
    Logger::log("[Engine] Game stopped!", Logger::Level::ERROR);
    return;
  }

  if (config->interactive) {
    iRenderer->clearScreen();
    iRenderer->displayBoard(gameSetup.board, gameSetup.size);

    char symbol = (gameResult.winner == 0) ? 'X' : 'O';

    // NOTE: lấy danh sách ô tạo thành đường thắng
    auto winLine = Logic::getWinLine(gameSetup.board, gameSetup.size, symbol,
                                     gameSetup.goal);

    if (winLine) {
      std::string s = "[WinLine] cells: ";
      for (auto [r, c] : winLine->cells)
        s += std::format("({}, {}) ", r, c);

      Logger::log(s, Logger::Level::DEBUG);
    } else {
      Logger::log("[WinLine] none", Logger::Level::DEBUG);
    }

    // hiển thị kết quả cuối cùng
    iRenderer->showResult(gameResult.winner, gameResult.isBot,
                          winLine ? &(*winLine) : nullptr);
    iRenderer->renderFrame();
    iInteraction->pause();
  } else if (config->judge_mode) {
    // mode judge (không interactive)
    iRenderer->printResult(gameResult);
  }

  // log thống kê
  std::stringstream ss;
  ss << "after " << gameResult.turns << " turns";
  Logger::log(ss.str(), Logger::Level::DEBUG);

  ss.str(std::string());
  ss.clear();

  ss << "game end with result: ";
  if (gameResult.winner == -1)
    ss << "draw";
  else
    ss << "player " << gameResult.winner + 1 << " ("
       << (gameResult.isBot ? "bot" : "human") << ") "
       << "win!";
  Logger::log(ss.str(), Logger::Level::DEBUG);

  Logger::log("[Engine] Game ended!");
}

/**
 * Mô tả:
 *   Dọn dẹp tài nguyên của engine.
 *
 * Đầu vào:
 *   - Không có
 *
 * Đầu ra:
 *   - Không có
 *
 * Tác dụng phụ:
 *   - Gọi close() của renderer và interaction
 */
void Engine::close() {
  Logger::log("Engine closing . . .");

  iRenderer->close();
  iInteraction->close();

  Logger::log("Engine closed!");
}