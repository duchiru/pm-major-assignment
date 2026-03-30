#include "game_logger.hpp"
#include "game_logic.hpp"

/* ---------- Game Logic ---------- */
/**
 * ============================================================
 * GAME LOGIC MODULE (STUDENT IMPLEMENTATION)
 * ============================================================
 *
 * This module implements the core rules of the game.
 *
 * Responsibilities:
 *
 *   • Validating player moves
 *   • Updating the game board
 *   • Checking win conditions
 *   • Detecting draw situations
 *   • Determining the current game state
 *
 * Important design rule:
 *
 *   The Logic module defines HOW the game rules work,
 *   but it does NOT control when actions happen.
 *
 * For example:
 *
 *   - GameEngine decides whose turn it is
 *   - Renderer prints the board
 *   - Interaction collects player input
 *   - Bot decides AI moves
 *
 * The Logic module only processes the board state
 * and determines whether moves are valid or if the
 * game has been won or drawn.
 *
 * Typical functions in this module include:
 *
 *   - checking if a move is valid
 *   - placing a move on the board
 *   - scanning the board for winning sequences
 *   - determining if the board is full
 *
 * Students must implement the skeleton functions
 * below to correctly enforce the rules of the game.
 *
 * ============================================================
 */

/**
 * Initialize board cells.
 *
 * All cells should be set to '-'
 * representing an empty cell.
 *
 * Example for size = 3:
 *
 *   - - -
 *   - - -
 *   - - -
 *
 * Hint:
 *   Use nested loops.
 */
void initBoard(char board[][BOARD_N_MAX],
               const int size)
{
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      board[i][j] = '-';
    }
  }
}

bool isValidMove(const char board[][BOARD_N_MAX],
                 const int size,
                 const int row,
                 const int col)
{
  return (row >= 0 && row < size) &&
         (col >= 0 && col < size) &&
         (board[row][col] == '-');
}

void makeMove(char board[][BOARD_N_MAX],
              const int row,
              const int col,
              const char symbol)
{
  board[row][col] = symbol;
}

bool isEmptyHead(char board[][BOARD_N_MAX],
                 int size,
                 int x,
                 int y,
                 const char symbol)
{
  // - on board boundary
  if (x < 0 || x >= size || y < 0 || y >= size)
    return true;

  // - is empty symbol ('-')
  if (board[x][y] == '-')
    return true;

  // - equal to current symbol
  if (board[x][y] == symbol)
    return true;

  return false;
}

/**
 * TODO:
 * Determine whether a player wins the game.
 *
 * A player wins if there are 'goal' consecutive symbols
 * in one of the following directions:
 *
 *   → horizontal
 *   ↓ vertical
 *   ↘ main diagonal
 *   ↙ anti diagonal
 *
 * Example (goal = 3):
 *
 *   X X X   -> win
 *
 * Parameters:
 *   board  : current board
 *   size   : board size
 *   symbol : player symbol ('X' or 'O')
 *   goal   : number of consecutive symbols required
 *   rule   : endpoint rule for advanced variants
 *
 * Return:
 *   true  -> player wins
 *   false -> no win detected
 */
bool checkWin(char board[][BOARD_N_MAX],
              const int size,
              const char symbol,
              const int goal,
              EndRule rule)
{
  // Check horizontal
  for (int i = 0; i < size; i++)
  {
    int cnt = 0;
    for (int j = 0; j < size; j++)
    {
      if (board[i][j] != symbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
      if (cnt >= goal && isEmptyHead(board, size, i, j - goal, symbol) && isEmptyHead(board, size, i, j + 1, symbol))
        return true;
    }
  }

  // Check vertical
  for (int j = 0; j < size; j++)
  {
    int cnt = 0;
    for (int i = 0; i < size; i++)
    {
      if (board[i][j] != symbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
      if (cnt >= goal && isEmptyHead(board, size, i - goal, j, symbol) && isEmptyHead(board, size, i + 1, j, symbol))
        return true;
    }
  }

  // Check main diagonal
  for (int sj = 0; sj < size; sj++)
  {
    int cnt = 0;
    for (int i = 0, j = sj; i < size && j < size; i++, j++)
    {
      if (board[i][j] != symbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
      if (cnt >= goal && isEmptyHead(board, size, i - goal, j - goal, symbol) && isEmptyHead(board, size, i + 1, j + 1, symbol))
        return true;
    }
  }

  for (int si = 1; si < size; si++)
  {
    int cnt = 0;
    for (int i = si, j = 0; i < size && j < size; i++, j++)
    {
      if (board[i][j] != symbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
      if (cnt >= goal && isEmptyHead(board, size, i - goal, j - goal, symbol) && isEmptyHead(board, size, i + 1, j + 1, symbol))
        return true;
    }
  }

  // Check anti diagonal
  for (int sj = 0; sj < size; sj++)
  {
    int cnt = 0;
    for (int i = 0, j = sj; i < size && j >= 0; i++, j--)
    {
      if (board[i][j] != symbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
      if (cnt >= goal && isEmptyHead(board, size, i - goal, j + goal, symbol) && isEmptyHead(board, size, i + 1, j - 1, symbol))
        return true;
    }
  }

  for (int si = 1; si < size; si++)
  {
    int cnt = 0;
    for (int i = si, j = size - 1; i < size && j >= 0; i++, j--)
    {
      if (board[i][j] != symbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
      if (cnt >= goal && isEmptyHead(board, size, i - goal, j + goal, symbol) && isEmptyHead(board, size, i + 1, j - 1, symbol))
        return true;
    }
  }

  return false;
}

/**
 * Determine whether the game is a draw.
 *
 * A draw occurs when:
 *
 *   - The board is full
 *   - No player has won
 *
 * Parameters:
 *   board : current board
 *   size  : board size
 *
 * Return:
 *   true  -> draw
 *   false -> game can continue
 */
bool checkDraw(char board[][BOARD_N_MAX],
               const int size)
{
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      if (board[i][j] == '-')
        return false;

  return true;
}

/* ---------- Bot Move Logic ---------- */
/**
 * ============================================================
 * BOT MOVE LOGIC (STUDENT IMPLEMENTATION)
 * ============================================================
 *
 * Students are required to implement bot strategies for different levels.
 *
 * EASY:
 *   - Required.
 *   - Implement a simple random move selection.
 *
 * MEDIUM:
 *   - Optional but recommended.
 *   - Suggested idea: heuristic evaluation.
 *
 * HARD:
 *   - Bonus challenge.
 *   - No restriction on algorithm.
 *   - You may design your own search / evaluation strategy.
 *
 * ============================================================
 */

pII botMove(char board[][BOARD_N_MAX],
            const int size,
            const int goal,
            const char symbol,
            const BotLevel level)
{
  char opponent = (symbol == 'X') ? 'O' : 'X';

  switch (level)
  {
  case BotLevel::EASY:
    // REQUIRED
    return random_pick(board, size);

  case BotLevel::MEDIUM:
    // OPTIONAL
    // Suggestion:
    // - try winning move
    // - block opponent winning move
    // - otherwise choose a heuristic position

    return simple_heuristic(board, size, goal, symbol, opponent);

  case BotLevel::HARD:
    // BONUS
    // Design your own strategy.
    // Possible approaches:
    //   - search algorithm
    //   - evaluation function
    //   - pattern detection

    // return hard_level(board, size, goal, symbol, opponent)

    // fallback (avoid crash)
    return random_pick(board, size);

  default:
    return random_pick(board, size);
  }
}

// Level 1
/* ---------- EASY BOT ---------- */
/**
 * TODO (Required)
 *
 * Implement a random move generator.
 *
 * Idea:
 *   1. Scan the board
 *   2. Collect all empty cells
 *   3. Randomly choose one
 *
 * Hint:
 *   board[i][j] == '-' means empty cell
 */

pII random_pick(char board[][BOARD_N_MAX],
                const int size)
{
  int row, col;

  do
  {
    row = rand() % size;
    col = rand() % size;
  } while (!isValidMove(board, size, row, col));

  return std::make_pair(row, col);
}

// Level 2
/* ---------- MEDIUM BOT ---------- */
/**
 * TODO (Optional)
 *
 * Implement a heuristic bot.
 *
 * Suggested strategy:
 *
 *   1. If bot has a winning move → play it
 *   2. If opponent has a winning move → block it
 *   3. Otherwise choose a good position
 *
 * Possible heuristics:
 *   - prefer center
 *   - prefer cells near existing pieces
 *   - extend current lines
 */

pII simple_heuristic(char board[][BOARD_N_MAX],
                     const int size,
                     const int goal,
                     const char botSymbol,
                     const char playerSymbol)
{
  // TODO: student implementation

  // fallback
  return random_pick(board, size);
}

// Level 3
/* ---------- HARD BOT ---------- */
/**
 * BONUS CHALLENGE
 *
 * Design a stronger AI for the game.
 *
 * There is no restriction on the algorithm.
 *
 * Possible directions:
 *   - search algorithm
 *   - board evaluation
 *   - pattern recognition
 *
 * You may also combine multiple techniques.
 */

pII hard_level(char board[][BOARD_N_MAX],
               const int size,
               const int goal,
               const char botSymbol,
               const char playerSymbol)
{
  // TODO: optional bonus implementation

  // fallback
  return random_pick(board, size);
}