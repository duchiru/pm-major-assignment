/**
 * Bot LV3 cpp implementation
 *
 */

#include "bot_lv3.h"

/* ---------- Importing ---------- */

#include <algorithm>
#include <sstream>

#include "../../utils/logger.h"
#include "../../utils/helper.h"
#include "../logic.h"

/* ---------- Definitions ---------- */

/**
 * Mô tả:
 *   Constructor của BotLevel3.
 */
BotLevel3::BotLevel3(const BotLevel &_level, const char &_symbol)
    : BotLevel2(_level, _symbol)
{
}

/**
 * Mô tả:
 *   Destructor của BotLevel3.
 */
BotLevel3::~BotLevel3()
{
}

/**
 * Mô tả:
 *   Trả về nước đi của bot HARD.
 *
 * TODO:
 *   - Bước 1: Gọi hard_level để tìm nước đi tối ưu
 *   - Bước 2: Nếu không tìm được → fallback về logic của BotLevel2
 */
pII BotLevel3::getMove(char board[][BOARD_N_MAX], const int size, const int goal)
{
    return hard_level(board, size, goal);
}

/**
 * Mô tả:
 *   Logic nâng cao cho bot HARD.
 *   Có thể sử dụng:
 *   - minimax
 *   - alpha-beta pruning
 *   - heuristic scoring
 *
 * TODO:
 *   - Bước 1: Duyệt các nước đi hợp lệ
 *   - Bước 2: Đánh giá từng nước (score)
 *   - Bước 3: Chọn nước có score tốt nhất
 *   - Trường hợp biên: không còn nước đi
 */
pII BotLevel3::hard_level(char board[][BOARD_N_MAX], const int size, const int goal)
{
}

ll minimax()
{
}

ll BotLevel3::evaluate(char board[][BOARD_N_MAX], const int size, const int goal, const char selfSymbol, const char opSymbol)
{
    ll heuristic_eval = 0;

    // Horizontal: left to right
    for (int i = 0; i < size; i++)
    {
        int cnt = 0;
        for (int j = 0; j < size; j++)
        {
            if (board[i][j] == '-' && cnt > 0)
            {
                bool can_win = true;                                                      // Whether bot can win by extending this streak
                can_win &= Logic::isEmptyHead(board, size, i, j - (cnt + 1), selfSymbol); // Check the start of this streak
                for (int k = j + 1; can_win && k < std::min(j + (goal - cnt), size); k++)
                    can_win &= (board[i][k] != opSymbol);

                if (can_win)
                    heuristic_eval = std::max(heuristic_eval, power(size, cnt));
            }

            if (board[i][j] != selfSymbol)
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
                can_win &= Logic::isEmptyHead(board, size, i, j + (cnt + 1), selfSymbol);
                for (int k = 1; can_win && k < goal - cnt; k++)
                {
                    if (j - k >= 0)
                        can_win &= (board[i][j - k] != opSymbol);
                    else
                        break;
                }

                if (can_win)
                    heuristic_eval = std::max(heuristic_eval, power(size, cnt));
            }

            if (board[i][j] != selfSymbol)
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
                bool can_win = true;                                                      // Whether bot can win by extending this streak
                can_win &= Logic::isEmptyHead(board, size, i - (cnt + 1), j, selfSymbol); // Check the start of this streak
                for (int k = i + 1; can_win && k < std::min(i + (goal - cnt), size); k++)
                    can_win &= (board[k][j] != opSymbol);

                if (can_win)
                    heuristic_eval = std::max(heuristic_eval, power(size, cnt));
            }

            if (board[i][j] != selfSymbol)
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
                can_win &= Logic::isEmptyHead(board, size, i + (cnt + 1), j, selfSymbol);
                for (int k = 1; can_win && k < goal - cnt; k++)
                {
                    if (i - k >= 0)
                        can_win &= (board[i - k][j] != opSymbol);
                    else
                        break;
                }

                if (can_win)
                    heuristic_eval = std::max(heuristic_eval, power(size, cnt));
            }

            if (board[i][j] != selfSymbol)
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
                can_win &= Logic::isEmptyHead(board, size, i - (cnt + 1), j - (cnt + 1), selfSymbol);
                for (int k = 1; can_win && i + k < size && j + k < size && k < goal - cnt; k++)
                    can_win &= (board[i + k][j + k] != opSymbol);

                if (can_win)
                    heuristic_eval = std::max(heuristic_eval, power(size, cnt));
            }

            if (board[i][j] != selfSymbol)
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
                can_win &= Logic::isEmptyHead(board, size, i - (cnt + 1), j - (cnt + 1), selfSymbol);
                for (int k = 1; can_win && i + k < size && j + k < size && k < goal - cnt; k++)
                    can_win &= (board[i + k][j + k] != opSymbol);

                if (can_win)
                    heuristic_eval = std::max(heuristic_eval, power(size, cnt));
            }

            if (board[i][j] != selfSymbol)
            {
                cnt = 0;
                continue;
            }

            cnt++;
        }
    }

    // Main diagonal: bottom-right to top-left
    for (int sj = 0; sj < size; sj++)
    {
        int cnt = 0;
        for (int i = size - 1, j = sj; i >= 0 && j >= 0; i--, j--)
        {
            if (board[i][j] == '-' && cnt > 0)
            {
                bool can_win = true;
                can_win &= Logic::isEmptyHead(board, size, i + (cnt + 1), j + (cnt + 1), selfSymbol);
                for (int k = 1; can_win && k < goal - cnt; k++)
                {
                    if (i - k >= 0 && j - k >= 0)
                        can_win &= (board[i - k][j - k] != opSymbol);
                    else
                        break;
                }

                if (can_win)
                    heuristic_eval = std::max(heuristic_eval, power(size, cnt));
            }

            if (board[i][j] != selfSymbol)
            {
                cnt = 0;
                continue;
            }

            cnt++;
        }
    }

    for (int si = 0; si < size - 1; si++)
    {
        int cnt = 0;
        for (int i = si, j = size - 1; i >= 0 && j >= 0; i--, j--)
        {
            if (board[i][j] == '-' && cnt > 0)
            {
                bool can_win = true;
                can_win &= Logic::isEmptyHead(board, size, i + (cnt + 1), j + (cnt + 1), selfSymbol);
                for (int k = 1; can_win && k < goal - cnt; k++)
                {
                    if (i - k >= 0 && j - k >= 0)
                        can_win &= (board[i - k][j - k] != opSymbol);
                    else
                        break;
                }

                if (can_win)
                    heuristic_eval = std::max(heuristic_eval, power(size, cnt));
            }

            if (board[i][j] != selfSymbol)
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
                can_win &= Logic::isEmptyHead(board, size, i - (cnt + 1), j + (cnt + 1), selfSymbol);
                for (int k = 1; can_win && i + k < size && j - k >= 0 && k < goal - cnt; k++)
                    can_win &= (board[i + k][j - k] != opSymbol);

                if (can_win)
                    heuristic_eval = std::max(heuristic_eval, power(size, cnt));
            }

            if (board[i][j] != selfSymbol)
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
                can_win &= Logic::isEmptyHead(board, size, i - (cnt + 1), j + (cnt + 1), selfSymbol);
                for (int k = 1; can_win && i + k < size && j - k >= 0 && k < goal - cnt; k++)
                    can_win &= (board[i + k][j - k] != opSymbol);

                if (can_win)
                    heuristic_eval = std::max(heuristic_eval, power(size, cnt));
            }

            if (board[i][j] != selfSymbol)
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
                can_win &= Logic::isEmptyHead(board, size, i + (cnt + 1), j - (cnt + 1), selfSymbol);
                for (int k = 1; can_win && k < goal - cnt; k++)
                {
                    if (i - k >= 0 && j + k < size)
                        can_win &= (board[i - k][j + k] != opSymbol);
                    else
                        break;
                }

                if (can_win)
                    heuristic_eval = std::max(heuristic_eval, power(size, cnt));
            }

            if (board[i][j] != selfSymbol)
            {
                cnt = 0;
                continue;
            }

            cnt++;
        }
    }

    for (int si = 0; si < size - 1; si++)
    {
        int cnt = 0;
        for (int i = si, j = 0; i >= 0 && j < size; i--, j++)
        {
            if (board[i][j] == '-' && cnt > 0)
            {
                bool can_win = true;
                can_win &= Logic::isEmptyHead(board, size, i + (cnt + 1), j - (cnt + 1), selfSymbol);
                for (int k = 1; can_win && k < goal - cnt; k++)
                {
                    if (i - k >= 0 && j + k < size)
                        can_win &= (board[i - k][j + k] != opSymbol);
                    else
                        break;
                }

                if (can_win)
                    heuristic_eval = std::max(heuristic_eval, power(size, cnt));
            }

            if (board[i][j] != selfSymbol)
            {
                cnt = 0;
                continue;
            }

            cnt++;
        }
    }

    return heuristic_eval;
}