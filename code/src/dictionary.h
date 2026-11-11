/**
 * @file dictionary.h
 *
 * This file contains the Dictionary class definition.
 *
 * Copyright 2026 Alan Crispin <crispinalan@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <vector>
#include <map>
#include <ctime>

/**
 * @class Dictionary
 *
 * This class provides a dictionary-like functionality for transcribing words.
 */
class Dictionary {
public:
    explicit Dictionary();

    /**
     * Transcribes the given word using decision tree approach.
     *
     * @param word The word to be transcribed.
     * @return A vector of phonemes representing the transcription.
     */
    std::vector<std::string> transcribeWordDecisionTree(const std::string& word);

private:
    // std::map<std::string, std::vector<std::string>> m_data;
    // std::map<char, std::string> m_consonantMap;
    // std::map<std::string, std::string> m_vowelClusterMap;
    // std::map<char, std::string> m_vowelMap;

    std::map<std::string, std::vector<std::string>> m_data; /**< Map containing phoneme data for words. */

    /**
     * Consonant mapping.
     */
    std::map<char, std::string> m_consonantMap;

    /**
     * Vowel cluster mapping.
     */
    std::map<std::string, std::string> m_vowelClusterMap;

    /**
     * Vowel mapping.
     */
    std::map<char, std::string> m_vowelMap;

    /**
     * Builds a list of diphones from phonemes.
     *
     * @param phonemes The phonemes to be used for building the diphone list.
     * @return A vector of diphones.
     */
    std::vector<std::string> buildDiphoneList(const std::vector<std::string>& phonemes);

    //Decision tree
    //c=current, p=previous, n=next nn=nextnext nnn=nextnextnext
    // nn and nnn are LOOK-AHEAD

    /**
     * Predicts a phoneme based on decision tree logic for A.
     *
     * @param p Previous character.
     * @param c Current character.
     * @param n Next character.
     * @param nn Character after next.
     * @param nnn Third character after current.
     * @return The predicted phoneme.
     */
    std::string predictA(char p, char c, char n, char nn, char nnn);
    //Other predict functions have the same parameters
    std::string predictB(char p, char c, char n, char nn, char nnn);
    std::string predictC(char p, char c, char n, char nn, char nnn);
    std::string predictD(char p, char c, char n, char nn, char nnn);
    std::string predictE(char p, char c, char n, char nn, char nnn);
    std::string predictF(char p, char c, char n, char nn, char nnn);
    std::string predictG(char p, char c, char n, char nn, char nnn);
    std::string predictH(char p, char c, char n, char nn, char nnn);
    std::string predictI(char p, char c, char n, char nn, char nnn);
    std::string predictJ(char p, char c, char n, char nn, char nnn);
    std::string predictK(char p, char c, char n, char nn, char nnn);
    std::string predictL(char p, char c, char n, char nn, char nnn);
    std::string predictM(char p, char c, char n, char nn, char nnn);
    std::string predictN(char p, char c, char n, char nn, char nnn);
    std::string predictO(char p, char c, char n, char nn, char nnn);
    std::string predictP(char p, char c, char n, char nn, char nnn);
    std::string predictQ(char p, char c, char n, char nn, char nnn);
    std::string predictR(char p, char c, char n, char nn, char nnn);
    std::string predictS(char p, char c, char n, char nn, char nnn);
    std::string predictT(char p, char c, char n, char nn, char nnn);
    std::string predictU(char p, char c, char n, char nn, char nnn);
    std::string predictV(char p, char c, char n, char nn, char nnn);
    std::string predictW(char p, char c, char n, char nn, char nnn);
    std::string predictX(char p, char c, char n, char nn, char nnn);
    std::string predictY(char p, char c, char n, char nn, char nnn);
    std::string predictZ(char p, char c, char n, char nn, char nnn);

    /**
     * Initializes the lexicon with common calendar word pre-defined diphone sequences.
     */
    void initializeLexicon();

    /**
     * Sets up G2P rules.
     */
    void setupG2PRules();
};

#endif // DICTIONARY_H
