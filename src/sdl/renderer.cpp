/**
 * SDL Renderer cpp implementation
 *
 */

#include "renderer.h"

/* ---------- Importing ---------- */

#include <SDL_ttf.h>
#include <format>
#include <string>

#include "../game/setup.h"
#include "../utils/config.h"

/* ---------- Definitions ---------- */
SDLRenderer::SDLRenderer() : I_Renderer() {}

/**
 * Mô tả: Destructor của SDLRenderer.
 * Đầu vào: Không.
 * Đầu ra: Không.
 * Tác dụng phụ: Không (việc giải phóng thực hiện trong close()).
 */
SDLRenderer::~SDLRenderer() {}

/**
 * Mô tả: Khởi tạo SDL, window và renderer.
 * Đầu vào: config - cấu hình màn hình và layout.
 * Đầu ra: Không.
 * Tác dụng phụ:
 *   - Khởi tạo SDL subsystem.
 *   - Tạo window và renderer.
 *   - Thiết lập chế độ blend.
 * NOTE: Phải gọi trước khi render.
 */
void SDLRenderer::init(const RunConfig &config) {
  // read config
  this->screenWidth = config.screenWidth;
  this->screenHeight = config.screenHeight;
  this->padding = config.boardPadding;

  this->buttonWidth = 128;
  this->buttonHeight = 80;
  this->gapX = 16;
  this->gapY = 16;

  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();

  window =
      SDL_CreateWindow("TicTacToe SDL", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, 0);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  titleFont = TTF_OpenFont("assets/NovaRound-Regular.ttf", 64);
  largeFont = TTF_OpenFont("assets/NovaRound-Regular.ttf", 32);
  regularFont = TTF_OpenFont("assets/NovaRound-Regular.ttf", 20);
  smallFont = TTF_OpenFont("assets/NovaRound-Regular.ttf", 16);
}

/**
 * Mô tả: Xóa toàn bộ nội dung màn hình (background tối).
 * Đầu vào: Không.
 * Đầu ra: Không.
 * Tác dụng phụ: Reset frame hiện tại.
 */
void SDLRenderer::clearScreen() {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white background
  SDL_RenderClear(renderer);
}

/**
 * Mô tả: Hiển thị frame đã render lên màn hình.
 * Đầu vào: Không.
 * Đầu ra: Không.
 * Tác dụng phụ: Swap buffer để hiển thị nội dung.
 */
void SDLRenderer::renderPresent() { SDL_RenderPresent(renderer); }

/**
 * Mô tả: Vẽ hình chữ nhật lên renderer.
 * Đầu vào:
 *   - x, y: tọa độ.
 *   - w, h: kích thước.
 *   - color: màu sắc.
 *   - filled: true nếu fill, false nếu vẽ viền.
 * Đầu ra: Không.
 * Tác dụng phụ: Vẽ trực tiếp lên renderer.
 */
void SDLRenderer::drawRect(int x, int y, int w, int h, SDL_Color color,
                           bool filled) {
  SDL_Rect rect = {x, y, w, h};
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  if (filled)
    SDL_RenderFillRect(renderer, &rect);
  else
    SDL_RenderDrawRect(renderer, &rect);
}

/**
 * Mô tả: Hiển thị menu lựa chọn.
 * Đầu vào: selectType, context.
 * Đầu ra: Không.
 * Tác dụng phụ: Vẽ UI lên màn hình.
 * TODO:
 *   - Bước 1: Xác định loại menu.
 *   - Bước 2: Render text tương ứng.
 *   - Trường hợp biên: selectType không hợp lệ.
 */
void SDLRenderer::showSelectMenu(SelectType selectType, int context) {
  clearScreen();

  int w = this->screenWidth, h = this->screenHeight;

  switch (selectType) {

  case SelectType::TITLE_UI: {
    SDL_Surface *titleSurface =
        TTF_RenderUTF8_Blended(titleFont, "Tic-Tac-Toe", textColor);
    SDL_Texture *titleTexture =
        SDL_CreateTextureFromSurface(renderer, titleSurface);
    int titleW = titleSurface->w;
    int titleH = titleSurface->h;

    SDL_Surface *versionSurface = TTF_RenderUTF8_Blended(
        regularFont, std::format("v{}", VERSION).c_str(), textColor);
    SDL_Texture *versionTexture =
        SDL_CreateTextureFromSurface(renderer, versionSurface);
    int versionW = versionSurface->w;
    int versionH = versionSurface->h;

    SDL_Surface *promptSurface = TTF_RenderUTF8_Blended(
        largeFont, "Press any key to continue...", textColor);
    SDL_Texture *promptTexture =
        SDL_CreateTextureFromSurface(renderer, promptSurface);
    int promptW = promptSurface->w;
    int promptH = promptSurface->h;

    SDL_Rect titleRect = {(w - titleW) / 2, (h - titleH) / 2 - 40, titleW,
                          titleH};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    SDL_Rect versionRect = {(w - versionW) / 2, titleRect.y + titleRect.h + 6,
                            versionW, versionH};
    SDL_RenderCopy(renderer, versionTexture, NULL, &versionRect);

    SDL_Rect promptRect = {(w - promptW) / 2,
                           versionRect.y + versionRect.h + 32, promptW,
                           promptH};
    SDL_RenderCopy(renderer, promptTexture, NULL, &promptRect);

    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);
    SDL_FreeSurface(versionSurface);
    SDL_DestroyTexture(versionTexture);
    SDL_FreeSurface(promptSurface);
    SDL_DestroyTexture(promptTexture);

    break;
  }

  case SelectType::SIZE_UI: {
    SDL_Surface *titleSurface =
        TTF_RenderUTF8_Blended(largeFont, "Select board size", textColor);
    SDL_Texture *titleTexture =
        SDL_CreateTextureFromSurface(renderer, titleSurface);
    int titleW = titleSurface->w;
    int titleH = titleSurface->h;

    SDL_Rect titleRect = {(w - titleW) / 2, 12, titleW, titleH};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

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

    for (int size = BOARD_N_MIN; size <= BOARD_N_MAX; size++) {
      int index = size - BOARD_N_MIN;
      int col = index % cols;
      int row = index / cols;

      int bx = startX + col * (buttonW + gapX);
      int by = startY + row * (buttonH + gapY);

      std::string text = std::format("{}x{}", size, size);

      renderSelectButton(bx, by, buttonW, buttonH, text);
    }

    break;
  }

  case SelectType::GOAL_UI: {
    SDL_Surface *titleSurface =
        TTF_RenderUTF8_Blended(largeFont, "Select goal", textColor);
    SDL_Texture *titleTexture =
        SDL_CreateTextureFromSurface(renderer, titleSurface);
    int titleW = titleSurface->w;
    int titleH = titleSurface->h;

    SDL_Rect titleRect = {(w - titleW) / 2, 12, titleW, titleH};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    int maxGoal = std::min(context, GOAL_MAX);
    int numGoal = maxGoal - BOARD_N_MIN + 1;

    int buttonW = 128;
    int buttonH = 80;
    int gapX = 16;

    int totalW = numGoal * buttonW + (numGoal - 1) * gapX;
    int totalH = buttonH;

    int startX = (w - totalW) / 2;
    int startY = (h - totalH) / 2;

    for (int goal = BOARD_N_MIN; goal <= maxGoal; goal++) {
      int index = goal - BOARD_N_MIN;

      int bx = startX + index * (buttonW + gapX);
      int by = startY;

      std::string text = std::to_string(goal);

      renderSelectButton(bx, by, buttonW, buttonH, text);
    }

    break;
  }

  case SelectType::GAME_MODE_UI: {
    SDL_Surface *titleSurface =
        TTF_RenderUTF8_Blended(largeFont, "Select game mode", textColor);
    SDL_Texture *titleTexture =
        SDL_CreateTextureFromSurface(renderer, titleSurface);
    int titleW = titleSurface->w;
    int titleH = titleSurface->h;

    SDL_Rect titleRect = {(w - titleW) / 2, 12, titleW, titleH};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    int nums = 3;

    int buttonW = 128;
    int buttonH = 80;
    int gapX = 16;

    int totalW = nums * buttonW + (nums - 1) * gapX;
    int totalH = buttonH;

    int startX = (w - totalW) / 2;
    int startY = (h - totalH) / 2;

    for (int mode = 0; mode < nums; mode++) {
      int bx = startX + mode * (buttonW + gapX);
      int by = startY;

      std::string text = modeToString(mode);

      renderSelectButton(bx, by, buttonW, buttonH, text);
    }

    break;
  }

  case SelectType::BOT_LEVEL_UI: {
    SDL_Surface *titleSurface =
        TTF_RenderUTF8_Blended(largeFont, "Select bot level", textColor);
    SDL_Texture *titleTexture =
        SDL_CreateTextureFromSurface(renderer, titleSurface);
    int titleW = titleSurface->w;
    int titleH = titleSurface->h;

    SDL_Rect titleRect = {(w - titleW) / 2, 12, titleW, titleH};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    int nums = 3;

    int buttonW = 128;
    int buttonH = 80;
    int gapX = 16;

    int totalW = nums * buttonW + (nums - 1) * gapX;
    int totalH = buttonH;

    int startX = (w - totalW) / 2;
    int startY = (h - totalH) / 2;

    for (int mode = 0; mode < nums; mode++) {
      int bx = startX + mode * (buttonW + gapX);
      int by = startY;

      std::string text = botToString(mode);

      renderSelectButton(bx, by, buttonW, buttonH, text);
    }

    break;
  }

  case SelectType::MUL_BOT_LEVEL_UI: {
    SDL_Surface *titleSurface = TTF_RenderUTF8_Blended(
        largeFont, std::format("Select level for bot {}", context + 1).c_str(),
        textColor);
    SDL_Texture *titleTexture =
        SDL_CreateTextureFromSurface(renderer, titleSurface);
    int titleW = titleSurface->w;
    int titleH = titleSurface->h;

    SDL_Rect titleRect = {(w - titleW) / 2, 12, titleW, titleH};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    int nums = 3;

    int buttonW = 128;
    int buttonH = 80;
    int gapX = 16;

    int totalW = nums * buttonW + (nums - 1) * gapX;
    int totalH = buttonH;

    int startX = (w - totalW) / 2;
    int startY = (h - totalH) / 2;

    for (int mode = 0; mode < nums; mode++) {
      int bx = startX + mode * (buttonW + gapX);
      int by = startY;

      std::string text = botToString(mode);

      renderSelectButton(bx, by, buttonW, buttonH, text);
    }

    break;
  }

  case SelectType::PLAYER_UI: {
    return;
  }
  }

  renderPresent();
}

void SDLRenderer::renderSelectButton(int x, int y, int w, int h,
                                     std::string text) {
  SDL_Color bgColor = {50, 98, 219, 255};
  drawRect(x, y, w, h, bgColor, true);

  SDL_Surface *textSurface =
      TTF_RenderUTF8_Blended(regularFont, text.c_str(), {255, 255, 255, 255});
  SDL_Texture *textTexture =
      SDL_CreateTextureFromSurface(renderer, textSurface);

  SDL_Rect textRect = {x + (w - textSurface->w) / 2,
                       y + (h - textSurface->h) / 2, textSurface->w,
                       textSurface->h};
  SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

  SDL_FreeSurface(textSurface);
  SDL_DestroyTexture(textTexture);
}

/**
 * Mô tả: Hiển thị thông báo lựa chọn không hợp lệ.
 * Đầu vào: selectType, context.
 * Đầu ra: Không.
 * Tác dụng phụ: Hiển thị thông báo lỗi.
 * TODO:
 *   - Bước 1: Xác định loại lỗi.
 *   - Bước 2: Render text cảnh báo.
 */
void SDLRenderer::showInvalidSelect(SelectType selectType, int context) {}

/**
 * Mô tả: Hiển thị thông báo lựa chọn hợp lệ.
 * Đầu vào: selectType, context.
 * Đầu ra: Không.
 * Tác dụng phụ: Hiển thị xác nhận.
 * TODO:
 *   - Bước 1: Xác định loại selection.
 *   - Bước 2: Render thông báo thành công.
 */
void SDLRenderer::showValidSelect(SelectType selectType, int context) {}

/**
 * Mô tả: Vẽ bàn cờ lên màn hình.
 * Đầu vào: board, size.
 * Đầu ra: Không.
 * Tác dụng phụ: Render grid và ký hiệu.
 * TODO:
 *   - Bước 1: Tính toán layout ô.
 *   - Bước 2: Vẽ grid.
 *   - Bước 3: Vẽ X/O.
 */
void SDLRenderer::displayBoard(const char board[][BOARD_N_MAX],
                               const int size) {
  int availableSize =
      std::min(this->screenWidth, this->screenHeight) - 2 * this->padding;
  int cellSize = std::min(availableSize / size, 120);

  int boardW = cellSize * size;
  int boardH = cellSize * size;

  int startX = (this->screenWidth - boardW) / 2;
  int startY = (this->screenHeight - boardH) / 2 + this->padding / 4;

  this->cellSize = cellSize;
  this->boardStartX = startX;
  this->boardStartY = startY;

  // Draw grid lines
  SDL_SetRenderDrawColor(renderer, 31, 31, 31, 255);
  for (int i = 1; i < size; ++i) {
    for (int offset = -1; offset <= 1; ++offset) {
      // Horizontal lines
      SDL_RenderDrawLine(renderer, startX, startY + i * cellSize + offset,
                         startX + boardW, startY + i * cellSize + offset);
      // Vertical lines
      SDL_RenderDrawLine(renderer, startX + i * cellSize + offset, startY,
                         startX + i * cellSize + offset, startY + boardH);
    }
  }

  // Draw X and O
  for (int r = 0; r < size; ++r) {
    for (int c = 0; c < size; ++c) {
      if (board[r][c] != '-') {
        std::string mark = std::string(1, board[r][c]);
        SDL_Color color = (board[r][c] == 'X') ? SDL_Color{235, 64, 52, 255}
                                               : SDL_Color{52, 110, 235, 255};

        SDL_Surface *markSurface =
            TTF_RenderUTF8_Blended(titleFont, mark.c_str(), color);
        SDL_Texture *markTexture =
            SDL_CreateTextureFromSurface(renderer, markSurface);

        int tw = markSurface->w;
        int th = markSurface->h;

        // Scale to 60% of cell size
        float scale = std::min((cellSize * 0.6f) / tw, (cellSize * 0.6f) / th);
        int dw = tw * scale;
        int dh = th * scale;

        int dx = startX + c * cellSize + (cellSize - dw) / 2;
        int dy = startY + r * cellSize + (cellSize - dh) / 2;

        SDL_Rect dest = {dx, dy, dw, dh};
        SDL_RenderCopy(renderer, markTexture, NULL, &dest);

        SDL_FreeSurface(markSurface);
        SDL_DestroyTexture(markTexture);
      }
    }
  }

  renderPresent();
}

/**
 * Mô tả: Hiển thị nước đi vừa thực hiện.
 * Đầu vào: row, col.
 * Đầu ra: Không.
 * Tác dụng phụ: Highlight ô.
 * TODO:
 *   - Bước 1: Xác định vị trí ô.
 *   - Bước 2: Vẽ highlight.
 */
void SDLRenderer::showMove(const int row, const int col) {}

/**
 * Mô tả: Hiển thị thông báo nước đi không hợp lệ.
 * Đầu vào: Không.
 * Đầu ra: Không.
 * Tác dụng phụ: Hiển thị lỗi.
 * TODO:
 *   - Bước 1: Render thông báo lỗi.
 */
void SDLRenderer::showInvalidMove() {}

/**
 * Mô tả: Hiển thị người chơi hiện tại.
 * Đầu vào: player, is_bot.
 * Đầu ra: Không.
 * Tác dụng phụ: Hiển thị thông tin turn.
 * TODO:
 *   - Bước 1: Xác định text.
 *   - Bước 2: Render lên màn hình.
 */
void SDLRenderer::showPlayer(const int player, const bool is_bot) {
  SDL_Surface *promptSurface =
      TTF_RenderUTF8_Blended(largeFont,
                             std::format("It's turn for player {}{}.",
                                         player + 1, is_bot ? " (bot)" : "")
                                 .c_str(),
                             secondaryColor);
  SDL_Texture *promptTexture =
      SDL_CreateTextureFromSurface(renderer, promptSurface);
  int promptW = promptSurface->w;
  int promptH = promptSurface->h;

  SDL_Rect promptRect = {(this->screenWidth - promptW) / 2,
                         ((this->padding * 5 / 4) - promptH) / 2, promptW,
                         promptH};
  SDL_RenderCopy(renderer, promptTexture, NULL, &promptRect);

  SDL_FreeSurface(promptSurface);
  SDL_DestroyTexture(promptTexture);

  renderPresent();
}

/**
 * Mô tả: Hiển thị kết quả game.
 * Đầu vào: winner, is_bot, winLine.
 * Đầu ra: Không.
 * Tác dụng phụ: Hiển thị kết quả và highlight đường thắng.
 * TODO:
 *   - Bước 1: Kiểm tra draw hoặc win.
 *   - Bước 2: Render text.
 *   - Bước 3: Highlight winLine nếu có.
 */
void SDLRenderer::showResult(const int winner, const bool is_bot,
                             const WinLine *winLine) {
  std::string winStr;
  if (winner == -1)
    winStr = "Draw";
  else
    winStr = std::format("The winner is player {}{}.", winner + 1,
                         is_bot ? " (bot)" : "");

  SDL_Surface *promptSurface =
      TTF_RenderUTF8_Blended(largeFont, winStr.c_str(), secondaryColor);
  SDL_Texture *promptTexture =
      SDL_CreateTextureFromSurface(renderer, promptSurface);
  int promptW = promptSurface->w;
  int promptH = promptSurface->h;

  SDL_Rect promptRect = {(this->screenWidth - promptW) / 2,
                         ((this->padding * 5 / 4) - promptH) / 2, promptW,
                         promptH};
  SDL_RenderCopy(renderer, promptTexture, NULL, &promptRect);

  SDL_FreeSurface(promptSurface);
  SDL_DestroyTexture(promptTexture);

  // Render win line if exist
  if (winLine != nullptr) {
    SDL_SetRenderDrawColor(renderer, 255, 235, 59, 128);
    for (const auto &cell : winLine->cells) {
      int r = cell.first;
      int c = cell.second;
      SDL_Rect highlightRect = {boardStartX + c * cellSize,
                                boardStartY + r * cellSize, cellSize, cellSize};
      SDL_RenderFillRect(renderer, &highlightRect);
    }
  }

  renderPresent();
}

/**
 * Mô tả: In kết quả ra stdout (judge mode).
 * Đầu vào: gameResult.
 * Đầu ra: Không.
 * Tác dụng phụ: In console.
 * TODO:
 *   - Bước 1: Format output.
 *   - Bước 2: In ra std::cout.
 */
void SDLRenderer::printResult(const GameResult &gameResult) {}

/**
 * Mô tả: Giải phóng tài nguyên SDL.
 * Đầu vào: Không.
 * Đầu ra: Không.
 * Tác dụng phụ:
 *   - Destroy renderer và window.
 *   - Shutdown SDL subsystem.
 */
void SDLRenderer::close() {
  TTF_Quit();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}