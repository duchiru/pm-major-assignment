#include <format>
#include <iostream>

#include "game_renderer.hpp"

/* ---------- Game Renderer ---------- */
/**
 * ============================================================
 * RENDERER MODULE (STUDENT IMPLEMENTATION)
 * ============================================================
 *
 * This module is responsible for displaying information
 * to the console.
 *
 * It includes:
 *
 *   • Clearing the screen
 *   • Rendering the game board
 *   • Showing player actions
 *   • Displaying error messages
 *   • Showing the final result
 *
 * Important design rule:
 *
 *   The Renderer should ONLY print output.
 *   It must NOT implement any game logic.
 *
 * Game rules such as:
 *   - validating moves
 *   - checking win conditions
 *   - detecting draws
 *
 * are handled in the GameLogic module.
 *
 * Students need to implement the rendering functions below
 * to produce a clear and readable console interface.
 *
 * ============================================================
 */

/**
 * Clear the console screen. (Given)
 *
 * This function uses ANSI escape sequences to clear
 * the terminal and move the cursor to the top-left.
 *
 * It works on most Unix-like terminals and modern
 * Windows terminals.
 *
 * If the terminal does not support ANSI escape codes,
 * the game will still run but the screen may not clear.
 */
void clearScreen()
{
  std::cout << "\n"
            << "\x1B[2J\x1B[H"
            << "\n";
  return;
}

/**
 * Display a selection menu depending on the UI type.
 *
 * This function prints different menus used during
 * the game setup and interaction phase.
 *
 * Parameters:
 *
 *   selectType : type of menu to display
 *
 * Possible menu types:
 *
 *   TITLE_UI           -> show game title
 *   SIZE_UI            -> ask for board size
 *   GOAL_UI            -> ask for win goal
 *   GAME_MODE_UI       -> select PvP / PvE / EvE
 *   BOT_LEVEL_UI       -> select bot difficulty
 *   PLAYER_UI          -> ask player move input
 *   MUL_BOT_LEVEL_UI   -> select bot levels for EvE
 */
void showSelectMenu(SelectType selectType, GameSetup &currentGameSetup)
{
  switch (selectType)
  {
  case SelectType::TITLE_UI:
    std::cout << std::format(">----- Tic-tac-toe [Console v{}] -----<\n\n", VERSION);
    break;

  case SelectType::SIZE_UI:
    std::cout << std::format("- Enter board size (NxN, 3 <= N <= {})\n", BOARD_N_MAX);
    break;

  case SelectType::GOAL_UI:
    std::cout << std::format("- Enter goal count (G, 3 <= G <= {})\n", currentGameSetup.size);
    break;

  case SelectType::GAME_MODE_UI:
    std::cout << "- Select game mode [(1) PvP | (2) PvE | (3) EvE]\n";
    break;

  case SelectType::BOT_LEVEL_UI:
    std::cout << "- Select bot difficulty [(1) EASY | (2) MEDIUM | (3) HARD]\n";
    break;

  case SelectType::PLAYER_UI:
    std::cout << std::format("- Enter your move (row col)\n");
    break;

  case SelectType::MUL_BOT_LEVEL_UI:
    std::cout << "- Select difficulty for bot 1 and bot 2 respectively [(1) EASY | (2) MEDIUM | (3) HARD]\n";
    break;

  default:
    break;
  }

  return;
}

/**
 * Render the game board to the console.
 *
 * Example output (size = 3):
 *
 *     0 1 2
 *   --------
 * 0 |X - O
 * 1 |- X -
 * 2 |O - X
 *
 * Parameters:
 *
 *   board : 2D board array representing the game state
 *   size  : dimension of the board (NxN)
 *
 * Hint:
 *
 *   Use nested loops to print rows and columns.
 *   Each cell contains either:
 *
 *     'X' -> player 1
 *     'O' -> player 2
 *     '-' -> empty cell
 */
void displayBoard(const char board[][BOARD_N_MAX], const int size)
{
  std::cout << "   ";
  for(int i = 0; i < size; i++) std::cout << "\033[46;30m" << (i / 10) << "\033[0m" << ' ';
  std::cout << '\n';

  std::cout << "   ";
  for(int i = 0; i < size; i++) std::cout << "\033[46;30m" << (i % 10) << "\033[0m" << ' ';
  std::cout << '\n';

  std::cout << "  *";
  for(int i = 0; i < size * 2 - 1; i++) std::cout << '=';
  std::cout << "*\n";

  for(int i = 0; i < size; i++)
  {
    std::cout << "\033[46;30m" << std::format("{:02}", i) << "\033[0m" << "|";
    for(int j = 0; j < size; j++) {
      std::cout << board[i][j];
      if (j < size - 1) std::cout << ' ';
    }
    std::cout << "|\n";
  }

  std::cout << "  *";
  for(int i = 0; i < size * 2 - 1; i++) std::cout << '=';
  std::cout << "*\n";
}

/**
 * Display which player is taking the turn.
 *
 * Parameters:
 *
 *   player  : player number (1 or 2)
 *   is_bot  : true if the player is controlled by the bot
 *
 * Example output:
 *
 *   Player 1's turn
 *   Bot (Player 2) is thinking...
 */
void showPlayer(int player, bool is_bot)
{
  std::cout << (is_bot ? std::format("Bot (Player {}) is thinking...\n", player) : std::format("Player {}'s turn\n", player));
}

/**
 * Display the move selected by a player.
 *
 * Parameters:
 *
 *   row : row index of the move
 *   col : column index of the move
 *
 * Example output:
 *
 *   Move placed at (1, 2)
 */
void showMove(const int row, const int col)
{
  std::cout << "Last move: (" << row << ", " << col << ")\n";
}

/**
 * Display an error message when a move is invalid.
 *
 * This occurs when:
 *
 *   - The selected cell is already occupied
 *   - The move is outside the board
 */
void showInvalidMove()
{
  std::cout << "\t+ Invalid move. Please try again.\n";
}

/**
 * Display the final result of the game.
 *
 * Parameters:
 *
 *   winner : player number (1 or 2), or 0 if draw
 *   is_bot : true if the winner is a bot
 *
 * Example output:
 *
 *   Player 1 wins!
 *   Bot wins!
 *   It's a draw!
 */
void showResult(const int winner, const bool is_bot)
{
  if (winner == 0)
  {
    std::cout << "It's a draw!\n";
    return;
  }

  std::cout << (is_bot ? std::format("Bot (Player {}) wins!", winner) : std::format("Player {} wins!", winner)) << '\n';
}

/**
 * Print the result in non-interactive mode.
 *
 * This function is used when the game is executed
 * in automated or testing environments where
 * interactive UI is disabled.
 *
 * Parameters:
 *
 *   gameResult : structure containing the final
 *                result of the game.
 */
void printResult(const GameResult &gameResult)
{
  std::cout << gameResult.winner << ' ' << gameResult.turns << '\n';
}