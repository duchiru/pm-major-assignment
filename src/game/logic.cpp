/**
 * Logic cpp implementation
 *
 */

#include "logic.h"

/* ---------- Importing ---------- */

#include <format>
#include <limits>
#include <sstream>

#include "../utils/logger.h"

/* ---------- Definitions ---------- */

/**
 * Mô tả: Khởi tạo bàn cờ với ký tự mặc định.
 * Đầu vào: board, size
 * Đầu ra: Không có
 * Tác dụng phụ: Ghi dữ liệu vào board
 * TODO:
 */
void Logic::initBoard(char board[][BOARD_N_MAX], const int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            board[i][j] = '-'; // ký tự mặc định cho ô trống
        }
    }
}

/**
 * Mô tả: Kiểm tra nước đi có hợp lệ không.
 * Đầu vào: board, size, row, col
 * Đầu ra: true nếu hợp lệ
 * Tác dụng phụ: Không có
 * TODO:
 */
bool Logic::isValidMove(const char board[][BOARD_N_MAX], const int size, const int row, const int col)
{
    return (row >= 0 && row < size) &&
           (col >= 0 && col < size) &&
           (board[row][col] == '-');
}

/**
 * Mô tả: Thực hiện một nước đi.
 * Đầu vào: board, row, col, symbol
 * Đầu ra: Không có
 * Tác dụng phụ: Ghi vào board
 * TODO:
 */
void Logic::makeMove(char board[][BOARD_N_MAX], const int row, const int col, const char symbol)
{
    board[row][col] = symbol;
}

/**
 * Mô tả: Kiểm tra một ô có phải là đầu mở hay không.
 * Đầu vào: board, size, x, y, symbol
 * Đầu ra: true nếu là đầu mở
 * Tác dụng phụ: Không có
 * TODO:
 */
bool Logic::isEmptyHead(const char board[][BOARD_N_MAX], const int size, int x, int y, const char symbol)
{
    return (x < 0 || x >= size || y < 0 || y >= size) || (board[x][y] == '-') || (board[x][y] == symbol);
}

/**
 * Mô tả: Kiểm tra điều kiện thắng.
 * Đầu vào: board, size, symbol, goal, rule
 * Đầu ra: true nếu thắng
 * Tác dụng phụ: Có thể log debug
 * TODO:
 */
bool Logic::checkWin(char board[][BOARD_N_MAX], const int size, const char symbol, const int goal, EndRule rule)
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
 * Mô tả: Kiểm tra trạng thái hòa.
 * Đầu vào: board, size
 * Đầu ra: true nếu hòa
 * Tác dụng phụ: Không có
 * TODO:
 */
bool Logic::checkDraw(char board[][BOARD_N_MAX], const int size)
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (board[i][j] == '-')
                return false;

    return true;
}

/**
 * Mô tả: Lấy đường thắng nếu tồn tại.
 * Đầu vào: board, size, symbol, goal, rule
 * Đầu ra: optional WinLine
 * Tác dụng phụ: Không có
 * TODO:
 */
std::optional<WinLine> Logic::getWinLine(
    const char board[][BOARD_N_MAX],
    const int size,
    const char symbol,
    const int goal,
    EndRule rule)
{
    std::vector<pII> cells; // Cells made the winning line

    // Check horizontal
    for (int i = 0; i < size && cells.size() != 0; i++)
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
            {
                // There is a winning line from (i, j - goal + 1) to (i, j)
                for (int k = 0; k < goal; k++)
                    cells.push_back({i, j - k});
            }
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
            {
                // There is a winning line from (i - goal + 1, j) to (i, j)
                for (int k = 0; k < goal; k++)
                    cells.push_back({i - k, j});
            }
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
            {
                // There is a winning line from (i - goal + 1, j - goal + 1) to (i, j)
                for (int k = 0; k < goal; k++)
                    cells.push_back({i - k, j - k});
            }
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
            {
                // There is a winning line from (i - goal + 1, j - goal + 1) to (i, j)
                for (int k = 0; k < goal; k++)
                    cells.push_back({i - k, j - k});
            }
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
            {
                // There is a winning line from (i - goal + 1, j + goal - 1) to (i, j)
                for (int k = 0; k < goal; k++)
                    cells.push_back({i - k, j + k});
            }
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
            {
                // There is a winning line from (i - goal + 1, j + goal - 1) to (i, j)
                for (int k = 0; k < goal; k++)
                    cells.push_back({i - k, j + k});
            }
        }
    }

    return cells.size() != 0 ? std::optional<WinLine>(cells) : std::nullopt;
}