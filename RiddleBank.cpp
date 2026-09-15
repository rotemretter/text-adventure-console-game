#include "RiddleBank.h"

// Loads riddles from a file into the riddle bank.
bool RiddleBank::loadRiddleFromFile(const std::string& filename) {
    std::ifstream f(filename);
    if (!f.is_open()) return false;

	// Clear existing riddles
    riddleBank.clear();
    riddleBag.clear();
    solved.clear();

    std::string line;

    while (true) {
        // Skip empty lines to find the start of a riddle (Q:)
        do {
            if (!std::getline(f, line)) {
                // End of file reached
                if (riddleBank.empty()) return false;

                // Initialize the solved tracking vector and prepare the first shuffle
                solved.assign(riddleBank.size(), false);
                refillAndShuffle();
                return true;
            }
            trimInPlace(line);
        } while (line.empty());

        // Validate Question prefix
        if (!startsWith(line, "Q:")) return false;

        std::string question = line.substr(2);
        trimInPlace(question);

        // Parse exactly 4 multiple-choice options
        std::vector<std::string> options;
        options.reserve(4);

        for (int i = 1; i <= 4; i++) {
            if (!std::getline(f, line)) return false;
            trimInPlace(line);

            std::string prefix = std::to_string(i) + ":";
            if (!startsWith(line, prefix)) return false;

            std::string opt = line.substr(prefix.size());
            trimInPlace(opt);
            options.push_back(opt);
        }

        // Parse Correct Answer prefix (C:)
        if (!std::getline(f, line)) return false;
        trimInPlace(line);

        if (!startsWith(line, "C:")) return false;

        std::string cStr = line.substr(2);
        trimInPlace(cStr);

        int correct1to4 = 0;
        try {
            correct1to4 = std::stoi(cStr);
        }
        catch (...) {
            return false;// Invalid integer format
        }

        if (correct1to4 < 1 || correct1to4 > 4) return false;

        int correct0to3 = correct1to4 - 1;

        // Store the parsed riddle
        riddleBank.emplace_back(question, options, correct0to3);
    }
}


// Refills the riddle bag with unsolved riddles and shuffles them.
void RiddleBank::refillAndShuffle() {
    riddleBag.clear();
    for (int i = 0; i < (int)riddleBank.size(); ++i) {
        if (i < (int)solved.size() && solved[i]) continue;// Skip indices that are already marked as solved
        riddleBag.push_back(i);
    }

    // Shuffle using the internal Mersenne Twister engine (rng)
    std::shuffle(riddleBag.begin(), riddleBag.end(), rng);
}


// Picks a random riddle index from the riddle bank.
int RiddleBank::pickRandomRiddleIndex() {
    if (riddleBank.empty()) return -1;

    // Lazy initialization of the solved vector if sizes don't match
    if (solved.size() != riddleBank.size())
        solved.assign(riddleBank.size(), false);

    // If the shuffle-bag is empty, refill it with remaining unsolved riddles
    if (riddleBag.empty())
        refillAndShuffle();

    // If still empty (meaning all riddles are solved), return -1
    if (riddleBag.empty())
        return -1;

    // Get the last riddle index from the shuffled bag
    int idx = riddleBag.back();
    riddleBag.pop_back();
    return idx;
}









