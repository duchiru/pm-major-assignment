#pragma once

#include "constants.hpp"

// Game Renderer
/**
 * Responsible for displaying all visual elements in the console.
 *
 * Includes:
 *
 *   • Clearing the screen
 *   • Displaying menus
 *   • Rendering the game board
 *   • Showing player actions
 *   • Showing final results
 *
 * This module does NOT contain game logic.
 * It only prints information to the console.
 *
 */

void clearScreen();
void showSelectMenu(SelectType selectType, GameSetup &gameSetup);
void displayBoard(const char board[][BOARD_N_MAX], const int size);
void showMove(const int row, const int col);
void showInvalidMove();
void showPlayer(const int player, const bool is_bot);
void showResult(const int winner, const bool is_bot);
void printResult(const GameResult &gameResult);