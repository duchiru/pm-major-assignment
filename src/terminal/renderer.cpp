/**
 * Terminal Renderer cpp implementation
 *
 * Mô tả:
 *   Port từ level 2, đổi chữ ký displayBoard để nhận const Board& thay vì char[][].
 *   Đây là tầng "imperative shell" -- được phép side-effect (cout, clear screen).
 */

#include "renderer.h"

/* ---------- Importing ---------- */

#include <format>
#include <iostream>

/* ---------- Definitions ---------- */

TerminalRenderer::TerminalRenderer() : I_Renderer() {}
TerminalRenderer::~TerminalRenderer() = default;

void TerminalRenderer::init(const RunConfig& config) {
    if (config.interactive)
        clearScreen();
}

void TerminalRenderer::clearScreen() {
    std::cout << "\n"
              << "\x1B[2J\x1B[H" << "\n";
}

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

void TerminalRenderer::displayBoard(const Board& board) {
    int size = board.size;

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
            std::cout << board.at(i, j);
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

void TerminalRenderer::showPlayer(int player, bool is_bot) {
    std::cout << std::format("It's turn for player {}{}.", player + 1,
                             is_bot ? " (bot)" : "")
              << '\n';
}

void TerminalRenderer::showMove(int row, int col) {
    std::cout << std::format("Move made at ({}, {})\n", row, col);
}

void TerminalRenderer::showInvalidMove() {
    std::cout << "[!] Invalid move. Please try again.\n";
}

void TerminalRenderer::showResult(int winner, bool is_bot,
                                   const WinLine* winLine) {
    if (winner == -1) {
        std::cout << "It's a draw!\n";
    } else {
        std::cout << std::format("Player {} {} wins!\n", winner + 1,
                                 is_bot ? "(Bot)" : "");
        if (winLine != nullptr) {
            std::cout << "Winning line: ";
            for (const pII& cell : winLine->cells)
                std::cout << std::format("({}, {}) ", cell.first, cell.second);
            std::cout << '\n';
        }
    }
}

void TerminalRenderer::printResult(const GameResult& gameResult) {
    std::cout << gameResult.winner << ' ' << gameResult.turns << '\n';
}

void TerminalRenderer::close() {
    // nothing is required here
}
