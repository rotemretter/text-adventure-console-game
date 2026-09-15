#pragma once

#include <list>

class Results {

public:
	enum class Type { ScreenChange, LifeLost, Riddle, GameEnd, None };

	// Represents a single event in the game
	struct Event {
		size_t t = 0;                 // Time iteration of the event
		Type type = Type::None;		  // Type of event

		int player = -1;              // Player involved (-1 if not relevant)
		std::string screen;           // New screen for ScreenChange
		std::string riddleText;       // Riddle question for Riddle event
		std::string answer;           // Player's answer for Riddle event
		bool correct = false;         // Whether the answer was correct for Riddle event
		int scoreGained = 0;          // Score gained for GameEnd event
	};

private:
	//std::list<std::pair<size_t, ResultValue>> results; // pair: iteration, result
	std::list<Event> events;

public:
	// Load and save results from a file
	static Results loadResults(const std::string& filename);
	void saveResults(const std::string& filename) const;

	void addScreenChange(size_t t, int player, const std::string& toScreen);
	void addLifeLost(size_t t, int player);
	void addRiddle(size_t t, int player, const std::string& q, const std::string& a, bool correct);
	void addGameEnd(size_t t, int scoreGained);
	const Event* peek() const;

	// Pop and return the next result
	Results::Event popResult() {
		if (events.empty()) return Results::Event{};
		Results::Event e = std::move(events.front());
		events.pop_front();
		return e;
	}


};