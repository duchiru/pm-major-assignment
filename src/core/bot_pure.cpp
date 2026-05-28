/**
 * Bot (FP-style) implementation
 *
 * Mô tả:
 *   Bot được mô hình hoá theo phong cách FP:
 *     BotFn = (GameState, Rng&) → Move
 *   Polymorphism qua higher-order function thay vì class hierarchy.
 */

#include "bot_pure.h"

/* ---------- Importing ---------- */

#include <algorithm>
#include <climits>
#include <cmath>
#include <random>

#include "logic.h"
#include "pipeline.h"

namespace bot {

/* ---------- Internal helpers ---------- */

using ll = long long;

static const int MAX_DEPTH = 4;
static const int BOT_SCORE_INF = 1000;
static const int BOT_EVALUATE_SCORE = 10;

/**
 * Mô tả: Hàm lũy thừa (pure, không overflow check).
 */
static ll power(ll base, int exp) {
    ll result = 1;
    for (int i = exp; i > 0; i >>= 1) {
        if (i & 1) result *= base;
        base *= base;
    }
    return result;
}

/**
 * Mô tả: Đánh giá heuristic cho trạng thái bàn cờ.
 * Pure -- port từ level 2 BotLevel3::evaluate, dùng Board API.
 */
static ll evaluate(const Board& board, int goal, char selfSymbol, char opSymbol) {
    ll score = 0;
    int size = board.size;

    const int dx[] = {0, 1, 1, 1};
    const int dy[] = {1, 0, 1, -1};

    for (int dir = 0; dir < 4; dir++) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                int endX = i + dx[dir] * (goal - 1);
                int endY = j + dy[dir] * (goal - 1);

                if (endX < 0 || endX >= size || endY < 0 || endY >= size)
                    continue;

                int selfCount = 0;
                int opCount = 0;

                for (int k = 0; k < goal; k++) {
                    int x = i + dx[dir] * k;
                    int y = j + dy[dir] * k;

                    if (board.at(x, y) == selfSymbol) selfCount++;
                    else if (board.at(x, y) == opSymbol) opCount++;
                }

                if (selfCount > 0 && opCount == 0) {
                    score += power(BOT_EVALUATE_SCORE, selfCount);
                } else if (opCount > 0 && selfCount == 0) {
                    score -= power(BOT_EVALUATE_SCORE, opCount);
                }
            }
        }
    }

    return score;
}

/**
 * Mô tả: Minimax + Alpha-Beta Pruning.
 * Pure -- dùng core::applyMove (immutable) thay vì mutate board.
 */
static ll minimax(const Board& board, int size, int goal,
                  char botSymbol, char opSymbol,
                  int depth, bool isMax, ll alpha, ll beta) {
    // Kiểm tra kết thúc
    if (core::checkWin(board, botSymbol, goal, EndRule::OPEN_TWO))
        return BOT_SCORE_INF * (depth + 1);

    if (core::checkWin(board, opSymbol, goal, EndRule::OPEN_TWO))
        return -BOT_SCORE_INF * (depth + 1);

    if (core::checkDraw(board))
        return 0;

    if (depth <= 0)
        return evaluate(board, goal, botSymbol, opSymbol);

    if (isMax) {
        ll maxEval = LLONG_MIN;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                Move m{i, j};
                if (core::isValidMove(board, m)) {
                    Board newBoard = core::applyMove(board, m, botSymbol);
                    ll eval = minimax(newBoard, size, goal, botSymbol, opSymbol,
                                      depth - 1, false, alpha, beta);
                    maxEval = std::max(maxEval, eval);
                    alpha = std::max(alpha, eval);
                    if (alpha >= beta) return maxEval;
                }
            }
        }
        return maxEval;
    } else {
        ll minEval = LLONG_MAX;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                Move m{i, j};
                if (core::isValidMove(board, m)) {
                    Board newBoard = core::applyMove(board, m, opSymbol);
                    ll eval = minimax(newBoard, size, goal, botSymbol, opSymbol,
                                      depth - 1, true, alpha, beta);
                    minEval = std::min(minEval, eval);
                    beta = std::min(beta, eval);
                    if (alpha >= beta) return minEval;
                }
            }
        }
        return minEval;
    }
}

/* ---------- Easy ---------- */

/**
 * Mô tả: Bot Easy -- random pick.
 *
 * TODO (mức 1 -- task 2, "Higher-order function cho Bot LV1"):
 *
 *   Yêu cầu KHÔNG dùng for-loop tích luỹ. Có 2 cách triển khai gợi ý:
 *
 *   ── Cách 1: dùng `fp::pipe(...)` từ pipeline.h ──
 *       auto moves = core::enumerateValidMoves(state.board);
 *       if (moves.empty()) return INVALID_MOVE;
 *       std::uniform_int_distribution<int> dist(0, (int)moves.size() - 1);
 *       return moves[dist(rng)];
 *
 *   ── Cách 2: full pipeline + std::ranges ──
 *       using namespace std::ranges;
 *       auto cells   = core::enumerateCells(state.board);
 *       auto valid   = cells | views::filter([&](Move m){
 *                                  return core::isValidMove(state.board, m);
 *                              });
 *       auto vec     = std::vector<Move>(valid.begin(), valid.end());
 *       ...
 *
 *   Lưu ý:
 *     - KHÔNG được dùng `generator` toàn cục -- tham số `rng` bắt buộc.
 *     - Phải xử lý trường hợp board đầy → trả về INVALID_MOVE.
 */
Move easy(const GameState& state, Rng& rng) {
    auto moves = core::enumerateValidMoves(state.board);
    if (moves.empty()) return INVALID_MOVE;

    std::uniform_int_distribution<int> dist(0, static_cast<int>(moves.size()) - 1);
    return moves[dist(rng)];
}

/* ---------- Medium ---------- */

/**
 * Mô tả: Bot Medium -- heuristic 3 bước.
 *
 * TODO (Mức 2 -- Bot, sinh viên tự quyết áp dụng FP vào bot nào):
 *   Khuyến khích bộc lộ rõ:
 *     - Composition: ráp 3 bước lại thành 1 pipeline.
 *     - Higher-order: viết hàm `tryFirstNonEmpty(strategies)` trả về Move
 *       đầu tiên không phải INVALID_MOVE.
 *     - Closure / currying: cố định `state` & `goal` cho các predicate.
 *
 *   Có thể fallback `easy(state, rng)` nếu chưa làm.
 */
Move medium(const GameState& state, int goal, Rng& rng) {
    const Board& board = state.board;
    char sym = core::symbolOf(state.currentPlayer);
    char opSym = core::opponentOf(sym);

    auto validMoves = core::enumerateValidMoves(board);
    if (validMoves.empty()) return INVALID_MOVE;

    // 1. Tìm nước thắng -- dùng fp::filter (HOF)
    auto winningMoves = fp::filter([&](Move m) {
        Board nb = core::applyMove(board, m, sym);
        return core::checkWin(nb, sym, goal, EndRule::OPEN_TWO);
    })(validMoves);

    if (!winningMoves.empty()) return winningMoves[0];

    // 2. Tìm nước chặn -- dùng fp::filter (HOF)
    auto blockingMoves = fp::filter([&](Move m) {
        Board nb = core::applyMove(board, m, opSym);
        return core::checkWin(nb, opSym, goal, EndRule::OPEN_TWO);
    })(validMoves);

    if (!blockingMoves.empty()) return blockingMoves[0];

    // 3. Ưu tiên trung tâm
    int size = board.size;
    double center = (size - 1) / 2.0;
    std::vector<Move> preferred;

    for (int x = static_cast<int>(std::floor(center));
         x <= static_cast<int>(std::ceil(center)); x++) {
        for (int y = static_cast<int>(std::floor(center));
             y <= static_cast<int>(std::ceil(center)); y++) {
            Move m{x, y};
            if (core::isValidMove(board, m))
                preferred.push_back(m);
        }
    }

    // 4. Ưu tiên ô kề quân đã đặt
    for (const auto& m : validMoves) {
        bool adjacent = false;
        for (int di = -1; di <= 1 && !adjacent; di++) {
            for (int dj = -1; dj <= 1 && !adjacent; dj++) {
                int ni = m.row + di, nj = m.col + dj;
                if (board.inRange(ni, nj) && board.at(ni, nj) != EMPTY_CELL)
                    adjacent = true;
            }
        }
        if (adjacent) preferred.push_back(m);
    }

    if (!preferred.empty()) {
        std::uniform_int_distribution<int> dist(0, static_cast<int>(preferred.size()) - 1);
        return preferred[dist(rng)];
    }

    // 5. Fallback easy
    return easy(state, rng);
}

/* ---------- Hard ---------- */

/**
 * Mô tả: Bot Hard -- minimax / heuristic mạnh.
 *
 * TODO:
 *   - Mức 2: ưu tiên cài bằng FP (currying, composition).
 *   - Mức 3 nhánh B: TÁCH `evaluateAllMoves` thành pure function rời
 *     trong `core/parallel.h` để chạy được song song.
 */
Move hard(const GameState& state, int goal, Rng& rng) {
    const Board& board = state.board;
    int size = board.size;
    char sym = core::symbolOf(state.currentPlayer);
    char opSym = core::opponentOf(sym);

    int adjustedDepth = MAX_DEPTH - static_cast<int>(std::log2(size) + 1);
    adjustedDepth = std::max(adjustedDepth, 2);

    ll bestScore = LLONG_MIN;
    Move bestMove = INVALID_MOVE;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            Move m{i, j};
            if (core::isValidMove(board, m)) {
                Board nb = core::applyMove(board, m, sym);
                ll score = minimax(nb, size, goal, sym, opSym,
                                   adjustedDepth, false, LLONG_MIN, LLONG_MAX);

                if (score > bestScore) {
                    bestScore = score;
                    bestMove = m;
                }
            }
        }
    }

    // Fallback
    if (bestMove == INVALID_MOVE)
        return medium(state, goal, rng);

    return bestMove;
}

/* ---------- Factory ---------- */

/**
 * Mô tả: Trả về BotFn ứng với level.
 *
 * TODO (mức 1 -- task 2, đi kèm easy()):
 *   - Mỗi nhánh trả về một LAMBDA (đóng gói goal vào closure):
 *
 *     case EASY:
 *         return [](const GameState& s, Rng& r) { return easy(s, r); };
 *
 *     case MEDIUM:
 *         return [goal](const GameState& s, Rng& r) {
 *             return medium(s, goal, r);
 *         };
 *
 *     case HARD:
 *         return [goal](const GameState& s, Rng& r) {
 *             return hard(s, goal, r);
 *         };
 *
 *     default:
 *         return [](const GameState&, Rng&) { return INVALID_MOVE; };
 *
 * Đây là cũng là một ví dụ HOF (factory trả về function).
 */
BotFn makeBot(BotLevel level, int goal) {
    switch (level) {
        case BotLevel::EASY:
            return [](const GameState& s, Rng& r) { return easy(s, r); };

        case BotLevel::MEDIUM:
            return [goal](const GameState& s, Rng& r) {
                return medium(s, goal, r);
            };

        case BotLevel::HARD:
            return [goal](const GameState& s, Rng& r) {
                return hard(s, goal, r);
            };

        default:
            return [](const GameState&, Rng&) { return INVALID_MOVE; };
    }
}

}  // namespace bot
