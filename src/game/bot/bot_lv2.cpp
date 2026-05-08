/**
 * Bot LV2 cpp implementation
 *
 */

#include "bot_lv2.h"

/* ---------- Importing ---------- */

#include <sstream>

#include "../../utils/logger.h"
#include "../../utils/helper.h"
#include "../logic.h"

/* ---------- Definitions ---------- */

/**
 * Mô tả:
 *   Constructor của BotLevel2.
 */
BotLevel2::BotLevel2(const BotLevel &_level, const char &_symbol)
    : BotLevel1(_level, _symbol)
{
}

/**
 * Mô tả:
 *   Destructor của BotLevel2.
 */
BotLevel2::~BotLevel2()
{
}

/**
 * Mô tả:
 *   Trả về nước đi của bot MEDIUM.
 *
 * TODO:
 *   - Gọi heuristic để chọn nước đi
 */
pII BotLevel2::getMove(char board[][BOARD_N_MAX], const int size, const int goal)
{
    // TODO: gọi simple_heuristic với symbol và op_symbol
    throw NotImplementedException();
    return {0, 0};
}

/**
 * Mô tả:
 *   Heuristic đơn giản:
 *   - Ưu tiên thắng
 *   - Chặn đối thủ
 *   - Fallback random
 *
 * TODO:
 *   - Bước 1: kiểm tra nước thắng
 *   - Bước 2: kiểm tra nước chặn
 *   - Bước 3: chọn random nếu không có
 */
pII BotLevel2::simple_heuristic(char board[][BOARD_N_MAX], const int size, const int goal, const char botSymbol, const char playerSymbol)
{
    // 1. Check if bot can win in the next move
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (Logic::isValidMove(board, size, i, j))
            {
                board[i][j] = botSymbol;
                if (Logic::checkWin(board, size, botSymbol, goal, EndRule::OPEN_TWO))
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
            if (Logic::isValidMove(board, size, i, j))
            {
                board[i][j] = playerSymbol;
                if (Logic::checkWin(board, size, playerSymbol, goal, EndRule::OPEN_TWO))
                {
                    board[i][j] = '-';
                    return std::make_pair(i, j);
                }
                board[i][j] = '-';
            }
        }
    }

    std::vector<pII> potential_moves;

    // 3. Prefer center
    double center = (size - 1) / 2.0;
    for (int x = std::floor(center); x <= std::ceil(center); x++)
        for (int y = std::floor(center); y <= std::ceil(center); y++)
            if (Logic::isValidMove(board, size, x, y))
                potential_moves.push_back(std::make_pair(x, y));

    // 4. Prefer cells near existing pieces
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (Logic::isValidMove(board, size, i, j))
            {
                // Check if adjacent to existing pieces
                bool adjacent_existed_move = false;
                for (int di = -1; di <= 1; di++)
                {
                    for (int dj = -1; dj <= 1; dj++)
                    {
                        int ni = i + di;
                        int nj = j + dj;
                        adjacent_existed_move |= (ni >= 0 && ni < size && nj >= 0 && nj < size && board[ni][nj] != '-');

                        if (adjacent_existed_move)
                            break;
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

    debugPotentialMoves(potential_moves);
    return potential_moves[rand() % potential_moves.size()];
}