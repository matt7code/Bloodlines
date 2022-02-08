#ifndef TOOL_H
#define TOOL_H

#include <SFML/Graphics.hpp>

#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

//#include <states/input_string.h>

#include "constants.h"

#define LOG(x) std::cout << (x) << std::endl;

class Tool {
public:
    static float dist(const sf::Vector3i& p1, const sf::Vector3i& p2);
    static float dist(const sf::Vector3f& p1, const sf::Vector3f& p2);
    static float dist(const sf::Vector2f& p1, const sf::Vector2f& p2);
    static sf::Vector2f centerSprite(sf::Sprite& sprite);
    static sf::Vector2f convertToTileCoords(sf::Vector2f& worldPos);
    static std::string pad(std::string in, int amt = 8);
    static std::string padLeft(std::string in, int amt = 8);
    static std::string padNum(long in, int amt = 8);
    static int getRand(const int & min, const int & max);
    static std::string FormatLine(const std::string text, const int max_length);
    static void FormatLineIntoVector(const std::string& text, const int max_length, std::vector<std::string>& vecref, bool clear_vector = true);
    static std::string Clip(const std::string text, const int max_length);
    static std::string generateName(const int len = -1);
    static std::string TimeToString(int seconds);
    static int GetDirList(std::string dir, std::vector<std::string> &files);
    static std::string GetCurrentDateAndTime();
    static std::string ConvertMsToTimeString(const long ms);
    static sf::Vector2i ConvertClickToScreenPos(const sf::Vector2i click, const sf::RenderWindow& window);
    static int face(sf::Vector2f from, sf::Vector2f to);
    static std::string reconstituteString(const std::vector<std::string>& vec);
    static std::string Trim(const std::string& str, const std::string& whitespace = " /t");
    static std::vector<std::string> Tokenize(const std::string& str);
    static bool LoadVecStr(const std::string& title, std::vector<std::string>& vec, bool trim = true);
    static void DumpVecStr(const std::vector<std::string>& vec);
    static void ConvertClickToScreenPos(sf::Vector2i& click, sf::RenderWindow& window);
    static int getNumDigits(int n);
    static int getDigitAt(int num, int pos);
    static std::vector<std::string> Parse(std::string input, const char delim = ':');
};

#endif // TOOL_H
