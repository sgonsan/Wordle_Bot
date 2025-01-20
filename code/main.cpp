#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "json.hpp"

using json = nlohmann::json;

// Function declarations
json loadDictionary(const std::string &filename);
std::vector<std::string> filterWords(const std::vector<std::string> &words,
                                     const std::string &guess,
                                     const std::string &feedback);
bool validateFeedback(const std::string &feedback, int wordLength);
void playGame(const json &dictionary);

int main() {
  // Load dictionary
  std::string filename = "dictionary.json";
  json dictionary = loadDictionary(filename);

  if (dictionary.empty()) {
    std::cerr << "Error: Failed to load dictionary from " << filename
              << std::endl;
    return 1;
  }

  // Start the game
  playGame(dictionary);

  return 0;
}

// Function to load dictionary from a JSON file
json loadDictionary(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error: Unable to open dictionary file." << std::endl;
    return {};
  }

  json dictionary;
  try {
    file >> dictionary;
  } catch (const json::parse_error &e) {
    std::cerr << "Error: Failed to parse dictionary JSON. " << e.what()
              << std::endl;
    return {};
  }

  return dictionary;
}

// Function to filter words based on the feedback provided by the user
std::vector<std::string> filterWords(const std::vector<std::string> &words,
                                     const std::string &guess,
                                     const std::string &feedback) {
  std::vector<std::string> filteredWords;

  for (const auto &word : words) {
    bool valid = true;

    for (size_t i = 0; i < feedback.size(); ++i) {
      char fb = feedback[i];
      char ch = guess[i];

      if (fb == 'B' && word[i] != ch) {  // 'B' -> Correct letter and position
        valid = false;
        break;
      } else if (fb == 'C' &&
                 (word[i] == ch ||
                  word.find(ch) ==
                      std::string::npos)) {  // 'C' -> Correct letter, wrong
                                             // position
        valid = false;
        break;
      } else if (fb == 'M' &&
                 word.find(ch) !=
                     std::string::npos) {  // 'M' -> Incorrect letter
        valid = false;
        break;
      }
    }

    if (valid) {
      filteredWords.push_back(word);
    }
  }

  return filteredWords;
}

// Function to validate the feedback std::string
bool validateFeedback(const std::string &feedback, int wordLength) {
  if (feedback.size() != wordLength) {
    std::cerr << "Error: Feedback length must match the word length."
              << std::endl;
    return false;
  }

  for (char ch : feedback) {
    if (ch != 'B' && ch != 'C' && ch != 'M') {
      std::cerr << "Error: Feedback must only contain 'B', 'C', or 'M'."
                << std::endl;
      return false;
    }
  }

  return true;
}

// Main game function
void playGame(const json &dictionary) {
  int wordLength;
  std::cout << "Enter the word length: ";
  std::cin >> wordLength;

  if (dictionary.find(std::to_string(wordLength)) == dictionary.end()) {
    std::cout << "No words of length " << wordLength
              << " found in the dictionary." << std::endl;
    return;
  }

  std::vector<std::string> words =
      dictionary[std::to_string(wordLength)].get<std::vector<std::string>>();
  std::vector<std::string> possibleWords = words;

  int attempts = 0;
  std::string feedback;

  while (true) {
    if (possibleWords.empty()) {
      std::cout << "No possible words match the feedback given. Exiting..."
                << std::endl;
      break;
    }

    // Select the first word as the guess (can be optimized)
    std::string guess = possibleWords.front();
    std::cout << "Bot guesses: " << guess << std::endl;

    std::cout
        << "Enter feedback (B for correct, C for correct but wrong position, "
           "M for incorrect): ";
    std::cin >> feedback;

    // Validate feedback
    if (!validateFeedback(feedback, wordLength)) {
      continue;
    }

    attempts++;

    if (feedback == std::string(wordLength, 'B')) {
      std::cout << "Bot solved the Wordle in " << attempts << " attempts!"
                << std::endl;
      break;
    }

    // Filter words based on feedback
    possibleWords = filterWords(possibleWords, guess, feedback);
  }
}
