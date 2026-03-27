#include <cctype>
#include <format>
#include <iostream>
#include <string>

#include "constants.hpp"
#include "game_interaction.hpp"
#include "game_logger.hpp"

/* ---------- Game Interaction ---------- */
/**
 * ============================================================
 * INTERACTION MODULE (STUDENT IMPLEMENTATION)
 * ============================================================
 *
 * This module handles all user input operations of the game.
 *
 * Responsibilities:
 *
 *   • Reading input from the console or redirected input file
 *   • Parsing numeric values
 *   • Validating input format
 *   • Handling menu selections
 *   • Reading player moves
 *
 * Important design rule:
 *
 *   The Interaction module ONLY deals with input handling.
 *   It should NOT implement game rules or game logic.
 *
 * Game rule validation such as:
 *
 *   - checking if a move is inside the board
 *   - checking if a cell is empty
 *   - checking win or draw conditions
 *
 * must be handled in the GameLogic module instead.
 *
 * If invalid input is detected:
 *   • the function should return false
 *   • the caller (usually the GameEngine) will ask again
 *
 * Students are responsible for implementing the input
 * parsing and validation functions below.
 *
 * ============================================================
 */

// Given
std::streambuf *GameInteraction::initInteraction(const ::RunConfig &config)
{
  std::streambuf *cin_backup = nullptr;

  if (!config.interactive && !config.input_file.empty())
  {
    GameInteraction::global_file_in.open(config.input_file);
    if (GameInteraction::global_file_in.is_open())
    {
      cin_backup = std::cin.rdbuf();
      std::cin.rdbuf(GameInteraction::global_file_in.rdbuf());

      GameLogger::log(std::format("redirected cin to: {}", config.input_file));
    }
    else
    {
      GameLogger::log("failed to open input file, using console.", GameLogger::Level::ERROR);
    }
  }
  return cin_backup;
}

// Given
void GameInteraction::closeInteraction(std::streambuf *cin_backup)
{
  if (cin_backup)
  {
    std::cin.rdbuf(cin_backup);
    GameLogger::log("fallback using 'std::cin' input stream.");
  }

  if (GameInteraction::global_file_in.is_open())
  {
    GameInteraction::global_file_in.close();
  }
}

/**
 * Validate raw input string.
 *
 * Purpose:
 *   Ensure the input contains only numeric characters
 *   and can be safely converted to an integer.
 *
 * Example:
 *   "12"  -> valid
 *   "3a"  -> invalid
 *   ""    -> invalid
 *
 * Parameters:
 *   input : raw string read from std::cin
 *
 * Return:
 *   true  -> valid numeric input
 *   false -> invalid input
 *
 * Hint:
 *   - Check each character using std::isdigit()
 *   - Only digits 0-9 are allowed.
 *   - Negative numbers are not accepted.
 */
bool GameInteraction::validateInput(std::string &input)
{
  for (char x : input)
    if (!std::isdigit(x))
      return false;

  return true;
}

/**
 * Read a numeric value from input stream.
 *
 * Steps:
 *   1. Read a string from std::cin
 *   2. Validate the string using validateInput()
 *   3. Convert the string to integer
 *
 * Parameters:
 *   val : pointer to store the parsed integer
 *
 * Return:
 *   true  -> successful read
 *   false -> invalid input format
 *
 * Notes:
 *   - Do NOT crash on invalid input
 *   - Return false so caller can ask again
 */
bool GameInteraction::getInput(int *val)
{
  std::string inp;
  std::cin >> inp;

  if (GameInteraction::validateInput(inp))
  {
    *val = std::stoi(inp);
    return true;
  }

  return false;
}

/**
 * Ask user to choose board size.
 *
 * Allowed range:
 *   3 <= size <= BOARD_N_MAX
 *
 * Example:
 *   Input: 5
 *   Board will be 5 x 5
 *
 * Parameters:
 *   size : pointer to store selected board size
 *
 * Return:
 *   true  -> valid selection
 *   false -> invalid input or out of range
 *
 * Hint:
 *   Use getInput() and check range.
 */
bool GameInteraction::selectSize(int *size)
{
  std::cout << "\t+ Enter N: ";

  int inp;
  if (GameInteraction::getInput(&inp) && inp >= 3 && inp <= BOARD_N_MAX)
  {
    *size = inp;
    return true;
  }

  return false;
}

/**
 * Ask user to choose the number of consecutive symbols required to win.
 *
 * Constraints:
 *   3 <= goal <= size
 *
 * Example:
 *   size = 5
 *   goal can be: 3, 4, or 5
 *
 * Parameters:
 *   goal : pointer to store win condition
 *   size : board size
 *
 * Return:
 *   true  -> valid selection
 *   false -> invalid selection
 */
bool GameInteraction::selectGoal(int *goal, const int size)
{
  std::cout << "\t+ Enter G: ";

  int inp;
  if (GameInteraction::getInput(&inp) && inp >= 3 && inp <= size)
  {
    *goal = inp;
    return true;
  }

  return false;
}

/**
 * Ask user to select game mode.
 *
 * Available modes:
 *
 *   1 -> Player vs Player
 *   2 -> Player vs Bot
 *   3 -> Bot vs Bot
 *
 * Parameters:
 *   mode : pointer to store selected mode
 *
 * Return:
 *   true  -> valid choice
 *   false -> invalid input
 */
bool GameInteraction::selectGameMode(GameMode *mode)
{
  std::cout << "\t+ Enter game mode: ";

  int inp;
  if (GameInteraction::getInput(&inp))
  {
    switch (inp)
    {
    case 1:
      *mode = GameMode::PVP;
      return true;
    case 2:
      *mode = GameMode::PVE;
      return true;
    case 3:
      *mode = GameMode::EVE;
      return true;
    default:
      return false;
    }
  }

  return false;
}

/**
 * Ask the user to select the bot difficulty level.
 *
 * Available level:
 *
 *   1 -> EASY
 *   2 -> MEDIUM
 *   3 -> HARD
 *
 * The selected level will be written into the provided
 * BotLevel array at the given index.
 *
 * Parameters:
 *
 *   levels  - Pointer to the bot level array used in the game
 *   index   - Position in the array where the selected level
 *             will be stored
 *
 * Return:
 *
 *   true  -> Level selected successfully
 *   false -> Invalid input
 *
 * Notes:
 *   - Player vs Bot -> index = 1
 *   - Bot vs Bot -> index in [0, 1]
 *
 */
bool GameInteraction::selectBotLevel(BotLevel *levels, int index)
{
  if (index == -1)
  { // PVE
    std::cout << "\t+ Enter bot difficulty: ";
    index = 0; // default to bot 1 for PvE
  }
  else
  { // EVE
    std::cout << std::format("\t+ Enter bot {} difficulty: ", index + 1);
  }

  int inp;
  if (GameInteraction::getInput(&inp))
  {
    switch (inp)
    {
    case 1:
      levels[index] = BotLevel::EASY;
      return true;
    case 2:
      levels[index] = BotLevel::MEDIUM;
      return true;
    case 3:
      levels[index] = BotLevel::HARD;
      return true;
    default:
      return false;
    }
  }

  return false;
}

/**
 * Read a player's move from input.
 *
 * Expected input format:
 *
 *   row col
 *
 * Example:
 *
 *   1 2
 *
 * Means:
 *   place symbol at board[1][2]
 *
 * Parameters:
 *   row : pointer storing row index
 *   col : pointer storing column index
 *
 * Return:
 *   true  -> input successfully parsed
 *   false -> invalid format
 *
 * Notes:
 *   - Range checking is done later using isValidMove().
 *   - Coordinates are ZERO-BASED.
 *
 * Example:
 * 0 0 -> top-left corner
 *
 */
bool GameInteraction::getPlayerMove(int *row, int *col)
{
  std::cout << "\t+ Enter your move (row col): ";

  return (GameInteraction::getInput(row) && GameInteraction::getInput(col));
}