#include <chrono>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

const char *help_message = "You need first_generation.txt file with configured first generation.\n"
                           "Example:\n"
                           "*******************************\n"
                           "*   #                         *\n"
                           "* # #                         *\n"
                           "*  ##                         *\n"
                           "*                             *\n"
                           "*                             *\n"
                           "*                             *\n"
                           "*                             *\n"
                           "*                             *\n"
                           "*                             *\n"
                           "*                             *\n"
                           "*                             *\n"
                           "*                             *\n"
                           "*                             *\n"
                           "*                             *\n"
                           "*******************************\n";

void print_the_field(std::vector<unsigned char> &field, uint8_t rows, uint8_t cols);
int field_from_file(std::vector<unsigned char> &field, uint8_t (&rows_cols)[2]);
int validate_field(const std::vector<std::string> &field);

void handle_exit(int) {
    std::cout << "\033[?1049l" << std::flush;
    std::exit(0);
}

int main(int argc, char *argv[]) {
    std::signal(SIGINT, handle_exit);
    std::signal(SIGTERM, handle_exit);

    std::vector<unsigned char> field;
    std::vector<unsigned char> next_generation;
    uint8_t row_cols[2] = {0, 0};

    if (0 != field_from_file(field, row_cols)) {
        std::cerr << "Failed to load field from file." << std::endl;
        std::cout << help_message << std::endl;
        return -1;
    }
    next_generation.resize(field.size());

    const auto &[ROWS, COLS] = row_cols;

    std::cout << "\033[?1049h" << "\033[H" << std::flush;
    while (true) {
        std::cout << "\033[H";

        print_the_field(field, ROWS, COLS);

        std::cout << std::flush;

        for (uint8_t r = 0; r < ROWS; r++) {
            for (uint8_t c = 0; c < COLS; c++) {
                if (field[r * COLS + c] == '*') {
                    next_generation[r * COLS + c] = '*';
                } else {
                    unsigned char siblings[8] = {0};
                    siblings[0] = field[(r - 1) * COLS + c];
                    siblings[1] = field[(r - 1) * COLS + (c + 1)];
                    siblings[2] = field[r * COLS + (c + 1)];
                    siblings[3] = field[(r + 1) * COLS + (c + 1)];
                    siblings[4] = field[(r + 1) * COLS + c];
                    siblings[5] = field[(r + 1) * COLS + (c - 1)];
                    siblings[6] = field[r * COLS + (c - 1)];
                    siblings[7] = field[(r - 1) * COLS + (c - 1)];

                    uint8_t counter = 0;
                    for (auto s : siblings) {
                        if (s == '#') {
                            counter++;
                        }
                    }

                    if (field[r * COLS + c] == ' ' && counter == 3) {
                        next_generation[r * COLS + c] = '#';
                    } else if (field[r * COLS + c] == '#' && (counter > 3 || counter < 2)) {
                        next_generation[r * COLS + c] = ' ';
                    } else {
                        next_generation[r * COLS + c] = field[r * COLS + c];
                    }
                }
            }
        }

        for (uint8_t r = 0; r < ROWS; r++) {
            for (uint8_t c = 0; c < COLS; c++) {
                field[r * COLS + c] = next_generation[r * COLS + c];
                next_generation[r * COLS + c] = ' ';
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    std::cout << "\033[?1049l" << std::flush;
    return 0;
}

void print_the_field(std::vector<unsigned char> &field, uint8_t rows, uint8_t cols) {
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            std::cout << field[row * cols + col];
        }
        std::cout << '\n';
    }
}

int field_from_file(std::vector<unsigned char> &field, uint8_t (&rows_cols)[2]) {
    std::ifstream in("first_generation.txt");
    if (in.bad()) {
        std::cout << "File first_generation.txt does not exists." << std::endl;
        return -1;
    }

    std::vector<std::string> str_vec;
    std::string line;

    if (in.is_open()) {
        while (std::getline(in, line)) {
            str_vec.push_back(line);
        }
    } else {
        std::cerr << "Faild open file." << std::endl;
        return -1;
    }
    in.close();

    if (0 != validate_field(str_vec)) {
        std::cerr << "Failed file validation." << std::endl;
        return -1;
    }

    rows_cols[0] = str_vec.size();
    rows_cols[1] = str_vec[0].size();

    for (auto i : str_vec) {
        for (auto c : i) {
            field.push_back(c);
        }
    }

    return 0;
}

int validate_field(const std::vector<std::string> &field) {
    if (field.empty()) {
        std::cerr << "Field file is empty." << std::endl;
        return -1;
    }

    std::string first_line = field.front();
    std::string last_line = field.back();

    size_t row_len = first_line.length();
    if (row_len > 255) {
        std::cerr << "Line length must be <= 255";
        return -1;
    }

    if (last_line.length() != row_len) {
        std::cerr << "All lines must be the same length." << std::endl;
        return -1;
    }

    auto f_line_it = first_line.cbegin();
    auto l_line_it = last_line.cbegin();

    for (size_t i = 0; i < row_len; i++) {
        if (*f_line_it != '*' || *l_line_it != '*') {
            std::cerr << "First and last lines must be filled with '*'." << std::endl;
            return -1;
        }
        f_line_it++;
        l_line_it++;
    }

    for (size_t i = 1; i < field.size() - 1; i++) {
        if (row_len != field[i].length()) {
            std::cerr << "All lines must be the same length." << std::endl;
            return -1;
        }

        if (field[i].front() != '*' || field[i].back() != '*') {
            std::cerr << "First and last columns must be filled with '*'." << std::endl;
            return -1;
        }

        for (size_t c = 1; c + 1 < field[i].length(); c++) {
            if (field[i][c] != '#' && field[i][c] != ' ') {
                std::cerr << "Field must be filled only with '#' or ' '." << std::endl;
                return -1;
            }
        }
    }

    return 0;
}
