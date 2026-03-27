#pragma once

#include <random>
#include <string>
#include <utility>

// version
/**
 * Current version of the game program.
 * Used mainly for logging and debugging.
 *
 * TODO: Modify <STUDENT_ID> with your id.
 * Example: "0.4.2502xxxx"
 */
const std::string VERSION = "0.4.25023070";

// flag
/**
 * Global feature flags.
 *
 * ALGORITHM_FLAG
 *   Used to enable or disable advanced algorithms.
 *
 * TIME_ENABLED
 *   If true, execution time of certain functions
 *   will be logged (useful for performance testing).
 *
 * RANDOM_SEED
 *   Seed used for random number generation.
 *   Keeping the seed fixed ensures deterministic
 *   behavior for testing and debugging.
 */
const bool ALGORITHM_FLAG = true;
const bool TIME_ENABLED = true;

// Fixed seed ensures deterministic behavior during testing.
// Change the seed if you want different random games.
const int RANDOM_SEED = 3108;

// constants
/**
 * Game-related constants.
 *
 * BOARD_N_MAX
 *   Maximum allowed board size.
 *
 * SLEEP_TIME
 *   Delay (milliseconds) used for bot moves
 *   or UI animations.
 *
 */

// Maximum board dimension allowed by the program.
// Increasing this value may affect performance of bot algorithms.
const int BOARD_N_MAX = 12;
const int SLEEP_TIME = 1500;

/**
 * Global random number generator.
 *
 * Used by bot algorithms to generate random moves.
 *
 * std::mt19937 is a high-quality pseudo-random
 * number generator based on the Mersenne Twister algorithm.
 */
inline std::mt19937 generator(RANDOM_SEED);

/**
 * Bot difficulty levels.
 *
 * EASY
 *   Random move selection (required).
 *
 * MEDIUM
 *   Heuristic-based strategy (optional).
 *
 * HARD
 *   Advanced AI strategy (bonus challenge).
 *
 * INVALID_LV
 *   Used internally to represent invalid input.
 */
enum class BotLevel
{
  EASY,
  MEDIUM,
  HARD,
  INVALID_LV
};

/**
 * Game modes supported by the program.
 *
 * PVP
 *   Player vs Player
 *
 * PVE
 *   Player vs Bot
 *
 * EVE
 *   Bot vs Bot
 *
 * INVALID_MODE
 *   Used when user input is invalid.
 */
enum class GameMode
{
  PVP,
  PVE,
  EVE,
  INVALID_MODE
};

/**
 * UI menu types.
 *
 * Used by showSelectMenu() to determine which
 * menu should be displayed.
 *
 * Example:
 *
 *   showSelectMenu(SelectType::SIZE_UI);
 *
 * will display the board size selection menu.
 */
enum class SelectType
{
  TITLE_UI,
  SIZE_UI,
  GOAL_UI,
  GAME_MODE_UI,
  BOT_LEVEL_UI,
  PLAYER_UI,
  MUL_BOT_LEVEL_UI,
  INVALID_UI
};

/**
 * End-point rule for detecting winning sequences.
 *
 * These rules are commonly used in Gomoku-style games
 * to determine whether a line is considered valid.
 *
 * NONE
 *   Ignore endpoint checking.
 *
 * OPEN_ONE
 *   At least one end of the sequence must be open.
 *
 * OPEN_TWO
 *   Both ends must be open.
 *
 * This rule can affect advanced bot evaluation.
 */
enum class EndRule
{
  NONE,
  OPEN_ONE,
  OPEN_TWO
};

/**
 * Alias for a board coordinate.
 *
 * pII stands for:
 *   pair<int, int>
 *
 * Representing:
 *
 *   (row, column)
 */
typedef std::pair<int, int> pII;

/**
 * Runtime configuration for the program.
 *
 * This struct stores options parsed from
 * command line arguments.
 *
 * interactive
 *   true  -> show UI and menus
 *   false -> run in automated mode
 *
 * judge_mode
 *   used for automated grading systems.
 *
 * input_file
 *   path to file used as stdin.
 *
 * to_file
 *   whether logs should be written to file.
 *
 * log_file
 *   path of the log file.
 */
struct RunConfig
{
  bool interactive = true;
  bool judge_mode = false;
  std::string input_file;

  bool to_file = true;
  std::string log_file = "log.txt";
};

/**
 * Stores configuration and current state of the game.
 *
 * board
 *   2D board storing symbols ('X', 'O', '-')
 *
 * size
 *   board dimension
 *
 * goal
 *   number of consecutive symbols required to win
 *
 * mode
 *   game mode (PVP / PVE / EVE)
 *
 * levels
 *   bot difficulty levels for players
 */
struct GameSetup
{
  char board[BOARD_N_MAX][BOARD_N_MAX];
  int size;
  int goal;
  GameMode mode;
  BotLevel levels[2];
};

/**
 * Final result returned by the game engine.
 *
 * winner
 *   0  -> Player 1
 *   1  -> Player 2
 *   -1 -> Draw
 *
 * isBot
 *   true  -> winner is a bot
 *   false -> winner is a human
 *
 * turns
 *   total number of turns played.
 */
struct GameResult
{
  int winner;
  bool isBot;
  int turns;
};

constexpr int DRAW_RESULT = -1;