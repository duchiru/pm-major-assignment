/**
 * SDL Interaction cpp implementation
 *
 */

#include "interaction.h"
#include <SDL_events.h>

/* ---------- Definitions ---------- */

/**
 * Mô tả: Kiểm tra sự kiện quit từ SDL.
 * Đầu vào:
 *   - e: SDL_Event hiện tại.
 * Đầu ra:
 *   - false nếu không phải quit event.
 * Tác dụng phụ:
 *   - Ném QuitException nếu nhận được sự kiện thoát.
 */
void SDLInteraction::waitForQuit(SDL_Event &e) {
  if (e.type == SDL_QUIT) {
    throw QuitException();
  }
}

/**
 * Mô tả: Khởi tạo đối tượng SDLInteraction.
 * Đầu vào: Không có.
 * Đầu ra: Không có.
 * Tác dụng phụ: Không có.
 */
SDLInteraction::SDLInteraction() {}

/**
 * Mô tả: Destructor của SDLInteraction.
 * Đầu vào: Không có.
 * Đầu ra: Không có.
 * Tác dụng phụ: Không có.
 */
SDLInteraction::~SDLInteraction() {}

/**
 * Mô tả: Khởi tạo hệ thống interaction cho SDL.
 * Đầu vào:
 *   - config: cấu hình runtime của chương trình.
 * Đầu ra: Không có.
 * Tác dụng phụ:
 *   - Thiết lập trạng thái ban đầu cho input SDL.
 */
void SDLInteraction::init(const RunConfig &config) {
  // TODO:
  // - Khởi tạo các thành phần cần thiết cho input SDL
  // - Có thể reset event queue hoặc trạng thái input
}

/**
 * Mô tả: Tạm dừng chương trình trong SDL.
 *        - Nếu có timeout > 0: delay trong khoảng thời gian tương ứng.
 *        - Nếu timeout == 0: chờ người dùng tương tác (nhấn phím hoặc click
 * chuột). Đầu vào:
 *   - timeout: thời gian chờ (milliseconds). Nếu = 0 thì chờ event từ người
 * dùng. Đầu ra: Không có. Tác dụng phụ:
 *   - Có thể block thread hiện tại.
 *   - Có thể ném QuitException nếu người dùng đóng cửa sổ.
 */
void SDLInteraction::pause(int timeout) {
  // Nếu có timeout cụ thể -> delay trực tiếp bằng SDL
  if (timeout > 0) {
    SDL_Delay(timeout);
    return;
  }

  bool waiting = true;
  SDL_Event event;

  // Vòng lặp chờ sự kiện từ người dùng
  while (waiting) {
    // SDL_WaitEvent sẽ block cho tới khi có event
    if (SDL_WaitEvent(&event)) {
      // Nếu người dùng đóng cửa sổ -> thoát game
      waitForQuit(event);

      // Nếu có tương tác (nhấn phím hoặc click chuột) -> kết thúc pause
      if (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) {
        waiting = false;
      }
    }
  }
}

/**
 * Mô tả: Lấy input kích thước bàn cờ từ người dùng qua SDL.
 * Đầu vào:
 *   - size: con trỏ lưu kết quả kích thước.
 * Đầu ra:
 *   - true nếu input hợp lệ, false nếu không hợp lệ.
 * Tác dụng phụ:
 *   - Cập nhật giá trị tại size nếu thành công.
 */
bool SDLInteraction::selectSize(int *size) {
  SDL_Event event;

  while (true) {
    if (SDL_WaitEvent(&event)) {
      waitForQuit(event);

      if (event.type == SDL_MOUSEBUTTONDOWN) {
        int w = 800, h = 600;
        SDL_Window *window = SDL_GetWindowFromID(event.button.windowID);
        if (window) {
          SDL_GetWindowSize(window, &w, &h);
        }

        int cols = 5;
        int rows = 2;
        int buttonW = 128;
        int buttonH = 80;
        int gapX = 16;
        int gapY = 24;

        int totalW = cols * buttonW + (cols - 1) * gapX;
        int totalH = rows * buttonH + (rows - 1) * gapY;
        int startX = (w - totalW) / 2;
        int startY = (h - totalH) / 2;

        int mx = event.button.x;
        int my = event.button.y;

        if (startX <= mx && mx <= startX + totalW && startY <= my &&
            my <= startY + totalH) {
          int idxx = (mx - startX) / (buttonW + gapX);
          int idxy = (my - startY) / (buttonH + gapY);

          *size = idxx + idxy * cols + BOARD_N_MIN;
          return true;
        }
      }
    }
  }
}

/**
 * Mô tả: Lấy input goal (số ô liên tiếp để thắng).
 * Đầu vào:
 *   - goal: con trỏ lưu giá trị goal.
 *   - size: kích thước bàn cờ hiện tại.
 * Đầu ra:
 *   - true nếu hợp lệ, false nếu không hợp lệ.
 * Tác dụng phụ:
 *   - Cập nhật goal nếu hợp lệ.
 */
bool SDLInteraction::selectGoal(int *goal, const int size) {
  SDL_Event event;

  while (true) {
    if (SDL_WaitEvent(&event)) {
      waitForQuit(event);

      if (event.type == SDL_MOUSEBUTTONDOWN) {
        int w = 800, h = 600;
        SDL_Window *window = SDL_GetWindowFromID(event.button.windowID);
        if (window) {
          SDL_GetWindowSize(window, &w, &h);
        }

        int numGoal = std::min(size, GOAL_MAX) - BOARD_N_MIN + 1;

        int buttonW = 128;
        int buttonH = 80;
        int gapX = 16;

        int totalW = numGoal * buttonW + (numGoal - 1) * gapX;
        int totalH = buttonH;

        int startX = (w - totalW) / 2;
        int startY = (h - totalH) / 2;

        int mx = event.button.x;
        int my = event.button.y;

        if (startX <= mx && mx <= startX + totalW && startY <= my &&
            my <= startY + totalH) {
          int idx = (mx - startX) / (buttonW + gapX);
          *goal = idx + BOARD_N_MIN;
          return true;
        }
      }
    }
  }
}

/**
 * Mô tả: Lấy input chế độ chơi (PVP, PVE, EVE).
 * Đầu vào:
 *   - mode: con trỏ lưu giá trị GameMode.
 * Đầu ra:
 *   - true nếu hợp lệ, false nếu không hợp lệ.
 * Tác dụng phụ:
 *   - Cập nhật mode nếu hợp lệ.
 */
bool SDLInteraction::selectGameMode(GameMode *mode) {
  SDL_Event event;

  while (true) {
    if (SDL_WaitEvent(&event)) {
      waitForQuit(event);

      if (event.type == SDL_MOUSEBUTTONDOWN) {
        int w = 800, h = 600;
        SDL_Window *window = SDL_GetWindowFromID(event.button.windowID);
        if (window) {
          SDL_GetWindowSize(window, &w, &h);
        }

        int nums = 3;

        int buttonW = 128;
        int buttonH = 80;
        int gapX = 16;

        int totalW = nums * buttonW + (nums - 1) * gapX;
        int totalH = buttonH;

        int startX = (w - totalW) / 2;
        int startY = (h - totalH) / 2;

        int mx = event.button.x;
        int my = event.button.y;

        if (startX <= mx && mx <= startX + totalW && startY <= my &&
            my <= startY + totalH) {
          int idx = (mx - startX) / (buttonW + gapX);
          *mode = (GameMode)idx;
          return true;
        }
      }
    }
  }
}

/**
 * Mô tả: Lấy input cấp độ bot.
 * Đầu vào:
 *   - levels: mảng lưu cấp độ bot.
 *   - index: vị trí bot cần gán.
 * Đầu ra:
 *   - true nếu hợp lệ, false nếu không hợp lệ.
 * Tác dụng phụ:
 *   - Cập nhật levels[index] nếu hợp lệ.
 */
bool SDLInteraction::selectBotLevel(BotLevel *levels, const int index) {
  SDL_Event event;

  while (true) {
    if (SDL_WaitEvent(&event)) {
      waitForQuit(event);

      if (event.type == SDL_MOUSEBUTTONDOWN) {
        int w = 800, h = 600;
        SDL_Window *window = SDL_GetWindowFromID(event.button.windowID);
        if (window) {
          SDL_GetWindowSize(window, &w, &h);
        }

        int nums = 3;

        int buttonW = 128;
        int buttonH = 80;
        int gapX = 16;

        int totalW = nums * buttonW + (nums - 1) * gapX;
        int totalH = buttonH;

        int startX = (w - totalW) / 2;
        int startY = (h - totalH) / 2;

        int mx = event.button.x;
        int my = event.button.y;

        if (startX <= mx && mx <= startX + totalW && startY <= my &&
            my <= startY + totalH) {
          int idx = (mx - startX) / (buttonW + gapX);
          levels[index] = (BotLevel)idx;
          return true;
        }
      }
    }
  }
}

/**
 * Mô tả: Lấy nước đi từ người chơi (row, col) qua SDL.
 * Đầu vào:
 *   - row: con trỏ lưu hàng.
 *   - col: con trỏ lưu cột.
 * Đầu ra:
 *   - true nếu lấy được input hợp lệ, false nếu không.
 * Tác dụng phụ:
 *   - Cập nhật row, col nếu hợp lệ.
 */
bool SDLInteraction::getPlayerMove(int *row, int *col, const int size) {
  SDL_Event event;

  while (true) {
    if (SDL_WaitEvent(&event)) {
      waitForQuit(event);

      if (event.type == SDL_MOUSEBUTTONDOWN) {
        int w = 800, h = 600;
        SDL_Window *window = SDL_GetWindowFromID(event.button.windowID);
        if (window) {
          SDL_GetWindowSize(window, &w, &h);
        }

        int padding = 60;
        int availableSize = std::min(w, h) - 2 * padding;
        int cellSize = std::min(availableSize / size, 120);

        int boardW = cellSize * size;
        int boardH = cellSize * size;

        int startX = (w - boardW) / 2;
        int startY = (h - boardH) / 2 + padding / 4;

        int mx = event.button.x;
        int my = event.button.y;

        if (startX <= mx && mx <= startX + cellSize * size &&
            startY <= my && my <= startY + cellSize * size) {
          *row = (my - startY) / cellSize;
          *col = (mx - startX) / cellSize;
          return true;
        }
      }
    }
  }
}

/**
 * Mô tả: Dọn dẹp tài nguyên liên quan đến SDL interaction.
 * Đầu vào: Không có.
 * Đầu ra: Không có.
 * Tác dụng phụ:
 *   - Giải phóng hoặc reset trạng thái input nếu cần.
 */
void SDLInteraction::close() {
  // TODO:
  // - Dọn dẹp trạng thái, nếu có resource thì giải phóng
}