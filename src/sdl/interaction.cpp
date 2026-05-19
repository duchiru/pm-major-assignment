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
  // Load config
  this->screenWidth = config.screenWidth;
  this->screenHeight = config.screenHeight;
  this->padding = config.boardPadding;

  // Init button size
  this->buttonWidth = 128;
  this->buttonHeight = 80;
  this->gapX = 16;
  this->gapY = 24;
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
        int cols = 5;
        int rows = 2;

        int totalW = cols * this->buttonWidth + (cols - 1) * this->gapX;
        int totalH = rows * this->buttonHeight + (rows - 1) * this->gapY;
        int startX = (this->screenWidth - totalW) / 2;
        int startY = (this->screenHeight - totalH) / 2;

        int mx = event.button.x;
        int my = event.button.y;

        if (startX <= mx && mx <= startX + totalW && startY <= my &&
            my <= startY + totalH) {
          int idxx = (mx - startX) / (this->buttonWidth + this->gapX);
          int idxy = (my - startY) / (this->buttonHeight + this->gapY);

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
        int numGoal = std::min(size, GOAL_MAX) - BOARD_N_MIN + 1;

        int totalW = numGoal * this->buttonWidth + (numGoal - 1) * this->gapX;
        int totalH = this->buttonHeight;

        int startX = (this->screenWidth - totalW) / 2;
        int startY = (this->screenHeight - totalH) / 2;

        int mx = event.button.x;
        int my = event.button.y;

        if (startX <= mx && mx <= startX + totalW && startY <= my &&
            my <= startY + totalH) {
          int idx = (mx - startX) / (this->buttonWidth + this->gapX);
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
        int nums = 3;

        int totalW = nums * this->buttonWidth + (nums - 1) * this->gapX;
        int totalH = this->buttonHeight;

        int startX = (this->screenWidth - totalW) / 2;
        int startY = (this->screenHeight - totalH) / 2;

        int mx = event.button.x;
        int my = event.button.y;

        if (startX <= mx && mx <= startX + totalW && startY <= my &&
            my <= startY + totalH) {
          int idx = (mx - startX) / (this->buttonWidth + this->gapX);
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
        int nums = 3;

        int totalW = nums * this->buttonWidth + (nums - 1) * this->gapX;
        int totalH = this->buttonHeight;

        int startX = (this->screenWidth - totalW) / 2;
        int startY = (this->screenHeight - totalH) / 2;

        int mx = event.button.x;
        int my = event.button.y;

        if (startX <= mx && mx <= startX + totalW && startY <= my &&
            my <= startY + totalH) {
          int idx = (mx - startX) / (this->buttonWidth + this->gapX);
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

  if (SDL_PollEvent(&event)) {
    waitForQuit(event);

    if (event.type == SDL_MOUSEBUTTONDOWN) {
      int availableSize =
          std::min(this->screenWidth, this->screenHeight) - 2 * this->padding;
      int cellSize = std::min(availableSize / size, 120);

      int boardW = cellSize * size;
      int boardH = cellSize * size;

      int startX = (this->screenWidth - boardW) / 2;
      int startY = (this->screenHeight - boardH) / 2 + this->padding / 4;

      int mx = event.button.x;
      int my = event.button.y;

      if (startX <= mx && mx <= startX + cellSize * size && startY <= my &&
          my <= startY + cellSize * size) {
        *row = (my - startY) / cellSize;
        *col = (mx - startX) / cellSize;
        return true;
      }
    }

    if (event.type == SDL_KEYDOWN) {
      if (event.key.keysym.sym == SDLK_UP) {
        *row = (*row - 1 + size) % size;
      } else if (event.key.keysym.sym == SDLK_DOWN) {
        *row = (*row + 1) % size;
      } else if (event.key.keysym.sym == SDLK_LEFT) {
        *col = (*col - 1 + size) % size;
      } else if (event.key.keysym.sym == SDLK_RIGHT) {
        *col = (*col + 1) % size;
      } else if (event.key.keysym.sym == SDLK_RETURN ||
                 event.key.keysym.sym == SDLK_SPACE) {
        return true;
      }
    }
  }

  return false;
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