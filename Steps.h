#pragma once

#include <list>

class Steps {

private:
	long randomSeed = 0;
	std::list<std::pair<size_t, char>> steps; // pair: iteration, step

public:
	// Load and save steps from a file
	static Steps loadSteps(const std::string& filename);
	void saveSteps(const std::string& filename) const;

	// Getters and setters for random seed
	long getRandomSeed() const {
		return randomSeed;
	}
	void setRandomSeed(long seed) {
		randomSeed = seed;
	}

	// Add a step for a specific iteration
	void addStep(size_t iteration, char step) {
		steps.push_back({ iteration, step });
	}

	// Check if the next step is for the given iteration
	bool isNextStepOnIteration(size_t iteration) const {
		return !steps.empty() && steps.front().first == iteration;
	}

	// Pop and return the next step
	char popStep() {
		char step = steps.front().second;
		steps.pop_front();
		return step;
	}
};
