#pragma once
#include <string>
#include <vector>
#include "Point.h"

using  std::vector;
using std::string;

class Riddle {
public:
    static constexpr char RIDDLE_CHAR = '?';

private:
    Point location;
    string question;
    vector <string> options;
    int correctIndex = 0;

public:
	// Default constructor
    Riddle() = default;

    Riddle(const string& q, const vector<string>& opts, int correct)
        : question(q), options(opts), correctIndex(correct) {
    }

    const Point& getLocation() const { return location; }
    const string& getQuestion() const { return question; }
    const vector<string>& getOptions() const { return options; }
    int getCorrectIndex() const { return correctIndex; }
};






