 /*
  * Copyright 2026 Alan Crispin <crispinalan@gmail.com>
  * SPDX-License-Identifier: GPL-2.0-or-later
  */
#ifndef SYNTHESIZER_H
#define SYNTHESIZER_H

#include <string>
#include <vector>
#include <cstdint>

class Dictionary;

class Synthesizer {
public:
    explicit Synthesizer();

    // The main vocoder logic
    std::vector<int16_t> vocoder(const std::vector<std::string> &diphones, float tempoFactor);

    void speak(const std::string &text, Dictionary *dict, float tempo);

private:
    // Internal helper for WAV files
    void writeWavHeader(FILE *file, int dataSize);
    void saveAndPlay(const std::vector<int16_t> &buffer);
};

#endif // SYNTHESIZER_H
