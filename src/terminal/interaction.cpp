/**
 * Terminal Interaction cpp implementation
 *
 * Mô tả:
 *   Port từ level 2. Đây là tầng shell -- được phép cin/cout.
 *   Khác biệt: getPlayerMove signature không có size param.
 */

#include "interaction.h"

/* ---------- Importing ---------- */

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

/* ---------- Internal State ---------- */

static std::ifstream g_file_in;
static std::streambuf* g_cin_backup = nullptr;

/* ---------- Helper Functions ---------- */

static bool validateInput(const std::string& input) {
    if (input.empty()) return false;
    for (char c : input) {
        if (!std::isdigit(c)) return false;
    }
    return true;
}

static bool getInput(int* val) {
    std::string s;
    std::cin >> s;

    if (!validateInput(s)) return false;

    *val = std::stoi(s);
    return true;
}

/* ---------- Definitions ---------- */

TerminalInteraction::TerminalInteraction() : I_Interaction() {}
TerminalInteraction::~TerminalInteraction() = default;

void TerminalInteraction::init(const RunConfig& config) {
    g_cin_backup = nullptr;

    if (!config.interactive && !config.input_file.empty()) {
        g_file_in.open(config.input_file);
        if (g_file_in.is_open()) {
            g_cin_backup = std::cin.rdbuf();
            std::cin.rdbuf(g_file_in.rdbuf());
        }
    }
}

void TerminalInteraction::pause(int timeout) {
    if (timeout <= 0)
        std::cin.get();
    else
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
}

bool TerminalInteraction::selectSize(int* size) {
    if (!getInput(size)) return false;
    if (*size < BOARD_N_MIN || *size > BOARD_N_MAX) return false;
    return true;
}

bool TerminalInteraction::selectGoal(int* goal, int size) {
    int max_goal = std::min(size, GOAL_MAX);
    if (!getInput(goal)) return false;
    if (*goal < BOARD_N_MIN || *goal > max_goal) return false;
    return true;
}

bool TerminalInteraction::selectGameMode(GameMode* mode) {
    int x;
    if (!getInput(&x)) return false;
    if (x < 1 || x > 3) {
        *mode = GameMode::INVALID_MODE;
        return false;
    }
    *mode = static_cast<GameMode>(x - 1);
    return true;
}

bool TerminalInteraction::selectBotLevel(BotLevel* levels, int index) {
    int x;
    if (!getInput(&x)) return false;
    if (x < 1 || x > 3) {
        levels[index] = BotLevel::INVALID_LV;
        return false;
    }
    levels[index] = static_cast<BotLevel>(x - 1);
    return true;
}

bool TerminalInteraction::getPlayerMove(int* row, int* col) {
    if (!getInput(row)) return false;
    if (!getInput(col)) return false;
    return true;
}

void TerminalInteraction::close() {
    if (g_cin_backup) {
        std::cin.rdbuf(g_cin_backup);
        g_cin_backup = nullptr;
    }
    if (g_file_in.is_open()) {
        g_file_in.close();
    }
}
