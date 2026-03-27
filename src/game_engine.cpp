#include <format>
#include <iostream>

#include "game_engine.hpp"
#include "game_interaction.hpp"
#include "game_logic.hpp"
#include "game_renderer.hpp"

/* ---------- Game Engine ---------- */
/**
 * ============================================================
 * GAME ENGINE MODULE (STUDENT IMPLEMENTATION)
 * ============================================================
 *
 * This module controls the overall flow of the game.
 *
 * Responsibilities:
 *
 *   • Setting up the game configuration
 *   • Running the main game loop
 *   • Coordinating Interaction, Renderer, Logic, and Bot modules
 *   • Producing the final GameResult
 *
 * Important design rule:
 *
 *   The GameEngine decides WHEN actions happen,
 *   but it does NOT implement the game rules itself.
 *
 * Game rules such as:
 *   - move validation
 *   - win detection
 *   - draw detection
 *
 * are handled by the GameLogic module.
 *
 * Students must implement the skeleton functions below
 * to connect all modules together.
 *
 * ============================================================
 */

void startGame(const RunConfig &config,
               GameSetup &gameSetup)
{
  // TODO:
  // 1. Clear screen if interactive mode
  if (config.interactive)
    clearScreen();

  // 2. Show game title
  showSelectMenu(SelectType::TITLE_UI, gameSetup);

  // 3. Ask user for board size
  // repeat until valid:
  showSelectMenu(SelectType::SIZE_UI, gameSetup);
  while (!GameInteraction::selectSize(&gameSetup.size))
    std::cout << std::format("\t+ Invalid N. Please enter a number between 3 and {}.\n", BOARD_N_MAX);

  // 4. Ask user for win condition (goal)
  showSelectMenu(SelectType::GOAL_UI, gameSetup);
  while (!GameInteraction::selectGoal(&gameSetup.goal, gameSetup.size))
    std::cout << std::format("\t+ Invalid G. Please enter a number between 3 and {}.\n", gameSetup.size);

  // 5. Ask for game mode
  showSelectMenu(SelectType::GAME_MODE_UI, gameSetup);
  while (!GameInteraction::selectGameMode(&gameSetup.mode))
    std::cout << "\t+ Invalid mode. Please enter 1, 2, or 3.\n";

  // 6. If mode == PVE, ask bot difficulty for player 2
  if (gameSetup.mode == GameMode::PVE)
  {
    showSelectMenu(SelectType::BOT_LEVEL_UI, gameSetup);
    while (!GameInteraction::selectBotLevel(gameSetup.levels, -1))
      std::cout << "\t+ Invalid level. Please enter 1, 2, or 3.\n";
  }

  // 7. If mode == EVE, ask bot difficulty for both bots
  if (gameSetup.mode == GameMode::EVE)
  {
    showSelectMenu(SelectType::BOT_LEVEL_UI, gameSetup);
    while (!GameInteraction::selectBotLevel(gameSetup.levels, 0))
      std::cout << "\t+ Invalid level. Please enter 1, 2, or 3.\n";
    while (!GameInteraction::selectBotLevel(gameSetup.levels, 1))
      std::cout << "\t+ Invalid level. Please enter 1, 2, or 3.\n";
  }

  // 8. Initialize board
  initBoard(gameSetup.board, gameSetup.size);
}

/**
 * Core game loop.
 *
 * Responsibilities:
 *
 *   1. Initialize game variables
 *      - current player
 *      - symbol mapping
 *      - turn counter
 *
 *   2. Repeat until game ends:
 *
 *        a) Display board
 *
 *        b) Determine current player type
 *           - human
 *           - bot
 *
 *        c) Get move
 *           - getPlayerMove() or botMove()
 *           - using measureExecutionTime()
 *
 *        d) Validate move
 *
 *        e) Apply move using makeMove()
 *
 *        f) Check win condition
 *           checkWin(...)
 *
 *        g) Check draw condition
 *           checkDraw(...)
 *
 *        h) Switch player
 *
 *   3. Return GameResult
 *
 * Return:
 *   GameResult structure containing:
 *      - winner
 *      - number of turns
 *      - whether winner is bot
 */
GameResult playGame(const RunConfig &config,
                    GameSetup &gameSetup)
{
  static const char symbols[2] = {'X', 'O'};
  GameResult result;

  // TODO:
  int currentPlayer = 0;
  int turns = 0;

  // 2. Main game loop
  while (true)
  {
    clearScreen();
    displayBoard(gameSetup.board, gameSetup.size);

    // Determine player type for current turn
    bool isBot = (gameSetup.mode == GameMode::EVE) ||
                 (gameSetup.mode == GameMode::PVE && currentPlayer == 1);

    showPlayer(currentPlayer + 1, isBot);

    int row, col;

    if (isBot)
    {
    }
    else
    {
      showSelectMenu(SelectType::PLAYER_UI, gameSetup);
      while (!GameInteraction::getPlayerMove(&row, &col) || !isValidMove(gameSetup.board, gameSetup.size, row, col))
        std::cout << "\t+ Invalid move. Please try again.\n";
    }

    gameSetup.board[row][col] = symbols[currentPlayer];
    currentPlayer = 1 - currentPlayer;
  }

  // while(true)

  //      a) display board
  //      displayBoard(...)

  //      b) determine if player is human or bot

  //      c) get move
  //         human -> getPlayerMove()
  //         bot   -> botMove()

  //         log bot runtime -> measureExecutionTime()

  // // Running Bot Move with meansure runtime
  // pII point = measureExecutionTime(
  //     "botMove",
  //     [&]() {
  //         return botMove(gameSetup.board,
  //                        gameSetup.size,
  //                        gameSetup.goal,
  //                        symbols[player],
  //                        gameSetup.levels[player]);
  //     },
  //     TIME_ENABLED);

  //      d) validate move
  //         isValidMove(...)

  //      e) apply move
  //         makeMove(...)

  //      f) check win
  //         checkWin(...)

  //      g) check draw
  //         checkDraw(...)

  //      h) switch player
  //         currentPlayer = 1 - currentPlayer

  // 3. fill GameResult structure
  // result.winner
  // result.turns
  // result.isBot

  return result;
}

void endGame(const RunConfig &config,
             GameSetup &gameSetup,
             GameResult &gameResult)
{
  // TODO:
  // 1. If interactive mode
  //      clear screen
  //      display final board
  //      show result

  // Example:
  // clearScreen();
  // displayBoard(gameSetup.board, gameSetup.size);
  // showResult(gameResult.winner, gameResult.isBot);

  // 2. If judge mode
  //      print minimal result
  // Example:
  // printResult(gameResult);

  // 3. (optional) log result using GameLogger
}