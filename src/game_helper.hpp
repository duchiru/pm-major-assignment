#pragma once

#include <chrono>
#include <sstream>
#include <string>
#include <functional>
#include <type_traits>

#include "game_logger.hpp"

// Game Helper
/**
 * Utility helper functions used across modules.
 */

template <typename Function>
auto measureExecutionTime(const std::string &label, Function func, bool enabled)
        -> std::invoke_result_t<Function>
{
    using ReturnT = std::invoke_result_t<Function>;
    auto start = std::chrono::high_resolution_clock::now();

    if constexpr (std::is_void_v<ReturnT>)
    {
        func();
        auto end = std::chrono::high_resolution_clock::now();

        if (enabled)
        {
            std::chrono::duration<double> duration = end - start;
            std::stringstream msg;
            msg << "execution time of " << "[" << label << "()]" << " = " << duration.count() << "s";
            GameLogger::log(msg.str(), GameLogger::Level::DEBUG);
        }
        return;
    }
    else
    {
        ReturnT result = func();
        auto end = std::chrono::high_resolution_clock::now();

        if (enabled)
        {
            std::chrono::duration<double> duration = end - start;
            std::stringstream msg;
            msg << "execution time of " << "[" << label << "()]" << " = " << duration.count() << "s";
            GameLogger::log(msg.str(), GameLogger::Level::DEBUG);
        }

        return result;
    }
}

inline void debugPotentialMoves(const std::vector<pII>& potential_moves)
{
  GameLogger::log("Potential moves count: " + std::to_string(potential_moves.size()), GameLogger::Level::DEBUG);

  std::string move_list;
  for (const auto &move : potential_moves)
    move_list += " (" + std::to_string(move.first) + "," + std::to_string(move.second) + ")";

  GameLogger::log("Potential moves: " + move_list, GameLogger::Level::DEBUG);
}