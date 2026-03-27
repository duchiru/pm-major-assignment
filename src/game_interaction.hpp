#pragma once

#include <fstream>
#include <iosfwd>
#include <string>

#include "constants.hpp"

// Game Interaction
/**
 * This module handles all input-related operations of the game.
 *
 * Responsibilities:
 *
 *   1. Input redirection
 *      - Read from keyboard (interactive mode)
 *      - Read from file (judge / testing mode)
 *
 *   2. Input validation
 *      - Ensure numeric input is valid
 *      - Prevent program crashes from bad input
 *
 *   3. User selections
 *      - Board size
 *      - Win condition (goal)
 *      - Game mode
 *      - Bot difficulty
 *
 *   4. Player moves
 *      - Read move coordinates (row, col)
 *
 * Notes for students:
 *
 *   • Functions here should ONLY handle input parsing.
 *   • Game rules are checked later in GameLogic.
 *   • Invalid input should return false so the caller
 *     can ask the user again.
 *
 */
namespace GameInteraction
{
  static std::ifstream global_file_in;
  std::streambuf *initInteraction(const ::RunConfig &config);
  void closeInteraction(std::streambuf *cin_backup);
  bool validateInput(std::string &input);
  bool getInput(int *val);
  bool selectSize(int *size);
  bool selectGoal(int *goal, const int size);
  bool selectGameMode(GameMode *mode);
  bool selectBotLevel(BotLevel *levels, const int index);
  bool getPlayerMove(int *row, int *col);
}