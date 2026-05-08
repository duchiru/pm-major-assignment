/**
 * Implementation of helper functions.
 */

#include "helper.h"

#include "../game/setup.h"

void debugPotentialMoves(const std::vector<pII> &potential_moves)
{
    Logger::log("Potential moves count: " + std::to_string(potential_moves.size()), Logger::Level::DEBUG);

    std::string move_list;
    for (const auto &move : potential_moves)
        move_list += " (" + std::to_string(move.first) + "," + std::to_string(move.second) + ")";

    Logger::log("Potential moves: " + move_list, Logger::Level::DEBUG);
}

ll power(ll base, int exp)
{
    ll result = 1;

    for (int i = exp; i > 0; i >>= 1)
    {
        if (i & 1)
            result *= base;
        base *= base;
    }

    return result;
}