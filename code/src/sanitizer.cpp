 /*
  * Copyright 2026 Alan Crispin <crispinalan@gmail.com>
  * SPDX-License-Identifier: GPL-2.0-or-later
  */
#include "sanitizer.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>

// Helper function to check if a character is a digit
bool is_digit(char c) {
    return std::isdigit(static_cast<unsigned char>(c));
}

std::string get_ordinal_string(int day) {
    switch (day) {
        case 1: return "first";
        case 2: return "second";
        case 3: return "third";
        case 4: return "fourth";
        case 5: return "fifth";
        case 6: return "sixth";
        case 7: return "seventh";
        case 8: return "eighth";
        case 9: return "ninth";
        case 10: return "tenth";
        case 11: return "eleventh";
        case 12: return "twelfth";
        case 13: return "thirteenth";
        case 14: return "fourteenth";
        case 15: return "fifteenth";
        case 16: return "sixteenth";
        case 17: return "seventeenth";
        case 18: return "eighteenth";
        case 19: return "nineteenth";
        case 20: return "twentieth";
        case 21: return "twenty first";
        case 22: return "twenty second";
        case 23: return "twenty third";
        case 24: return "twenty fourth";
        case 25: return "twenty fifth";
        case 26: return "twenty sixth";
        case 27: return "twenty seventh";
        case 28: return "twenty eighth";
        case 29: return "twenty ninth";
        case 30: return "thirtieth";
        case 31: return "thirty first";
        default: return "unknown";
    }
}

std::string get_cardinal_string(int number) {
    switch (number) {
        case 0: return "zero";
        case 1: return "one";
        case 2: return "two";
        case 3: return "three";
        case 4: return "four";
        case 5: return "five";
        case 6: return "six";
        case 7: return "seven";
        case 8: return "eight";
        case 9: return "nine";
        case 10: return "ten";
        case 11: return "eleven";
        case 12: return "twelve";
        case 13: return "thirteen";
        case 14: return "fourteen";
        case 15: return "fifteen";
        case 16: return "sixteen";
        case 17: return "seventeen";
        case 18: return "eighteen";
        case 19: return "nineteen";
        case 20: return "twenty";
        case 21: return "twenty one";
        case 22: return "twenty two";
        case 23: return "twenty three";
        case 24: return "twenty four";
        case 25: return "twenty five";
        case 26: return "twenty six";
        case 27: return "twenty seven";
        case 28: return "twenty eight";
        case 29: return "twenty nine";
        case 30: return "thirty";
        case 31: return "thirty one";
        case 32: return "thirty two";
        case 33: return "thirty three";
        case 34: return "thirty four";
        case 35: return "thirty five";
        case 36: return "thirty six";
        case 37: return "thirty seven";
        case 38: return "thirty eight";
        case 39: return "thirty nine";
        case 40: return "forty";
        case 41: return "forty one";
        case 42: return "forty two";
        case 43: return "forty three";
        case 44: return "forty four";
        case 45: return "forty five";
        case 46: return "forty six";
        case 47: return "forty seven";
        case 48: return "forty eight";
        case 49: return "forty nine";
        case 50: return "fifty";
        case 51: return "fifty one";
        case 52: return "fifty two";
        case 53: return "fifty three";
        case 54: return "fifty four";
        case 55: return "fifty five";
        case 56: return "fifty six";
        case 57: return "fifty seven";
        case 58: return "fifty eight";
        case 59: return "fifty nine";
        default: return std::to_string(number); // Fallback for numbers beyond 59
    }
}

void process_ordinals(std::string& text) {
    static const std::vector<std::pair<std::string, size_t>> ordinal_suffixes = {
        {"st", 0}, {"nd", 0}, {"rd", 0}, {"th", 0}
    };

    for (auto& suffix_pair : ordinal_suffixes) {
        const std::string& suffix = suffix_pair.first;
        size_t pos = 0;
        bool is_ordinal = false;
        while ((pos = text.find(suffix, pos)) != std::string::npos) {
            if (pos > 0 && is_digit(text[pos - 1])) {


                size_t num_start = pos - 1;
                while (num_start > 0 && is_digit(text[num_start - 1])) {
                    num_start--;
                }

                std::string num_str = text.substr(num_start, pos - num_start);
                try {
                    int num = std::stoi(num_str);
                    if (num >= 1 && num <= 31) {
                        is_ordinal = true;
                        std::string replacement = " " + get_ordinal_string(num);
                        text.replace(num_start, pos - num_start + suffix.size(), replacement);
                        pos += replacement.length();
                    }
                } catch (...) {
                    std::cout << "Error: Invalid number at position: " << num_start
                    << ", num_str: '" << num_str << "'" << std::endl;
                }
            }

            if (!is_ordinal) pos += suffix.size();
        }
    }
}

void process_cardinals(std::string& text) {
    size_t pos = 0;
    while ((pos = std::distance(text.begin(), std::find_if(text.begin() + pos, text.end(), is_digit))) < text.size()) {
        size_t num_end = pos;
        while (num_end < text.size() && is_digit(text[num_end])) {
            num_end++;
        }

        std::string num_str = text.substr(pos, num_end - pos);
        std::string replacement;

        if (num_str.length() == 1) {
            int num = std::stoi(num_str);
            replacement = " " + get_cardinal_string(num);
        } else if (num_str.length() <= 2) {
            int num = std::stoi(num_str);
            replacement = " " + get_cardinal_string(num);
        } else {
            replacement = " " + num_str;
        }

        text.replace(pos, num_end - pos, replacement);
        pos += replacement.length();
    }
}

void remove_punctuation(std::string& text) {
    for (size_t i = 0; i < text.size(); ++i) {
        if (!std::isalnum(text[i]) && !std::isspace(text[i])) {
            text[i] = ' ';
        }
    }
}

std::string preprocess_text(const std::string& input) {
    std::string result = input;

    remove_punctuation(result);
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    process_cardinals(result);
    process_ordinals(result);

    return result;
}
