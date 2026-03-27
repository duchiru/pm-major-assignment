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