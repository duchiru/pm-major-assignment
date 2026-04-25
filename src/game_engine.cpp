#include <format>
#include <iostream>
#include <thread>
#include <chrono>

#include "game_engine.hpp"
#include "game_interaction.hpp"
#include "game_logic.hpp"
#include "game_renderer.hpp"
#include "game_helper.hpp"

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
  if (config.interactive)
    showSelectMenu(SelectType::TITLE_UI, gameSetup);

  // 3. Ask user for board size
  // repeat until valid:
  if (config.interactive)
    showSelectMenu(SelectType::SIZE_UI, gameSetup);
  while (!GameInteraction::selectSize(&gameSetup.size, config.interactive))
    std::cout << std::format("\t+ Invalid N. Please enter a number between 3 and {}.\n", BOARD_N_MAX);

  // 4. Ask user for win condition (goal)
  if (config.interactive)
    showSelectMenu(SelectType::GOAL_UI, gameSetup);
  while (!GameInteraction::selectGoal(&gameSetup.goal, gameSetup.size, config.interactive))
    std::cout << std::format("\t+ Invalid G. Please enter a number between 3 and {}.\n", gameSetup.size);

  // 5. Ask for game mode
  if (config.interactive)
    showSelectMenu(SelectType::GAME_MODE_UI, gameSetup);
  while (!GameInteraction::selectGameMode(&gameSetup.mode, config.interactive))
    std::cout << "\t+ Invalid mode. Please enter 1, 2, or 3.\n";

  // 6. If mode == PVE, ask bot difficulty for player 2
  if (gameSetup.mode == GameMode::PVE)
  {
    if (config.interactive)
      showSelectMenu(SelectType::BOT_LEVEL_UI, gameSetup);
    while (!GameInteraction::selectBotLevel(gameSetup.levels, -1, config.interactive))
      std::cout << "\t+ Invalid level. Please enter 1, 2, or 3.\n";
  }

  // 7. If mode == EVE, ask bot difficulty for both bots
  if (gameSetup.mode == GameMode::EVE)
  {
    if (config.interactive)
      showSelectMenu(SelectType::MUL_BOT_LEVEL_UI, gameSetup);
    while (!GameInteraction::selectBotLevel(gameSetup.levels, 0, config.interactive))
      std::cout << "\t+ Invalid level. Please enter 1, 2, or 3.\n";
    while (!GameInteraction::selectBotLevel(gameSetup.levels, 1, config.interactive))
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

  int currentPlayer = 0;
  int turns = 0;

  int row, col;

  // 2. Main game loop
  while (true)
  {
    if (config.interactive)
    {
      clearScreen();
      displayBoard(gameSetup.board, gameSetup.size);
    }

    // Show previous move
    if (turns > 0 && config.interactive)
      showMove(row, col);

    turns++;

    GameLogger::log(std::format("It's turn {} for player {}'s move", turns, currentPlayer + 1), GameLogger::Level::INFO);

    // Determine player type for current turn
    bool isBot = (gameSetup.mode == GameMode::EVE) ||
                 (gameSetup.mode == GameMode::PVE && currentPlayer == 1);

    if (config.interactive)
      showPlayer(currentPlayer + 1, isBot);

    if (isBot)
    {
      pII botMoveResult = measureExecutionTime(
          "botMove",
          [&]()
          {
            return botMove(gameSetup.board,
                           gameSetup.size,
                           gameSetup.goal,
                           symbols[currentPlayer],
                           gameSetup.levels[currentPlayer]);
          },
          true);
      
      std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME)); // Add a small delay for better UX
      
      row = botMoveResult.first;
      col = botMoveResult.second;
    }
    else
    {
      if (config.interactive)
        showSelectMenu(SelectType::PLAYER_UI, gameSetup);

      while (!GameInteraction::getPlayerMove(&row, &col, config.interactive) || !isValidMove(gameSetup.board, gameSetup.size, row, col))
        showInvalidMove();
    }

    makeMove(gameSetup.board, row, col, symbols[currentPlayer]);

    if (checkWin(gameSetup.board, gameSetup.size, symbols[currentPlayer], gameSetup.goal))
    {
      result.winner = currentPlayer;
      result.isBot = isBot;
      result.turns = turns;
      break;
    }

    if (checkDraw(gameSetup.board, gameSetup.size))
    {
      result.winner = -1;
      result.isBot = false;
      result.turns = turns;
      break;
    }

    currentPlayer = 1 - currentPlayer;
  }

  return result;
}

void endGame(const RunConfig &config,
             GameSetup &gameSetup,
             GameResult &gameResult)
{
  if (config.interactive)
  {
    clearScreen();
    displayBoard(gameSetup.board, gameSetup.size);
    showResult(gameResult.winner + 1, gameResult.isBot);
  }
  else if (config.judge_mode)
  {
    printResult(gameResult);
  }
}