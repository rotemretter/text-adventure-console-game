#include <limits>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "Results.h"

// Convert string to Type enum
static Results::Type stringToType(const std::string& s) {
	if (s == "ScreenChange") return Results::Type::ScreenChange;
	if (s == "LifeLost")     return Results::Type::LifeLost;
	if (s == "Riddle")       return Results::Type::Riddle;
	if (s == "GameEnd")      return Results::Type::GameEnd;
	return Results::Type::None;
}

// Convert Type enum to string
static const char* typeToString(Results::Type t) {
	switch (t) {
	case Results::Type::ScreenChange: return "ScreenChange";
	case Results::Type::LifeLost:     return "LifeLost";
	case Results::Type::Riddle:       return "Riddle";
	case Results::Type::GameEnd:      return "GameEnd";
	default:                          return "None";
	}
}

// Load results from a file
Results Results::loadResults(const std::string& filename) {
	std::ifstream in(filename);
	Results res;

	if (!in.is_open()) return res;

	size_t n = 0;
	in >> n;

	for (size_t i = 0; i < n; ++i) {
		Results::Event e;
		std::string typeStr;

		in >> e.t >> typeStr;
		e.type = stringToType(typeStr);

		if (e.type == Type::ScreenChange) {
			in >> e.player >> std::quoted(e.screen);
		}
		else if (e.type == Type::LifeLost) {
			in >> e.player;
		}
		else if (e.type == Type::Riddle) {
			int corr = 0;
			in >> e.player >> std::quoted(e.riddleText) >> std::quoted(e.answer) >> corr;
			e.correct = (corr != 0);
		}
		else if (e.type == Type::GameEnd) {
			in >> e.scoreGained;
		}
		else if (e.type == Type::None) { // unknown type
			std::cout << "Invalid result file: unknown event type '" << typeStr << "'\n";
			return Results{};
		}

		res.events.push_back(std::move(e));
	}

	return res;
}

// Save results to a file
void Results::saveResults(const std::string& filename) const {
	std::ofstream out(filename);

	out << events.size();

	for (const auto& e : events) {
		out << '\n' << e.t << ' ' << typeToString(e.type);

		switch (e.type) {
		case Type::ScreenChange:
			out << ' ' << e.player << ' ' << std::quoted(e.screen);
			break;

		case Type::LifeLost:
			out << ' ' << e.player;
			break;

		case Type::Riddle:
			out << ' ' << e.player
				<< ' ' << std::quoted(e.riddleText)
				<< ' ' << std::quoted(e.answer)
				<< ' ' << (e.correct ? 1 : 0);
			break;

		case Type::GameEnd:
			out << ' ' << e.scoreGained;
			break;

		default:
			break;
		}
	}
}

void Results::addScreenChange(size_t t, int player, const std::string& toScreen) {
	Event e;
	e.t = t;
	e.type = Type::ScreenChange;
	e.player = player;
	e.screen = toScreen;
	events.push_back(std::move(e));
}

void Results::addLifeLost(size_t t, int player) {
	Event e;
	e.t = t;
	e.type = Type::LifeLost;
	e.player = player;
	events.push_back(std::move(e));
}

void Results::addRiddle(size_t t, int player, const std::string& q, const std::string& a, bool correct) {
	Event e;
	e.t = t;
	e.type = Type::Riddle;
	e.player = player;
	e.riddleText = q;
	e.answer = a;
	e.correct = correct;
	events.push_back(std::move(e));
}

void Results::addGameEnd(size_t t, int scoreGained) {
	Event e;
	e.t = t;
	e.type = Type::GameEnd;
	e.player = -1;           // not relevant
	e.scoreGained = scoreGained;
	events.push_back(std::move(e));
}

// Peek at the next event without removing it
const Results::Event* Results::peek() const {
	if (events.empty()) return nullptr;
	return &events.front();
}
