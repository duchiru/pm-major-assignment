/**
 * Ternimal Renderer cpp implementation
 *
 */

#include "renderer.h"

/* ---------- Importing ---------- */

#include <format>
#include <iostream>

#include "../game/setup.h"
#include "../utils/config.h"

/* ---------- Definitions ---------- */

/**
 * Mô tả:
 *   Constructor của TerminalRenderer.
 *
 * TODO:
 *   - Khởi tạo trạng thái ban đầu nếu cần
 */
TerminalRenderer::TerminalRenderer() : I_Renderer() {
  // TODO: init state
}

/**
 * Mô tả:
 *   Destructor của TerminalRenderer.
 *
 * TODO:
 *   - Giải phóng tài nguyên nếu có
 */
TerminalRenderer::~TerminalRenderer() {
  // TODO: cleanup nếu cần
}

/**
 * Mô tả:
 *   Khởi tạo renderer với config.
 *
 * TODO:
 *   - Đọc config
 *   - Thiết lập môi trường hiển thị
 */
void TerminalRenderer::init(const RunConfig &config) {
  if (config.interactive)
    clearScreen();
}

/**
 * Mô tả:
 *   Xóa màn hình terminal.
 *
 * TODO:
 *   - Sử dụng ANSI escape sequence để clear screen
 */
void TerminalRenderer::clearScreen() {
  std::cout << "\n"
            << "\x1B[2J\x1B[H" << "\n";
}

void TerminalRenderer::renderFrame() {}

/**
 * Mô tả:
 *   Hiển thị menu lựa chọn.
 *
 * TODO:
 *   - Bước 1: kiểm tra selectType
 *   - Bước 2: in menu tương ứng
 *   - Trường hợp biên: selectType không hợp lệ
 */
void TerminalRenderer::showSelectMenu(SelectType selectType, int context) {
  switch (selectType) {
  case SelectType::TITLE_UI:
    std::cout << std::format(">----- Tic-tac-toe [Console v{}] -----<\n\n",
                             VERSION);
    break;

  case SelectType::SIZE_UI:
    std::cout << std::format("[*] Enter board size (NxN, {} <= N <= {}): ",
                             BOARD_N_MIN, BOARD_N_MAX);
    break;

  case SelectType::GOAL_UI:
    std::cout << std::format("[*] Enter goal count (G, {} <= G <= {}): ",
                             BOARD_N_MIN, std::min(context, GOAL_MAX));
    break;

  case SelectType::GAME_MODE_UI:
    std::cout << "[*] Select game mode [(1) PvP | (2) PvE | (3) EvE]: ";
    break;

  case SelectType::BOT_LEVEL_UI:
    std::cout
        << "[*] Select bot difficulty [(1) EASY | (2) MEDIUM | (3) HARD]: ";
    break;

  case SelectType::MUL_BOT_LEVEL_UI:
    std::cout << std::format(
        "[*] Select difficulty for bot {} [(1) EASY | (2) MEDIUM | (3) HARD]: ",
        context + 1);
    break;

  case SelectType::PLAYER_UI:
    std::cout << std::format("[*] Enter your move (row col): ");
    break;

  default:
    break;
  }
}

/**
 * Mô tả:
 *   Hiển thị thông báo lựa chọn không hợp lệ.
 *
 * TODO:
 *   - Bước 1: xác định loại lỗi
 *   - Bước 2: in thông báo tương ứng
 */
void TerminalRenderer::showInvalidSelect(SelectType selectType, int context) {
  switch (selectType) {
  case SelectType::SIZE_UI:
    std::cout << std::format(
        "[!] {} is an invalid board size, please enter another value\n",
        context);
    break;

  case SelectType::GOAL_UI:
    std::cout << std::format(
        "[!] {} is an invalid goal count, please enter another value\n",
        context);
    break;

  case SelectType::GAME_MODE_UI:
    std::cout << std::format("[!] We can not recognize your game mode ({}), "
                             "please enter another value\n",
                             modeToString(context));
    break;

  default:
    std::cout << "[!] Invalid selection.\n";
    break;
  }
}

/**
 * Mô tả:
 *   Hiển thị thông báo lựa chọn hợp lệ.
 *
 * TODO:
 *   - Bước 1: xác định loại select
 *   - Bước 2: in thông báo xác nhận
 */
void TerminalRenderer::showValidSelect(SelectType selectType, int context) {
  switch (selectType) {
  case SelectType::SIZE_UI:
    std::cout << std::format("[v] Board size set to {}x{}\n", context, context);
    break;

  case SelectType::GOAL_UI:
    std::cout << std::format("[v] Goal count set to {}\n", context);
    break;

  case SelectType::GAME_MODE_UI:
    std::cout << std::format("[v] Game mode set to {}\n",
                             modeToString(context));
    break;

  case SelectType::BOT_LEVEL_UI:
    std::cout << std::format("[v] Bot level set to {}\n", botToString(context));
    break;

  case SelectType::MUL_BOT_LEVEL_UI:
    std::cout << std::format("[v] Bot level set to {}\n", botToString(context));
    break;

  default:
    break;
  }
}

/**
 * Mô tả:
 *   Hiển thị bàn cờ.
 *
 * TODO:
 *   - Bước 1: in header
 *   - Bước 2: in trục tọa độ
 *   - Bước 3: duyệt board và in từng ô
 */
void TerminalRenderer::displayBoard(const char board[][BOARD_N_MAX],
                                    const int size) {
  std::cout << "   ";
  for (int i = 0; i < size; i++)
    std::cout << "\033[46;30m" << (i / 10) << "\033[0m" << ' ';
  std::cout << '\n';

  std::cout << "   ";
  for (int i = 0; i < size; i++)
    std::cout << "\033[46;30m" << (i % 10) << "\033[0m" << ' ';
  std::cout << '\n';

  std::cout << "  *";
  for (int i = 0; i < size * 2 - 1; i++)
    std::cout << '=';
  std::cout << "*\n";

  for (int i = 0; i < size; i++) {
    std::cout << "\033[46;30m" << std::format("{:02}", i) << "\033[0m" << "|";
    for (int j = 0; j < size; j++) {
      std::cout << board[i][j];
      if (j < size - 1)
        std::cout << ' ';
    }
    std::cout << "|\n";
  }

  std::cout << "  *";
  for (int i = 0; i < size * 2 - 1; i++)
    std::cout << '=';
  std::cout << "*\n";
}

/**
 * Mô tả:
 *   Hiển thị thông tin player hiện tại.
 *
 * TODO:
 *   - In player index
 *   - Nếu là bot thì thêm label "(Bot)"
 */
void TerminalRenderer::showPlayer(int player, bool is_bot) {
  std::cout << std::format("It's turn for player {}{}.", player + 1,
                           is_bot ? " (bot)" : "")
            << '\n';
}

/**
 * Mô tả:
 *   Hiển thị nước đi vừa thực hiện.
 *
 * TODO:
 *   - In ra tọa độ (row, col)
 */
void TerminalRenderer::showMove(const int row, const int col) {
  std::cout << std::format("Move made at ({}, {})\n", row, col);
}

/**
 * Mô tả:
 *   Hiển thị thông báo nước đi không hợp lệ.
 *
 * TODO:
 *   - In message lỗi
 */
void TerminalRenderer::showInvalidMove() {
  std::cout << "[!] Invalid move. Please try again.\n";
}

/**
 * Mô tả:
 *   Hiển thị kết quả game.
 *
 * TODO:
 *   - Bước 1: kiểm tra draw hoặc winner
 *   - Bước 2: in kết quả
 *   - Trường hợp biên: winner = -1
 */
void TerminalRenderer::showResult(const int winner, const bool is_bot,
                                  const WinLine *winLine) {
  if (winner == -1) {
    std::cout << "It's a draw!\n";
  } else {
    std::cout << std::format("Player {} {} wins!\n", winner + 1,
                             is_bot ? "(Bot)" : "");
    if (winLine != nullptr) {
      std::cout << "Winning line: ";
      for (const pII &cell : winLine->cells)
        std::cout << std::format("({}, {}) ", cell.first, cell.second);
      std::cout << '\n';
    }
  }
}

/**
 * Mô tả:
 *   In kết quả đơn giản (judge mode).
 *
 * TODO:
 *   - In winner và số lượt
 */
void TerminalRenderer::printResult(const GameResult &gameResult) {
  std::cout << gameResult.winner << ' ' << gameResult.turns << '\n';
}

/**
 * Mô tả:
 *   Đóng renderer.
 *
 * TODO:
 *   - Giải phóng tài nguyên nếu có
 */
void TerminalRenderer::close() {
  // TODO: cleanup renderer
}