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

    return hard_level(board, size, goal, symbol, opponent);

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
  // 1. Check if bot can win in the next move
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      if (isValidMove(board, size, i, j))
      {
        board[i][j] = botSymbol;
        if (checkWin(board, size, botSymbol, goal, EndRule::OPEN_TWO))
        {
          board[i][j] = '-';
          return std::make_pair(i, j);
        }
        board[i][j] = '-';
      }
    }
  }

  // 2. Check if opponent can win in the next move and block it
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      if (isValidMove(board, size, i, j))
      {
        board[i][j] = playerSymbol;
        if (checkWin(board, size, playerSymbol, goal, EndRule::OPEN_TWO))
        {
          board[i][j] = '-';
          return std::make_pair(i, j);
        }
        board[i][j] = '-';
      }
    }
  }

  // 3. Prefer center
  if (isValidMove(board, size, size / 2, size / 2))
    return std::make_pair(size / 2, size / 2);

  // 4. Prefer cells near existing pieces
  std::vector<pII> potential_moves;
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      if (isValidMove(board, size, i, j))
      {
        // Check if adjacent to existing pieces
        bool adjacent_existed_move = false;
        for (int di = -1; di <= 1; di++)
        {
          for (int dj = -1; dj <= 1; dj++)
          {
            int ni = i + di;
            int nj = j + dj;
            if (ni >= 0 && ni < size && nj >= 0 && nj < size)
            {
              if (board[ni][nj] != '-')
              {
                adjacent_existed_move = true;
                break;
              }
            }
          }
          if (adjacent_existed_move)
            break;
        }
        if (adjacent_existed_move)
        {
          potential_moves.push_back(std::make_pair(i, j));
        }
      }
    }
  }

  if (!potential_moves.empty())
  {
    return potential_moves[rand() % potential_moves.size()];
  }

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
  static int reward[BOARD_N_MAX][BOARD_N_MAX];

  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      reward[i][j] = 0;

  calcRewardOnBotSide(board, size, goal, botSymbol, playerSymbol, reward);
  calcRewardOnBotSide(board, size, goal, playerSymbol, botSymbol, reward);

  int center = size / 2;
  if (isValidMove(board, size, center, center))
    reward[center][center] += 1;

  int best_reward = 0;
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      best_reward = std::max(best_reward, reward[i][j]);

  std::vector<pII> potential_moves;
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      if (reward[i][j] == best_reward)
        potential_moves.push_back(std::make_pair(i, j));

  GameLogger::log("Max reward: " + std::to_string(best_reward), GameLogger::Level::DEBUG);
  GameLogger::log("Potential moves count: " + std::to_string(potential_moves.size()), GameLogger::Level::DEBUG);

  std::string move_list;
  for (const auto &move : potential_moves)
    move_list += " (" + std::to_string(move.first) + "," + std::to_string(move.second) + ")";

  GameLogger::log("Potential moves: " + move_list, GameLogger::Level::DEBUG);

  return potential_moves[rand() % potential_moves.size()];
}

void calcRewardOnBotSide(char board[][BOARD_N_MAX],
                         const int size,
                         const int goal,
                         const char botSymbol,
                         const char playerSymbol,
                         int reward[][BOARD_N_MAX])
{
  // Horizontal: left to right
  for (int i = 0; i < size; i++)
  {
    int cnt = 0;
    for (int j = 0; j < size; j++)
    {
      if (board[i][j] == '-' && cnt > 0)
      {
        bool can_win = true; // Whether bot can win by extending this streak
        for (int k = j + 1; k < std::min(j + (goal - cnt), size); k++)
          can_win &= (board[i][k] != playerSymbol);

        if (can_win)
          reward[i][j] += cnt * cnt;
      }

      if (board[i][j] != botSymbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
    }
  }

  // Horizontal: right to left
  for (int i = 0; i < size; i++)
  {
    int cnt = 0;
    for (int j = size - 1; j >= 0; j--)
    {
      if (board[i][j] == '-' && cnt > 0)
      {
        bool can_win = true;
        for (int k = j - 1; k >= std::max(j - (goal - cnt), 0); k--)
          can_win &= (board[i][k] != playerSymbol);

        if (can_win)
          reward[i][j] += cnt * cnt;
      }

      if (board[i][j] != botSymbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
    }
  }

  // Vertical: top to bottom
  for (int j = 0; j < size; j++)
  {
    int cnt = 0;
    for (int i = 0; i < size; i++)
    {
      if (board[i][j] == '-' && cnt > 0)
      {
        bool can_win = true;
        for (int k = i + 1; k < std::min(i + (goal - cnt), size); k++)
          can_win &= (board[k][j] != playerSymbol);

        if (can_win)
          reward[i][j] += cnt * cnt;
      }

      if (board[i][j] != botSymbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
    }
  }

  // Vertical: bottom to top
  for (int j = 0; j < size; j++)
  {
    int cnt = 0;
    for (int i = size - 1; i >= 0; i--)
    {
      if (board[i][j] == '-' && cnt > 0)
      {
        bool can_win = true;
        for (int k = i - 1; k >= std::max(i - (goal - cnt), 0); k--)
          can_win &= (board[k][j] != playerSymbol);

        if (can_win)
          reward[i][j] += cnt * cnt;
      }

      if (board[i][j] != botSymbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
    }
  }

  // Main diagonal: top-left to bottom-right
  for (int sj = 0; sj < size; sj++)
  {
    int cnt = 0;
    for (int i = 0, j = sj; i < size && j < size; i++, j++)
    {
      if (board[i][j] == '-' && cnt > 0)
      {
        bool can_win = true;
        int need = goal - cnt;
        for (int k = 1; k < need; k++)
        {
          int ni = i + k;
          int nj = j + k;
          if (ni >= size || nj >= size)
            break;
          can_win &= (board[ni][nj] != playerSymbol);
        }

        if (can_win)
          reward[i][j] += cnt * cnt;
      }

      if (board[i][j] != botSymbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
    }
  }

  for (int si = 1; si < size; si++)
  {
    int cnt = 0;
    for (int i = si, j = 0; i < size && j < size; i++, j++)
    {
      if (board[i][j] == '-' && cnt > 0)
      {
        bool can_win = true;
        int need = goal - cnt;
        for (int k = 1; k < need; k++)
        {
          int ni = i + k;
          int nj = j + k;
          if (ni >= size || nj >= size)
            break;
          can_win &= (board[ni][nj] != playerSymbol);
        }

        if (can_win)
          reward[i][j] += cnt * cnt;
      }

      if (board[i][j] != botSymbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
    }
  }

  // Main diagonal: bottom-right to top-left
  for (int sj = size - 1; sj >= 0; sj--)
  {
    int cnt = 0;
    for (int i = size - 1, j = sj; i >= 0 && j >= 0; i--, j--)
    {
      if (board[i][j] == '-' && cnt > 0)
      {
        bool can_win = true;
        int need = goal - cnt;
        for (int k = 1; k < need; k++)
        {
          int ni = i - k;
          int nj = j - k;
          if (ni < 0 || nj < 0)
            break;
          can_win &= (board[ni][nj] != playerSymbol);
        }

        if (can_win)
          reward[i][j] += cnt * cnt;
      }

      if (board[i][j] != botSymbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
    }
  }

  for (int si = size - 2; si >= 0; si--)
  {
    int cnt = 0;
    for (int i = si, j = size - 1; i >= 0 && j >= 0; i--, j--)
    {
      if (board[i][j] == '-' && cnt > 0)
      {
        bool can_win = true;
        int need = goal - cnt;
        for (int k = 1; k < need; k++)
        {
          int ni = i - k;
          int nj = j - k;
          if (ni < 0 || nj < 0)
            break;
          can_win &= (board[ni][nj] != playerSymbol);
        }

        if (can_win)
          reward[i][j] += cnt * cnt;
      }

      if (board[i][j] != botSymbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
    }
  }

  // Anti diagonal: top-right to bottom-left
  for (int sj = 0; sj < size; sj++)
  {
    int cnt = 0;
    for (int i = 0, j = sj; i < size && j >= 0; i++, j--)
    {
      if (board[i][j] == '-' && cnt > 0)
      {
        bool can_win = true;
        int need = goal - cnt;
        for (int k = 1; k < need; k++)
        {
          int ni = i + k;
          int nj = j - k;
          if (ni >= size || nj < 0)
            break;
          can_win &= (board[ni][nj] != playerSymbol);
        }

        if (can_win)
          reward[i][j] += cnt * cnt;
      }

      if (board[i][j] != botSymbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
    }
  }

  for (int si = 1; si < size; si++)
  {
    int cnt = 0;
    for (int i = si, j = size - 1; i < size && j >= 0; i++, j--)
    {
      if (board[i][j] == '-' && cnt > 0)
      {
        bool can_win = true;
        int need = goal - cnt;
        for (int k = 1; k < need; k++)
        {
          int ni = i + k;
          int nj = j - k;
          if (ni >= size || nj < 0)
            break;
          can_win &= (board[ni][nj] != playerSymbol);
        }

        if (can_win)
          reward[i][j] += cnt * cnt;
      }

      if (board[i][j] != botSymbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
    }
  }

  // Anti diagonal: bottom-left to top-right
  for (int sj = 0; sj < size; sj++)
  {
    int cnt = 0;
    for (int i = size - 1, j = sj; i >= 0 && j < size; i--, j++)
    {
      if (board[i][j] == '-' && cnt > 0)
      {
        bool can_win = true;
        int need = goal - cnt;
        for (int k = 1; k < need; k++)
        {
          int ni = i - k;
          int nj = j + k;
          if (ni < 0 || nj >= size)
            break;
          can_win &= (board[ni][nj] != playerSymbol);
        }

        if (can_win)
          reward[i][j] += cnt * cnt;
      }

      if (board[i][j] != botSymbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
    }
  }

  for (int si = size - 2; si >= 0; si--)
  {
    int cnt = 0;
    for (int i = si, j = 0; i >= 0 && j < size; i--, j++)
    {
      if (board[i][j] == '-' && cnt > 0)
      {
        bool can_win = true;
        int need = goal - cnt;
        for (int k = 1; k < need; k++)
        {
          int ni = i - k;
          int nj = j + k;
          if (ni < 0 || nj >= size)
            break;
          can_win &= (board[ni][nj] != playerSymbol);
        }

        if (can_win)
          reward[i][j] += cnt * cnt;
      }

      if (board[i][j] != botSymbol)
      {
        cnt = 0;
        continue;
      }

      cnt++;
    }
  }
}