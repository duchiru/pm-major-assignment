/**
 * Core Logic implementation (PURE)
 *
 * Mô tả:
 *   Các hàm logic game viết theo phong cách FP -- pure, không IO, không Logger.
 *
 * Quy tắc:
 *   - KHÔNG include <iostream>, KHÔNG dùng Logger.
 *   - KHÔNG dùng biến toàn cục, KHÔNG random.
 *   - Trả về giá trị mới thay vì sửa tham số.
 *   - Sử dụng fp::pipe, fp::filter, fp::map, fp::reduce (pipeline.h).
 *   - Sử dụng đệ quy (tail recursion) cho enumerateCells.
 */

#include "logic.h"

/* ---------- Importing ---------- */

#include <functional>

#include "pipeline.h"

namespace core {

/* ---------- Construction ---------- */

/**
 * Mô tả: Tạo board rỗng kích thước size × size.
 *
 * Đầu vào: size -- kích thước.
 * Đầu ra : Board mới, mọi ô = EMPTY_CELL.
 */
Board initBoard(int size) {
    Board b;
    b.size = size;
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            b.grid[r][c] = EMPTY_CELL;
    return b;
}

/* ---------- Queries ---------- */

/**
 * Mô tả: Nước đi có hợp lệ?
 *
 * Hàm này phải PURE -- không log, không side-effect.
 */
bool isValidMove(const Board& board, Move move) {
    return board.inRange(move.row, move.col)
        && board.at(move.row, move.col) == EMPTY_CELL;
}

/**
 * Mô tả: Liệt kê tất cả các Move (r, c) trên board.
 *
 * Đây là "nguồn" cho các pipeline filter/map/reduce phía sau.
 * Vì input là Board (không phải sẵn 1 container), enumerateCells là phép
 * "khởi tạo container" -- không cần dùng pipeline.h ở chính nó.
 */
std::vector<Move> enumerateCells(const Board& board) {
    // Tail-recursive helper: dùng lambda + std::function
    std::function<std::vector<Move>(int, int, std::vector<Move>)> helper =
        [&](int r, int c, std::vector<Move> acc) -> std::vector<Move> {
            if (r >= board.size) return acc;  // base case: đã duyệt hết

            acc.push_back({r, c});

            // Chuyển sang ô tiếp theo
            if (c + 1 < board.size)
                return helper(r, c + 1, std::move(acc));
            else
                return helper(r + 1, 0, std::move(acc));
        };

    return helper(0, 0, {});
}

/**
 * Mô tả: Liệt kê các nước đi hợp lệ (= các ô trống).
 *
 * Đây là chỗ rất phù hợp để DÙNG `pipeline.h` -- input là 1 vector<Move>
 * (từ enumerateCells), output cũng là vector<Move>. Đúng dạng filter HOF.
 */
std::vector<Move> enumerateValidMoves(const Board& board) {
    auto pickValid = fp::pipe(
        [](const Board& b) { return enumerateCells(b); },
        fp::filter([&](Move m) {
            return isValidMove(board, m);
        })
    );

    return pickValid(board);
}

/**
 * Mô tả: Ô (x, y) có phải đầu mở của chuỗi `symbol` không?
 *
 * Pure: chỉ đọc board, không log, không global.
 */
bool isEmptyHead(const Board& board, int x, int y, char symbol) {
    return (!board.inRange(x, y))
        || (board.at(x, y) == EMPTY_CELL)
        || (board.at(x, y) == symbol);
}

/* ---------- Transitions ---------- */

/**
 * Mô tả: Tạo Board MỚI sau khi đặt symbol vào ô move.
 *
 * Đây là viên gạch nền của tính BẤT BIẾN -- nhớ KHÔNG sửa `board` đầu vào.
 */
Board applyMove(const Board& board, Move move, char symbol) {
    Board newBoard = board;  // copy toàn bộ
    newBoard.grid[move.row][move.col] = symbol;
    return newBoard;
}

/**
 * Mô tả: Tính GameState MỚI sau 1 lượt đi.
 *
 * Đây là hàm "fold step" -- Engine ở Mức 2 chỉ cần GỌI hàm này, KHÔNG mutate.
 * Mỗi lượt sinh ra một GameState mới, state cũ vẫn còn nguyên.
 */
GameState applyTurn(const GameState& state, Move move, int goal) {
    // Bước 1: Xác định symbol
    char sym = symbolOf(state.currentPlayer);

    // Bước 2: Tạo board mới (immutable)
    Board newBoard = applyMove(state.board, move, sym);

    // Bước 3: Tạo next state
    GameState next;
    next.board = newBoard;
    next.turn = state.turn + 1;

    // Bước 4: Kiểm tra kết thúc
    if (checkWin(newBoard, sym, goal)) {
        next.winner = state.currentPlayer;
        next.isFinished = true;
        next.currentPlayer = state.currentPlayer;  // giữ nguyên player thắng
    } else if (checkDraw(newBoard)) {
        next.winner = DRAW_RESULT;
        next.isFinished = true;
        next.currentPlayer = state.currentPlayer;
    } else {
        // Bước 5: Chưa kết thúc → đổi lượt
        next.winner = -1;
        next.isFinished = false;
        next.currentPlayer = 1 - state.currentPlayer;
    }

    return next;
}

/* ---------- End-of-game ---------- */

/**
 * Mô tả: Có người thắng?
 *
 * Có thể là ứng viên cho chứng minh ở nhánh A (mức 3).
 */
bool checkWin(const Board& board, char symbol, int goal, EndRule /*rule*/) {
    int size = board.size;

    // Check horizontal
    for (int i = 0; i < size; i++) {
        int cnt = 0;
        for (int j = 0; j < size; j++) {
            if (board.at(i, j) != symbol) {
                cnt = 0;
                continue;
            }
            cnt++;
            if (cnt >= goal
                && isEmptyHead(board, i, j - goal, symbol)
                && isEmptyHead(board, i, j + 1, symbol))
                return true;
        }
    }

    // Check vertical
    for (int j = 0; j < size; j++) {
        int cnt = 0;
        for (int i = 0; i < size; i++) {
            if (board.at(i, j) != symbol) {
                cnt = 0;
                continue;
            }
            cnt++;
            if (cnt >= goal
                && isEmptyHead(board, i - goal, j, symbol)
                && isEmptyHead(board, i + 1, j, symbol))
                return true;
        }
    }

    // Check main diagonal (top-left to bottom-right)
    for (int sj = 0; sj < size; sj++) {
        int cnt = 0;
        for (int i = 0, j = sj; i < size && j < size; i++, j++) {
            if (board.at(i, j) != symbol) {
                cnt = 0;
                continue;
            }
            cnt++;
            if (cnt >= goal
                && isEmptyHead(board, i - goal, j - goal, symbol)
                && isEmptyHead(board, i + 1, j + 1, symbol))
                return true;
        }
    }
    for (int si = 1; si < size; si++) {
        int cnt = 0;
        for (int i = si, j = 0; i < size && j < size; i++, j++) {
            if (board.at(i, j) != symbol) {
                cnt = 0;
                continue;
            }
            cnt++;
            if (cnt >= goal
                && isEmptyHead(board, i - goal, j - goal, symbol)
                && isEmptyHead(board, i + 1, j + 1, symbol))
                return true;
        }
    }

    // Check anti diagonal (top-right to bottom-left)
    for (int sj = 0; sj < size; sj++) {
        int cnt = 0;
        for (int i = 0, j = sj; i < size && j >= 0; i++, j--) {
            if (board.at(i, j) != symbol) {
                cnt = 0;
                continue;
            }
            cnt++;
            if (cnt >= goal
                && isEmptyHead(board, i - goal, j + goal, symbol)
                && isEmptyHead(board, i + 1, j - 1, symbol))
                return true;
        }
    }
    for (int si = 1; si < size; si++) {
        int cnt = 0;
        for (int i = si, j = size - 1; i < size && j >= 0; i++, j--) {
            if (board.at(i, j) != symbol) {
                cnt = 0;
                continue;
            }
            cnt++;
            if (cnt >= goal
                && isEmptyHead(board, i - goal, j + goal, symbol)
                && isEmptyHead(board, i + 1, j - 1, symbol))
                return true;
        }
    }

    return false;
}

/**
 * Mô tả: Hoà? (không còn ô trống và chưa có người thắng)
 *
 * PURE.
 */
bool checkDraw(const Board& board) {
    return countSymbol(board, EMPTY_CELL) == 0;
}

/**
 * Mô tả: Lấy đường thắng (nếu có).
 */
std::optional<WinLine> getWinLine(const Board& board, char symbol,
                                   int goal, EndRule /*rule*/) {
    int size = board.size;
    std::vector<pII> cells;

    // Check horizontal
    for (int i = 0; i < size && cells.empty(); i++) {
        int cnt = 0;
        for (int j = 0; j < size; j++) {
            if (board.at(i, j) != symbol) {
                cnt = 0;
                continue;
            }
            cnt++;
            if (cnt >= goal
                && isEmptyHead(board, i, j - goal, symbol)
                && isEmptyHead(board, i, j + 1, symbol)) {
                for (int k = 0; k < goal; k++)
                    cells.push_back({i, j - k});
            }
        }
    }

    // Check vertical
    for (int j = 0; j < size && cells.empty(); j++) {
        int cnt = 0;
        for (int i = 0; i < size; i++) {
            if (board.at(i, j) != symbol) {
                cnt = 0;
                continue;
            }
            cnt++;
            if (cnt >= goal
                && isEmptyHead(board, i - goal, j, symbol)
                && isEmptyHead(board, i + 1, j, symbol)) {
                for (int k = 0; k < goal; k++)
                    cells.push_back({i - k, j});
            }
        }
    }

    // Check main diagonal
    for (int sj = 0; sj < size && cells.empty(); sj++) {
        int cnt = 0;
        for (int i = 0, j = sj; i < size && j < size; i++, j++) {
            if (board.at(i, j) != symbol) {
                cnt = 0;
                continue;
            }
            cnt++;
            if (cnt >= goal
                && isEmptyHead(board, i - goal, j - goal, symbol)
                && isEmptyHead(board, i + 1, j + 1, symbol)) {
                for (int k = 0; k < goal; k++)
                    cells.push_back({i - k, j - k});
            }
        }
    }
    for (int si = 1; si < size && cells.empty(); si++) {
        int cnt = 0;
        for (int i = si, j = 0; i < size && j < size; i++, j++) {
            if (board.at(i, j) != symbol) {
                cnt = 0;
                continue;
            }
            cnt++;
            if (cnt >= goal
                && isEmptyHead(board, i - goal, j - goal, symbol)
                && isEmptyHead(board, i + 1, j + 1, symbol)) {
                for (int k = 0; k < goal; k++)
                    cells.push_back({i - k, j - k});
            }
        }
    }

    // Check anti diagonal
    for (int sj = 0; sj < size && cells.empty(); sj++) {
        int cnt = 0;
        for (int i = 0, j = sj; i < size && j >= 0; i++, j--) {
            if (board.at(i, j) != symbol) {
                cnt = 0;
                continue;
            }
            cnt++;
            if (cnt >= goal
                && isEmptyHead(board, i - goal, j + goal, symbol)
                && isEmptyHead(board, i + 1, j - 1, symbol)) {
                for (int k = 0; k < goal; k++)
                    cells.push_back({i - k, j + k});
            }
        }
    }
    for (int si = 1; si < size && cells.empty(); si++) {
        int cnt = 0;
        for (int i = si, j = size - 1; i < size && j >= 0; i++, j--) {
            if (board.at(i, j) != symbol) {
                cnt = 0;
                continue;
            }
            cnt++;
            if (cnt >= goal
                && isEmptyHead(board, i - goal, j + goal, symbol)
                && isEmptyHead(board, i + 1, j - 1, symbol)) {
                for (int k = 0; k < goal; k++)
                    cells.push_back({i - k, j + k});
            }
        }
    }

    return cells.empty() ? std::nullopt : std::optional<WinLine>(WinLine{cells});
}

/* ---------- Helpers ---------- */

/**
 * Mô tả: Đếm số ô = symbol.
 *
 * Đây là hàm rất phù hợp để CHỨNG MINH ở nhánh A (mức 3):
 *   countSymbol(applyMove(b, m, s), s) = countSymbol(b, s) + 1
 *   với điều kiện isValidMove(b, m) == true.
 */
int countSymbol(const Board& board, char symbol) {
    auto counter = fp::pipe(
        [](const Board& b) { return enumerateCells(b); },
        fp::filter([&](Move m) {
            return board.at(m.row, m.col) == symbol;
        }),
        fp::reduce(0, [](int acc, const Move& /*m*/) {
            return acc + 1;
        })
    );

    return counter(board);
}

}  // namespace core
