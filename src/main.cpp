/**
 * ============================================================
 * TIC-TAC-TOE PROGRAMMING ASSIGNMENT
 * ============================================================
 *
 * This file contains the starter code for the Tic-Tac-Toe
 * programming assignment.
 *
 * The purpose of this starter code is to provide the basic
 * infrastructure and program architecture so that students
 * can focus on implementing the core game logic and AI.
 *
 * ------------------------------------------------------------
 * PROVIDED (GIVEN CODE)
 * ------------------------------------------------------------
 *
 * The following components are already implemented and
 * should NOT need modification:
 *
 *   ✔ GameLogger
 *       Logging system for debugging and program output
 *
 *   ✔ Command line argument parser
 *       Handles --judge, --input, --log options
 *
 *   ✔ Game configuration structures
 *       RunConfig, GameSetup, GameResult
 *
 *   ✔ Program architecture
 *       Interaction / Renderer / Engine / Logic / Bot modules
 *
 *   ✔ Game engine structure (skeleton)
 *       startGame(), playGame(), endGame()
 *
 * These parts define the overall program flow.
 *
 * ------------------------------------------------------------
 * REQUIRED IMPLEMENTATION (STUDENTS MUST COMPLETE)
 * ------------------------------------------------------------
 *
 * Students must implement the following core functions:
 *
 *   Game Interaction
 *       validateInput()
 *       getInput()
 *       selectSize()
 *       selectGoal()
 *       selectGameMode()
 *       selectBotLevel()
 *       getPlayerMove()
 *
 *   Game Renderer
 *       displayBoard()
 *       showMove()
 *       showInvalidMove()
 *       showPlayer()
 *       showResult()
 *       printResult()
 *
 *   Game Logic
 *       initBoard()
 *       isValidMove()
 *       makeMove()
 *       isEmptyHead()
 *       checkWin()
 *       checkDraw()
 *
 *   Bot Logic
 *       random_pick()   (EASY bot - REQUIRED)
 *
 *   Game Engine
 *       startGame()
 *       playGame()
 *       endGame()
 *
 * ------------------------------------------------------------
 * OPTIONAL / BONUS TASKS
 * ------------------------------------------------------------
 *
 * Students may optionally implement stronger bot strategies:
 *
 *   simple_heuristic()   (MEDIUM difficulty)
 *   hard_level()         (HARD difficulty)
 *
 * These are not required for a basic passing solution but
 * may receive additional credit depending on the assignment
 * grading policy.
 *
 * ------------------------------------------------------------
 * IMPORTANT
 * ------------------------------------------------------------
 *
 * • Do NOT remove the provided program structure.
 * • You may add helper functions if needed.
 * • You may modify implementation details as long as the
 *   overall program still compiles and runs correctly.
 *
 * Please read the full assignment specification (PDF)
 * before starting the implementation.
 *
 * ============================================================
 */

#include <cstdlib>
#include <iostream>
#include <string>

/* ------------------------------------------------------------ */
/* -------------------- [GLOBAL VARIABLES] -------------------- */
/* ------------------------------------------------------------ */
/**
 * ============================================================
 * GLOBAL CONFIGURATION (MUST READ)
 * ============================================================
 *
 * This section defines global constants, enums, and data
 * structures used throughout the program.
 *
 * Students should read this section carefully to understand
 * the configuration of the game.
 *
 * Most values here should NOT need to be modified.
 * However, you are allowed to adjust them if necessary
 * for experimentation or testing.
 *
 * Contents of this section:
 *
 *   1. Version information
 *   2. Global flags
 *   3. Game constants
 *   4. Random number generator
 *   5. Enumerations (game modes, bot levels, UI types)
 *   6. Data structures used by the game engine
 *
 * ============================================================
 */
#include "constants.hpp"

/* -------------------------------------------------------- */
/* -------------------- [DECLARATIONS] -------------------- */
/* -------------------------------------------------------- */
/**
 * ============================================================
 * FUNCTION DECLARATIONS (MUST READ)
 * ============================================================
 *
 * This section lists all function prototypes used in the program.
 *
 * The implementation of these functions appears later in the file.
 *
 * Functions are grouped by module:
 *
 *   1. GameLogger      → logging system
 *   2. RunConfig       → command line arguments
 *   3. GameInteraction → input handling
 *   4. GameRenderer    → console UI
 *   5. GameEngine      → main game flow
 *   6. GameLogic       → board rules
 *   7. BotLogic        → AI move generation
 *   8. Helper          → utility functions
 *
 * You do not need to modify most of these declarations.
 *
 * ============================================================
 */
/* ------------------------------------------------------- */
/* -------------------- [DEFINITIONS] -------------------- */
/* ------------------------------------------------------- */
/**
 * ============================================================
 * TODO LIST FOR STUDENTS
 * ============================================================
 *
 * Interaction:
 *   validateInput
 *   getInput
 *   selectSize
 *   selectGoal
 *   selectGameMode
 *   selectBotLevel
 *   getPlayerMove
 *
 * Renderer:
 *   displayBoard
 *   showMove
 *   showInvalidMove
 *   showPlayer
 *   showResult
 *   printResult
 *
 * Game Logic:
 *   initBoard
 *   isValidMove
 *   makeMove
 *   isEmptyHead
 *   checkWin
 *   checkDraw
 *
 * Bot:
 *   random_pick (required)
 *   simple_heuristic (optional)
 *   hard_level (bonus)
 *
 * Game Engine
 *   startGame
 *   playGame
 *   endGame
 *
 * ============================================================
 */
#include "game_logger.hpp"
#include "game_interaction.hpp"
#include "game_renderer.hpp"
#include "game_engine.hpp"
#include "game_logic.hpp"
#include "game_helper.hpp"

/**
 * ============================================================
 * ARGUMENT PARSER (GIVEN)
 * ============================================================
 *
 * Parse command line arguments for the program.
 *
 * Supported arguments:
 *
 *   --judge, -j
 *       Enable judge mode (no UI, output minimal result)
 *
 *   --input <file>
 *       Read input from file instead of keyboard
 *
 *   --log <file>
 *       Write log output to file
 *
 * Students DO NOT need to modify this function.
 *
 * ============================================================
 */
RunConfig parseArgs(int argc, char *argv[])
{
  RunConfig config;

  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];

    if (arg == "-j" || arg == "--judge")
    {
      config.judge_mode = true;
      // auto turn off interactive
      config.interactive = false;
    }
    else if ((arg == "-i" || arg == "--input") && i + 1 < argc)
    {
      config.input_file = argv[++i];
    }
    else if ((arg == "-l" || arg == "--log") && i + 1 < argc)
    {
      config.log_file = argv[++i];
      if (config.log_file == "")
      {
        config.to_file = false;
      }
    }
    else if (arg == "-h" || arg == "--help")
    {
      std::cout << "Tic-tac-toe Game (Version: " << VERSION << " by baluong.87)\n";
      std::cout << "Usage: \n"
                << "\t" << "--judge, -j" << "\t" << "Enable judge mode (no UI, standard output only)\n"
                << "\t" << "--input, -i" << "\t" << "Path to input file\n"
                << "\t" << "--log, -l" << "\t" << "Path to log file (default: log.txt)\n";
      exit(0);
    }
  }

  return config;
}

/* ----------------------------------------------------- */
/* -------------------- [MAIN GAME] -------------------- */
/* ----------------------------------------------------- */
/**
 * ============================================================
 * MAIN GAME (GIVEN)
 * ============================================================
 *
 * This section contains the main game flow and core control
 * logic of the application.
 *
 * Responsibilities of this section include:
 *
 *   - Initializing the game environment
 *   - Starting the main game loop
 *   - Coordinating interaction between:
 *         + Game Engine
 *         + Game Renderer
 *         + Game Interaction
 *   - Managing turn order (player / bot)
 *   - Determining game state (running, win, draw, exit)
 *
 * The main game loop repeatedly:
 *
 *   1. Render the current board
 *   2. Receive player interaction
 *   3. Update the game state
 *   4. Let the bot perform its move
 *   5. Check win/draw conditions
 *
 * This section acts as the central controller that connects
 * all major modules together.
 *
 * ============================================================
 */
int main(int argc, char *argv[])
{
  RunConfig config = parseArgs(argc, argv);

  GameLogger::init(config.judge_mode, true, config.log_file);
  GameLogger::log("GameLogger initialized!");

  std::streambuf *cin_backup = GameInteraction::initInteraction(config);
  GameLogger::log("GameInteraction initialized!");

  GameSetup gameSetup;
  startGame(config, gameSetup);
  GameLogger::log("GameEngine initialized!");

  GameResult gameResult = playGame(config, gameSetup);
  GameLogger::log("GameEngine playing done!");

  endGame(config, gameSetup, gameResult);
  GameLogger::log("GameEngine show endgame done!");

  GameInteraction::closeInteraction(cin_backup);
  GameLogger::log("GameInteraction closed!");

  GameLogger::log("GameLogger closing . . .");
  GameLogger::close();

  return 0;
}

/* -------------------- [COMPILE] -------------------- */
/**
 * OPTION 1: Using g++ (>= v15.2.0) installed via MSYS2 (MinGW64)
 *
 * Compile:
 *   g++ -std=c++20 game.cpp -o game.exe
 *
 * (optional) Enable warnings:
 *   g++ -std=c++20 -Wall -Wextra game.cpp -o game.exe
 *
 * (optional) Debug build:
 *   g++ -std=c++20 -Wall -Wextra -g game.cpp -o game.exe
 *
 * OPTION 2: Using "Run Build Task" in VS Code
 *
 * - Create tasks.json (compiler configuration)
 * - (optional) Enable IntelliSense (C++20 compatible):
 *   create c_cpp_properties.json
 *
 * Compile:
 *   Ctrl + Shift + B
 */
