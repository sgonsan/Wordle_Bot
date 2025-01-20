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
    bool valid = true;  // Assume the word is valid
    std::vector<bool> used(
        word.size(),
        false);  // Track positions already validated in the candidate word

    for (size_t i = 0; i < feedback.size(); ++i) {
      char fb = feedback[i];      // Feedback actual (B, C, M)
      char guessChar = guess[i];  // Letra en el guess actual
      char wordChar =
          word[i];  // Letra de la palabra candidata en la misma posición

      if (fb == 'B') {
        // 'B': La letra está en la posición correcta
        if (wordChar != guessChar) {
          valid = false;  // Si no coincide exactamente, la palabra no es válida
          break;
        }
        used[i] = true;  // Marca esta posición como utilizada
      } else if (fb == 'C') {
        // 'C': La letra está en la palabra pero en otra posición
        bool found = false;
        for (size_t j = 0; j < word.size(); ++j) {
          if (!used[j] && word[j] == guessChar && j != i) {
            found = true;    // Find the letter in a different valid position
            used[j] = true;  // Mark this position as used
            break;
          }
        }
        if (!found) {
          valid = false;  // If not found, the word is not valid
          break;
        }
      } else if (fb == 'M') {
        // 'M': La letra no debe estar en la palabra
        if (word.find(guessChar) != std::string::npos) {
          valid = false;  // If the letter is in any position, it is not valid
          break;
        }
      }
    }

    if (valid) {
      filteredWords.push_back(word);  // Add the word if it passed all checks
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
    std::string guess = (attempts == 0) ? "caida" : possibleWords[0];
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
