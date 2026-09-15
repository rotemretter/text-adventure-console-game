#pragma once
#include <vector>
#include "Riddle.h"
#include <fstream>
#include <algorithm>
#include <cctype>
#include <random>
#include <numeric>

using std::vector;
using std::string;

class RiddleBank
{
	vector<Riddle> riddleBank;
	vector<int> riddleBag;
	vector<bool> solved;
	std::mt19937 rng{ std::random_device{}() };

public:
	bool loadRiddleFromFile(const std::string& filename);
	void refillAndShuffle();

	bool empty() const { return riddleBank.empty(); }

	int  pickRandomRiddleIndex();

	// Helper functions
	static void trimInPlace(std::string& s) {
		auto notSpace = [](unsigned char c) { return !std::isspace(c); };
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
		s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
	}

	// Checks if string s starts with prefix
	static bool startsWith(const std::string& s, const std::string& prefix) {
		return s.rfind(prefix, 0) == 0;
	}

	// Returns the riddle at the specified index.
	const Riddle& getRiddle(int idx) const {
		return riddleBank[idx];
	}

	// Marks the riddle at the specified index as solved.
	void markSolved(int idx) {
		if (idx >= 0 && idx < (int)solved.size())
			solved[idx] = true;
	}
};






