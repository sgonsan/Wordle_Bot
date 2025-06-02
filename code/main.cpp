#include <curl/curl.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "json.hpp"

using json = nlohmann::json;

// Callback function for cURL to store response
size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                     std::string *s) {
  size_t newLength = size * nmemb;
  try {
    s->append((char *)contents, newLength);
  } catch (std::bad_alloc &e) {
    // Handle memory problem
    return 0;
  }
  return newLength;
}

// Function to fetch words from API
std::vector<std::string> fetchWordsFromAPI(int wordLength,
                                           std::string language) {
  const std::string apiUrl = "http://localhost:5000/words/" + language +
                             "?length=" + std::to_string(wordLength);
  std::string response;

  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
    }

    curl_easy_cleanup(curl);
  } else {
    std::cerr << "Failed to initialize cURL." << std::endl;
  }

  // Parse JSON response
  std::vector<std::string> words;
  try {
    json jsonResponse = json::parse(response);
    words = jsonResponse.get<std::vector<std::string>>();
  } catch (const json::parse_error &e) {
    std::cerr << "Error parsing JSON response: " << e.what() << std::endl;
    std::cerr << "Response: " << response << std::endl;
  }

  return words;
}

// Remove accents from a word
std::string removeAccents(const std::string &word) {
  std::set<int> accents = {'á', 'é', 'í', 'ó', 'ú', 'ñ', 'ü',
                           'Á', 'É', 'Í', 'Ó', 'Ú', 'Ñ', 'Ü'};
  std::string result = word;
  for (char ch : word) {
    if (accents.find(ch) != accents.end()) {
      result = result.erase(result.find(ch), 1);
    }
  }
  return result;
}

// Function to filter words based on the feedback provided by the user
std::vector<std::string> filterWords(
    const std::vector<std::string> &possibleWords, const std::string &guess,
    const std::string &feedback) {
  std::vector<std::string> filteredWords;
  std::string normalizedGuess = removeAccents(guess);

  for (const auto &word : possibleWords) {
    std::string normalizedWord = removeAccents(word);

    if (normalizedWord.length() != guess.length() ||
        feedback.length() != guess.length()) {
      std::cerr << "Skipping '" << normalizedWord
                << "' because the word length (" << normalizedWord.length()
                << ") does not " << "match the guess length (" << guess.length()
                << ")." << std::endl;
      continue;
    }

    bool match = true;
    std::vector<bool> used(normalizedWord.length(), false);

    // First pass: check for correct letters in correct positions (B)
    for (size_t i = 0; i < normalizedWord.length(); ++i) {
      if (feedback[i] == '*' && normalizedWord[i] != guess[i]) {
        match = false;
        break;
      }
      if (feedback[i] == '*') {
        used[i] = true;
      }
    }
    if (!match) continue;

    // Second pass: check for correct letters in wrong positions (C)
    for (size_t i = 0; i < normalizedWord.length(); ++i) {
      if (feedback[i] == '+') {
        bool found = false;
        for (size_t j = 0; j < normalizedWord.length(); ++j) {
          if (!used[j] && normalizedWord[j] == guess[i] && i != j) {
            found = true;
            used[j] = true;
            break;
          }
        }
        if (!found) {
          match = false;
          break;
        }
      }
    }
    if (!match) continue;

    // Third pass: check for incorrect letters (M)
    for (size_t i = 0; i < normalizedWord.length(); ++i) {
      if (feedback[i] == '-') {
        for (size_t j = 0; j < normalizedWord.length(); ++j) {
          if (normalizedWord[j] == guess[i] && !used[j]) {
            match = false;
            break;
          }
        }
      }
    }
    if (match) {
      filteredWords.push_back(normalizedWord);
    }
  }
  return filteredWords;
}

// Function to validate the feedback std::string
bool validateFeedback(const std::string &feedback, int wordLength) {
  if (int(feedback.size()) != wordLength) {
    std::cerr << "Error: Feedback length must match the word length."
              << std::endl;
    return false;
  }

  for (char ch : feedback) {
    if (ch != '*' && ch != '+' && ch != '-') {
      std::cerr
          << "Error: Feedback must only contain '*', '+' or '-' characters."
          << std::endl;
      return false;
    }
  }

  return true;
}

// Select the best guess based on the feedback
std::string selectBestGuess(const std::vector<std::string> &possibleWords,
                            bool isFirstGuess) {
  if (possibleWords.size() == 1) {
    return possibleWords[0];  // If
  }

  // First guess: Prioritize words with most vowels
  if (isFirstGuess) {
    std::string bestWord;
    size_t maxVowels = 0;

    for (const auto &word : possibleWords) {
      std::set<char> vowels;
      for (char c : word) {
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
          vowels.insert(c);
        }
      }
      if (vowels.size() > maxVowels) {
        maxVowels = vowels.size();
        bestWord = word;
      }
    }
    return bestWord;
  }

  // Subsequent guesses: Prioritize words with most frequent letters
  std::map<char, int> letterFrequency;
  for (const auto &word : possibleWords) {
    for (char c : word) {
      letterFrequency[c]++;
    }
  }

  std::string bestGuess;
  int maxScore = -1;

  for (const auto &word : possibleWords) {
    int score = 0;
    std::set<char> uniqueLetters(word.begin(), word.end());

    for (char c : uniqueLetters) {
      score += letterFrequency[c];
    }

    if (score > maxScore) {
      maxScore = score;
      bestGuess = word;
    }
  }

  bestGuess = removeAccents(bestGuess);

  return bestGuess;
}

// Function to play the game
void playGame() {
  int wordLength;
  std::string language, wordLengthStr;

  while (true) {
    std::cout << "Enter the language (en, es): ";  // fr, de, it
    std::cin >> language;

    if (language != "en" && language != "es") {
      std::cerr << "Invalid language. Please enter one of: en, es."
                << std::endl;
    } else {
      break;
    }
  }

  while (true) {
    std::cout << "Enter the word length: ";
    std::cin >> wordLengthStr;

    try {
      wordLength = std::stoi(wordLengthStr);
      if (wordLength <= 0) {
        throw std::invalid_argument("Word length must be a positive integer.");
      }
    } catch (const std::invalid_argument &e) {
      std::cerr << "Invalid input. Please enter a valid positive integer."
                << std::endl;
      continue;
    } catch (const std::out_of_range &e) {
      std::cerr << "Word length is out of range. Please enter a smaller number."
                << std::endl;
      continue;
    }
    break;
  }

  std::vector<std::string> possibleWords =
      fetchWordsFromAPI(wordLength, language);

  for (auto &word : possibleWords) {
    word = removeAccents(word);  // Normalize words by removing accents
  }

  if (possibleWords.empty()) {
    std::cerr << "No words of length " << wordLength
              << " available from the API." << std::endl;
    return;
  }

  std::cout << "Total possible words: " << possibleWords.size() << std::endl;

  int attempts = 0;
  std::string feedback;
  bool isFirstGuess = true;

  while (true) {
    if (possibleWords.empty()) {
      std::cerr << "No possible words match the feedback given. Exiting..."
                << std::endl;
      break;
    }

    // Seleccionar la mejor palabra
    std::string guess = selectBestGuess(possibleWords, isFirstGuess);
    isFirstGuess = false;

    std::cout << "Bot guesses: " << guess << std::endl;

    std::cout
        << "Enter feedback (* for correct, + for correct but wrong position, "
           "- for incorrect): ";
    std::cin >> feedback;

    if (!validateFeedback(feedback, wordLength)) {
      continue;
    }

    attempts++;

    if (feedback == std::string(wordLength, '*')) {
      std::cout << "Bot solved the Wordle in " << attempts << " attempts!"
                << std::endl;
      break;
    }

    possibleWords = filterWords(possibleWords, guess, feedback);

    std::cout << "Remaining possible words: " << possibleWords.size()
              << std::endl;
  }
}

int main() {
  // Start the game
  playGame();

  return 0;
}