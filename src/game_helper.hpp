#pragma once

#include <string>
#include <functional>
#include <type_traits>

// Game Helper
/**
 * Utility helper functions used across modules.
 */

template <typename Function>
auto measureExecutionTime(const std::string &label, Function func, bool enabled)
    -> std::invoke_result_t<Function>;