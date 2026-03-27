#pragma once

#include "constants.hpp"

// Game Engine
/**
 * This module controls the overall flow of the game.
 *
 * Responsibilities:
 *
 *   1. Game setup
 *      - read configuration from user
 *      - initialize the board
 *
 *   2. Game loop
 *      - display board
 *      - determine current player
 *      - get move (human or bot)
 *      - apply move
 *      - check win / draw
 *      - switch player
 *
 *   3. Game termination
 *      - show final board
 *      - print result
 *
 * Important:
 *
 *   • The Engine manages WHEN things happen.
 *   • The actual rules of the game are implemented
 *     in the GameLogic module.
 *
 */

void startGame(const RunConfig &config, GameSetup &gameSetup);
GameResult playGame(const RunConfig &config, GameSetup &gameSetup);
void endGame(const RunConfig &config, GameSetup &gameSetup, GameResult &gameResult);