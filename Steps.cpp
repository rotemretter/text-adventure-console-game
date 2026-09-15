#include <fstream>
#include <iostream>
#include "Steps.h"

Steps Steps::loadSteps(const std::string& filename) {
	Steps steps;
	std::ifstream steps_file(filename);

	// Check if file opened successfully
	if (!steps_file.is_open()) {
		std::cerr << "Error: File '" << filename << "' not found or could not be opened." << std::endl;
		return steps;
	}

	// Read random seed
	steps_file >> steps.randomSeed;

	// Read random seed and steps
	steps_file >> steps.randomSeed;

	// Read number the of steps that saved
	size_t size;
	steps_file >> size;

	// Until EOF or size reached, read each step
	while (!steps_file.eof() && size-- != 0) {
		size_t iteration;
		char step;
		steps_file >> iteration >> step;
		steps.addStep(iteration, step);
	}

	// Close the file
	steps_file.close();
	return steps;
}

void Steps::saveSteps(const std::string& filename) const {
	// Open file for writing
	std::ofstream steps_file(filename);

	// Write random seed and number of steps
	steps_file << randomSeed << '\n' << steps.size();

	// Write each step
	for (const auto& step : steps) {
		steps_file << '\n' << step.first << ' ' << step.second;
	}
	steps_file.close();
}
