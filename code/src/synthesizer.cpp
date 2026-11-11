 /*
  * Copyright 2026 Alan Crispin <crispinalan@gmail.com>
  * SPDX-License-Identifier: GPL-2.0-or-later
  */
#include "synthesizer.h"
#include "diphone.h" // Needed for DiphoneEntry and finding diphones
#include "dictionary.h"
#include <cmath>
#include <cstdlib>  // For system()
#include <iostream>
#include <thread>   // For threading
#include <chrono>   // For sleep

/**
 * @brief Synthesizer constructor.
 */
Synthesizer::Synthesizer() {
     // std::cout << "Synthesizer constructor called!" << std::endl;
}

/**
 * @brief Generates spoken audio from a text string using the provided dictionary and tempo.
 * @param text The input text to be spoken.
 * @param dict Pointer to the Dictionary instance for diphone lookup or generation.
 * @param tempo Tempo factor (value between 1.0 – 2.0 where 10 = 1.0).
 */
void Synthesizer::speak(const std::string &text, Dictionary *dict, float tempo) {
    // Split text into words
    std::vector<std::string> words;
    std::string currentWord;
    
    for (char c : text) {
        if (std::isspace(c)) {
            if (!currentWord.empty()) {
                words.push_back(currentWord);
                currentWord.clear();
            }
        } else {
            currentWord += std::tolower(c);
        }
    }
    
    if (!currentWord.empty()) {
        words.push_back(currentWord);
    }

    std::vector<std::string> allDiphones;

    for (const std::string &word : words) {
        // transcribe word to diphone sequence
        auto diphones = dict->transcribeWordDecisionTree(word);
        allDiphones.insert(allDiphones.end(), diphones.begin(), diphones.end());
    }

    if (!allDiphones.empty()) {
        std::vector<int16_t> audio = vocoder(allDiphones, tempo);
        saveAndPlay(audio);
    }
}

/**
 * @brief Vocoder that converts a list of diphone names into raw PCM samples.
 * @param diphones List of diphone identifiers to synthesize.
 * @param tempoFactor Tempo scaling factor (e.g., 1.0 = normal speed).
 * @return Vector of signed 16‑bit audio samples ready for playback.
 */
std::vector<int16_t> Synthesizer::vocoder(const std::vector<std::string> &diphones, float tempoFactor) {
    std::vector<int16_t> output;
    const int FRAME_SIZE = 160;
    const int analysis_step = 80;
    int synthesis_step = static_cast<int>(analysis_step / tempoFactor);
    int currentOutStart = 0;

    for (const std::string &name : diphones) {
        DiphoneEntry* entry = Diphone::findDiphoneEntry(name.c_str());
        if (!entry) continue;

        const int16_t* source = reinterpret_cast<const int16_t*>(entry->data + 44);
        int sourceSamples = (entry->size - 44) / sizeof(int16_t);

        int currentIn = 0;
        int currentOut = currentOutStart;

        while (currentIn + FRAME_SIZE < sourceSamples) {
            if (currentOut + FRAME_SIZE > static_cast<int>(output.size())) {
                output.resize(currentOut + FRAME_SIZE, 0);
            }

            for (int i = 0; i < FRAME_SIZE; ++i) {
                // TRIANGLE WINDOW:
                // Fade in for the first half, Fade out for the second half
                float window;
                if (i < FRAME_SIZE / 2)
                    window = static_cast<float>(i) / (FRAME_SIZE / 2);
                else
                    window = static_cast<float>(FRAME_SIZE - i) / (FRAME_SIZE / 2);

                int32_t currentSample = output[currentOut + i];
                // Apply the window to the source to smooth the edges
                int32_t newSample = static_cast<int32_t>(source[currentIn + i] * window);

                int32_t combined = currentSample + newSample;
                output[currentOut + i] = static_cast<int16_t>(std::max(-32768, std::min(32767, combined)));
            }

            currentIn += analysis_step;
            currentOut += synthesis_step;
        }
        // the next diphone must overlap with the end of the last one
        currentOutStart = currentOut - synthesis_step;
    }

    // Normalization logic
    if (output.empty()) return output;
    int16_t maxVal = 0;
    for (int16_t sample : output) {
        if (std::abs(sample) > maxVal) maxVal = std::abs(sample);
    }

    if (maxVal > 0) {
        float scaleFactor = 29500.0f / maxVal;
        for (size_t i = 0; i < output.size(); ++i) {
            int32_t amplified = static_cast<int32_t>(output[i] * scaleFactor);
            output[i] = static_cast<int16_t>(std::max(-32768, std::min(32767, amplified)));
        }
    }
    return output;
}

/**
 * @brief Writes the generated audio buffer to a WAV file and plays it back.
 * @param buffer Vector of signed 16‑bit PCM samples produced by the vocoder.
 */
void Synthesizer::saveAndPlay(const std::vector<int16_t> &buffer) {
    // Write to WAV file
    FILE* file = fopen("diphone_out.wav", "wb");
    if (file) {
        writeWavHeader(file, buffer.size() * sizeof(int16_t));
        fwrite(buffer.data(), sizeof(int16_t), buffer.size(), file);
        fclose(file);
        
        // Print a message and play audio using system call in background
        //std::cout << "Audio saved to diphone_out.wav" << std::endl;
        //std::cout << "Playing audio in background..." << std::endl;
        
        // Run aplay in background thread
        std::thread playThread([]() {
            int result = system("aplay diphone_out.wav");
            if (result != 0) {
                std::cerr << "Failed to play audio with aplay" << std::endl;
            }
        });
        
        playThread.join(); // Wait for playback to finish
    } else {
        std::cerr << "Failed to save audio file!" << std::endl;
    }
}

/**
 * @brief Writes a minimal RIFF/WAV header to the given file.
 * @param file Pointer to an open FILE for writing.
 * @param dataSize Size of the audio data in bytes (excluding header).
 */
void Synthesizer::writeWavHeader(FILE *file, int dataSize) {
    // RIFF Header
    fwrite("RIFF", 1, 4, file);
    int fileSize = 36 + dataSize;
    fwrite(&fileSize, sizeof(int), 1, file);
    
    // WAVE Format
    fwrite("WAVEfmt ", 1, 8, file);
    int fmtSize = 16;
    fwrite(&fmtSize, sizeof(int), 1, file);
    
    // Format information
    short format = 1; // PCM
    fwrite(&format, sizeof(short), 1, file);
    
    short channels = 1; // Mono
    fwrite(&channels, sizeof(short), 1, file);
    
    int sampleRate = 16000;
    fwrite(&sampleRate, sizeof(int), 1, file);
    
    int byteRate = sampleRate * 2;
    fwrite(&byteRate, sizeof(int), 1, file);
    
    short blockAlign = 2;
    fwrite(&blockAlign, sizeof(short), 1, file);
    
    short bps = 16;
    fwrite(&bps, sizeof(short), 1, file);
    
    // Data chunk
    fwrite("data", 1, 4, file);
    fwrite(&dataSize, sizeof(int), 1, file);
}
