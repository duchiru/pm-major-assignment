#include <chrono>
#include <sstream>
#include <string>
#include <functional>
#include <type_traits>

#include "game_helper.hpp"
#include "game_logger.hpp"

/* ---------- Game Helper ---------- */
/**
 * ============================================================
 * HELPER FUNCTION (GIVEN)
 * ============================================================
 *
 * Measure execution time of a function.
 *
 * Example usage:
 *
 *   auto result = measureExecutionTime(
 *       "botMove",
 *       [&]() {
 *           return botMove(...);
 *       },
 *       true
 *   );
 *
 * If 'enabled = true', execution time will be printed to log.
 *
 * Students DO NOT need to modify this function.
 *
 * ============================================================
 */
template <typename Function>
auto measureExecutionTime(const std::string &label, Function func, bool enabled)
    -> std::invoke_result_t<Function>
{
  // Get Return Type
  using ReturnT = std::invoke_result_t<Function>;

  // Exercute func() and Meansure Runtime
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