#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "constants.hpp"
#include "game_logger.hpp"

/**
 * ============================================================
 * LOGGER MODULE (GIVEN)
 * ============================================================
 *
 * This module provides a simple logging system used throughout
 * the program.
 *
 * Students DO NOT need to modify this module.
 *
 * Usage examples:
 *
 *   GameLogger::log("Game started");
 *   GameLogger::log("Invalid move", GameLogger::Level::WARNING);
 *
 * Log levels:
 *   DEBUG   : detailed information for debugging
 *   INFO    : normal program information
 *   WARNING : suspicious behaviour
 *   ERROR   : invalid input or unexpected error
 *
 * Logs are printed to console and optionally written to file.
 *
 * ============================================================
 */
void GameLogger::init(bool judge_mode, bool to_file, const std::string &path)
{
  write_to_file = to_file;
  is_judge_mode = judge_mode;

  if (write_to_file)
  {
    // not append
    // log_file.open(path, std::ios::out | std::ios::app);

    // write new log contents
    log_file.open(path, std::ios::out | std::ios::trunc);
    if (!log_file.is_open())
    {
      std::cerr << "[Logger] Cannot open log file: " << path
                << ". Falling back to console only." << std::endl;
      write_to_file = false; // fallback
    }
  }

  std::string header = "Tic-tac-toe Game (Version: " + std::string(VERSION) + ")\n";
  header += std::string(48, '-');

  if (write_to_file)
  {
    log_file << header << std::endl;
  }

  if (!is_judge_mode)
  {
    std::cout << header << std::endl;
  }
}

void GameLogger::log(const std::string &msg, Level level)
{
  // Only log if current level is important as or more than min_level
  if (static_cast<int>(level) < static_cast<int>(min_level))
  {
    return;
  }

  auto now = std::time(nullptr);

  // NOTE: std::localtime is not thread-safe but acceptable here
  // since the program is single-threaded.
  auto tm = *std::localtime(&now);

  // format: '[Level] - [YYYY-MM-DD HH:MM:SS] Message'
  std::stringstream ss_lv;
  std::string formatted_lv;
  if (level != Level::MSG)
  {
    ss_lv << "[" << levelToString(level) << "]";
    formatted_lv = ss_lv.str();
  }

  std::stringstream ss_msg;
  ss_msg << (formatted_lv.empty() ? "" : " - ")
         << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] ";
  ss_msg << msg;
  std::string formatted_msg = ss_msg.str();

  // to output stream
  if (write_to_file)
  {
    log_file << formatted_lv << formatted_msg << std::endl;
    log_file.flush();
  }

  if (!is_judge_mode)
  {
    std::cout << getColor(level) << formatted_lv;
    std::cout << getColor(Level::MSG) << formatted_msg << RESET << std::endl;
  }
}

void GameLogger::close()
{
  if (log_file.is_open())
  {
    log_file.close();
  }
}