#pragma once

#include "constants.hpp"

// Game Logic
/**
 * This module implements the rules of the game.
 *
 * Responsibilities:
 *
 *   • Board initialization
 *   • Move validation
 *   • Applying moves to the board
 *   • Checking win conditions
 *   • Detecting draw situations
 *
 * These functions operate directly on the board state.
 *
 * Important:
 *
 *   • GameLogic defines WHAT the rules are.
 *   • GameEngine decides WHEN these rules are checked.
 *
 * Students will mainly implement:
 *
 *   - initBoard()
 *   - isValidMove()
 *   - makeMove()
 *   - isEmptyHead()
 *   - checkWin()
 *   - checkDraw()
 *
 */

void initBoard(char board[][BOARD_N_MAX], const int size);
bool isValidMove(const char board[][BOARD_N_MAX], const int size, const int row, const int col);
void makeMove(char board[][BOARD_N_MAX], const int row, const int col, const char symbol);
bool isEmptyHead(char board[][BOARD_N_MAX], int size, int x, int y, const char symbol);
bool checkWin(char board[][BOARD_N_MAX], const int size, const char symbol, const int goal, EndRule rule = EndRule::OPEN_TWO);
bool checkDraw(char board[][BOARD_N_MAX], const int size);

// Bot Move Logic
/**
 * Generate the AI move for the current board state.
 *
 * This function dispatches to different strategies depending
 * on the selected difficulty level.
 *
 * The internal algorithms are intentionally modular so they can
 * be replaced or extended without affecting the rest of the engine.
 *
 * Developers are encouraged to experiment with their own AI logic
 * (pattern evaluation, Monte Carlo methods, minimax, etc.).
 *
 * @param board  Current board state
 * @param size   Board dimension (NxN)
 * @param goal   Number of consecutive symbols needed to win
 * @param symbol Bot symbol
 * @param level  Difficulty level
 *
 * @return Coordinate of the chosen move
 */
pII botMove(char board[][BOARD_N_MAX], const int size, const int goal, const char symbol, const BotLevel level);

// Level 1
/**
 * EASY difficulty.
 *
 * Selects a random valid move from the board.
 *
 * This mode exists mainly for testing and casual gameplay.
 *
 * @return Random empty cell
 */
pII random_pick(char board[][BOARD_N_MAX], const int size);

// Level 2
/**
 * MEDIUM difficulty.
 *
 * Uses a simple heuristic evaluation:
 * - try to win if possible
 * - block opponent winning moves
 * - otherwise choose a reasonable position
 *
 * This function is intentionally lightweight so the engine
 * remains fast even on large boards.
 *
 * @return Heuristically chosen move
 */
pII simple_heuristic(char board[][BOARD_N_MAX], const int size, const int goal, const char botSymbol, const char playerSymbol);

/*
 * NOTE FOR DEVELOPERS
 *
 * More advanced AI implementations can be added here if desired.
 *
 * Example approaches include:
 *
 * - Minimax with alpha-beta pruning
 * - Monte Carlo Tree Search (MCTS)
 * - Pattern scoring systems
 * - Hybrid evaluation + search
 *
 * The engine is designed so that new difficulty levels can be
 * implemented without modifying the rest of the game logic.
 */
pII hard_level(char board[][BOARD_N_MAX], const int size, const int goal, const char botSymbol, const char playerSymbol);