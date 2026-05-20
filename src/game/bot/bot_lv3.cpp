/**
 * Bot LV3 cpp implementation
 *
 * Thuật toán Minimax với Alpha-Beta Pruning:
 *
 *   Minimax là thuật toán tìm kiếm cây trò chơi, trong đó:
 *   - Người chơi tối đa hóa (maximizer) cố gắng chọn nước đi có điểm CAO nhất
 *   - Đối thủ tối thiểu hóa (minimizer) cố gắng chọn nước đi có điểm THẤP nhất
 *   - Thuật toán đệ quy duyệt toàn bộ cây trạng thái đến độ sâu giới hạn
 *
 *   Alpha-Beta Pruning tối ưu hóa bằng cách cắt tỉa nhánh không cần thiết:
 *   - alpha: giá trị TỐT NHẤT mà maximizer đã tìm được trên đường đi hiện tại
 *   - beta:  giá trị TỐT NHẤT mà minimizer đã tìm được trên đường đi hiện tại
 *   - Khi alpha >= beta, ta biết nhánh hiện tại sẽ KHÔNG BAO GIỜ được chọn
 *     bởi một trong hai người chơi → cắt bỏ (prune) toàn bộ nhánh con
 *   - Điều này giảm đáng kể số nút cần đánh giá, từ O(b^d) xuống O(b^(d/2))
 *     trong trường hợp tốt nhất (với b là branching factor, d là depth)
 */

#include "bot_lv3.h"

/* ---------- Importing ---------- */

#include <algorithm>
#include <climits>
#include <sstream>

#include "../../utils/helper.h"
#include "../../utils/logger.h"
#include "../logic.h"

/* ---------- Definitions ---------- */

/**
 * Mô tả:
 *   Constructor của BotLevel3.
 */
BotLevel3::BotLevel3(const BotLevel &_level, const char &_symbol)
    : BotLevel2(_level, _symbol) {}

/**
 * Mô tả:
 *   Destructor của BotLevel3.
 */
BotLevel3::~BotLevel3() {}

/**
 * Mô tả:
 *   Trả về nước đi của bot HARD.
 *   Gọi hard_level để tìm nước đi tối ưu bằng minimax.
 *   Nếu hard_level trả về (-1, -1) → fallback về logic của BotLevel2.
 */
pII BotLevel3::getMove(char board[][BOARD_N_MAX], const int size,
                       const int goal) {
  pII move = hard_level(board, size, goal);

  // Fallback về BotLevel2 nếu hard_level không tìm được nước đi hợp lệ
  if (move.first == -1 || move.second == -1) {
    return BotLevel2::getMove(board, size, goal);
  }

  return move;
}

/**
 * Mô tả:
 *   Logic nâng cao cho bot HARD sử dụng thuật toán Minimax + Alpha-Beta
 * Pruning.
 *
 *   - Bước 1: Duyệt tất cả các nước đi hợp lệ
 *   - Bước 2: Với mỗi nước đi, gọi minimax để đánh giá điểm
 *   - Bước 3: Chọn nước đi có điểm cao nhất (tốt nhất cho bot)
 *
 * Đầu vào:
 *   - board: trạng thái bàn cờ hiện tại
 *   - size: kích thước bàn cờ
 *   - goal: số quân liên tiếp cần để thắng
 *
 * Đầu ra:
 *   - pII: nước đi tối ưu (row, col), hoặc (-1, -1) nếu không còn nước đi
 */
pII BotLevel3::hard_level(char board[][BOARD_N_MAX], const int size,
                          const int goal) {
  int adjusted_max_depth = MAX_DEPTH - (int)(std::log2(size) + 1);
  adjusted_max_depth = std::max(adjusted_max_depth, 2);

  ll bestScore = LLONG_MIN;
  pII bestMove = {-1, -1};

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (Logic::isValidMove(board, size, i, j)) {
        board[i][j] = symbol;
        ll score = minimax(board, size, goal, adjusted_max_depth, false, LLONG_MIN, LLONG_MAX);
        board[i][j] = '-';

        if (score > bestScore) {
          bestScore = score;
          bestMove = {i, j};
        }
      }
    }
  }

  Logger::debug("Hard level best score: " + std::to_string(bestScore) +
                " at (" + std::to_string(bestMove.first) + "," +
                std::to_string(bestMove.second) + ")");

  return bestMove;
}

/**
 * Mô tả:
 *   Thuật toán Minimax với Alpha-Beta Pruning.
 *
 *   Đệ quy duyệt cây trạng thái trò chơi:
 *   - Nếu đạt trạng thái kết thúc (thắng/thua/hòa) hoặc hết độ sâu → trả về
 * evaluate()
 *   - Nếu isMax (lượt bot): tìm nước đi có điểm CAO nhất
 *   - Nếu !isMax (lượt đối thủ): tìm nước đi có điểm THẤP nhất
 *
 *   Alpha-Beta Pruning:
 *   - alpha: giới hạn dưới mà maximizer đã đảm bảo được
 *   - beta:  giới hạn trên mà minimizer đã đảm bảo được
 *   - Khi alpha >= beta → cắt nhánh (prune), vì nhánh này không thể
 *     tạo ra kết quả tốt hơn cho cả hai bên
 *
 * Đầu vào:
 *   - board: trạng thái bàn cờ
 *   - size: kích thước bàn cờ
 *   - goal: số quân liên tiếp cần để thắng
 *   - depth: độ sâu còn lại cho phép duyệt
 *   - isMax: true nếu đang là lượt của bot (maximizer)
 *   - alpha: giá trị alpha (giới hạn dưới)
 *   - beta: giá trị beta (giới hạn trên)
 *
 * Đầu ra:
 *   - ll: điểm đánh giá tốt nhất cho trạng thái hiện tại
 */
ll BotLevel3::minimax(char board[][BOARD_N_MAX], const int size, const int goal,
                      int depth, bool isMax, ll alpha, ll beta) {
  // Kiểm tra trạng thái kết thúc: bot thắng
  if (Logic::checkWin(board, size, symbol, goal, EndRule::OPEN_TWO)) {
    // Thưởng thêm điểm cho chiến thắng sớm (depth càng cao = thắng càng sớm)
    return SCORE_INF * (depth + 1);
  }

  // Kiểm tra trạng thái kết thúc: đối thủ thắng
  if (Logic::checkWin(board, size, op_symbol, goal, EndRule::OPEN_TWO)) {
    // Phạt nặng hơn cho thua sớm (depth càng cao = thua càng sớm)
    return -SCORE_INF * (depth + 1);
  }

  // Kiểm tra hòa
  if (Logic::checkDraw(board, size)) {
    return 0;
  }

  // Hết độ sâu → đánh giá heuristic
  if (depth <= 0) {
    return evaluate(board, size, goal, symbol, op_symbol);
  }

  if (isMax) {
    // Lượt của bot (maximizer): tìm điểm CAO nhất
    ll maxEval = LLONG_MIN;

    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (Logic::isValidMove(board, size, i, j)) {
          board[i][j] = symbol;
          ll eval = minimax(board, size, goal, depth - 1, false, alpha, beta);
          board[i][j] = '-';

          maxEval = std::max(maxEval, eval);
          alpha = std::max(alpha, eval);

          // Alpha-Beta Pruning: cắt nhánh khi alpha >= beta
          // Vì minimizer ở tầng trên sẽ KHÔNG BAO GIỜ chọn nhánh này
          if (alpha >= beta) {
            return maxEval;
          }
        }
      }
    }

    return maxEval;
  } else {
    // Lượt của đối thủ (minimizer): tìm điểm THẤP nhất
    ll minEval = LLONG_MAX;

    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (Logic::isValidMove(board, size, i, j)) {
          board[i][j] = op_symbol;
          ll eval = minimax(board, size, goal, depth - 1, true, alpha, beta);
          board[i][j] = '-';

          minEval = std::min(minEval, eval);
          beta = std::min(beta, eval);

          // Alpha-Beta Pruning: cắt nhánh khi alpha >= beta
          // Vì maximizer ở tầng trên sẽ KHÔNG BAO GIỜ chọn nhánh này
          if (alpha >= beta) {
            return minEval;
          }
        }
      }
    }

    return minEval;
  }
}

/**
 * Mô tả:
 *   Hàm đánh giá heuristic cho trạng thái bàn cờ.
 *
 *   Phân tích tất cả các chuỗi quân liên tiếp theo 4 hướng:
 *   - Ngang (horizontal)
 *   - Dọc (vertical)
 *   - Chéo chính (main diagonal ↘)
 *   - Chéo phụ (anti-diagonal ↗)
 *
 *   Điểm được tính theo cấp số nhân: chuỗi dài hơn có giá trị cao hơn
 *   nhiều lần so với chuỗi ngắn (EVALUATE_SCORE^count).
 *   Bot được cộng điểm, đối thủ bị trừ điểm.
 *
 * Đầu vào:
 *   - board: trạng thái bàn cờ
 *   - size: kích thước bàn cờ
 *   - goal: số quân cần để thắng
 *   - selfSymbol: ký hiệu của bot
 *   - opSymbol: ký hiệu của đối thủ
 *
 * Đầu ra:
 *   - ll: điểm đánh giá (dương = bot có lợi, âm = đối thủ có lợi)
 */
ll BotLevel3::evaluate(char board[][BOARD_N_MAX], const int size,
                       const int goal, const char selfSymbol,
                       const char opSymbol) {
  ll score = 0;

  const int dx[] = {0, 1, 1, 1};
  const int dy[] = {1, 0, 1, -1};

  for (int dir = 0; dir < 4; dir++) {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        // Kiểm tra xem chuỗi goal ô có nằm trong bàn cờ không
        int endX = i + dx[dir] * (goal - 1);
        int endY = j + dy[dir] * (goal - 1);

        if (endX < 0 || endX >= size || endY < 0 || endY >= size)
          continue;

        // Đếm số quân của bot và đối thủ trong chuỗi goal ô
        int selfCount = 0;
        int opCount = 0;

        for (int k = 0; k < goal; k++) {
          int x = i + dx[dir] * k;
          int y = j + dy[dir] * k;

          if (board[x][y] == selfSymbol)
            selfCount++;
          else if (board[x][y] == opSymbol)
            opCount++;
        }

        // Chỉ tính điểm khi chuỗi chứa quân của MỘT bên (không bị chặn)
        if (selfCount > 0 && opCount == 0) {
          // Chuỗi thuần của bot: điểm tăng theo cấp số nhân
          score += power(EVALUATE_SCORE, selfCount);
        } else if (opCount > 0 && selfCount == 0) {
          // Chuỗi thuần của đối thủ: trừ điểm
          score -= power(EVALUATE_SCORE, opCount);
        }
      }
    }
  }

  return score;
}