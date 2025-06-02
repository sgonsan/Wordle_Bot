# Wordle Bot

This is a terminal-based bot that helps you play Wordle. It is written in c++ and uses a trie data structure to store the dictionary. The bot uses a simple algorithm to guess the word based on the feedback given by the user. In addition, there is two programs in python that can be used to generate the dictionary and launch the api server to play the game.

## How to use

### C++ bot

To use the bot, you need to compile the source code. You can do this by running the following command:

```bash
g++ -o wordle_bot code/main.cpp
```

Then, you can run the bot by executing the following command:

```bash
./wordle_bot
```

### Python dictionary generator

To generate the dictionary, you need to run the following command:

```bash
python3 code/generate_dictionary.py
```

This will generate two files: `dictionary.txt` and `dictionary.json`. The first file contains the list of words in the dictionary, and the second file contains the dictionary in json format.

### Python API server

To launch the api server, you need to run the following command:

```bash
python3 code/server.py
```

This will launch the api server on `http://localhost:5000`. You can use this server to play the game using the bot.

## How to play

To play the game, you need to provide the bot with the feedback for each guess. The feedback should be in the format `XXXXX`, where `X` is one of the following characters:

- `*`: The letter is in the word and in the correct position.
- `+`: The letter is in the word but in the wrong position.
- `-`: The letter is not in the word.

For example, if the word is `words` and the guess is `words`, the feedback should be `*****`. If the guess is `world`, the feedback should be `***-+`.

## How it works

The bot uses a simple algorithm to guess the word based on the feedback given by the user. The algorithm works as follows:

1. The bot starts by guessing the word `apple`.
2. The user provides feedback for the guess.
3. The bot uses the feedback to eliminate words from the dictionary that do not match the feedback.
4. The bot generates a new guess based on the remaining words in the dictionary.
5. Steps 2-4 are repeated until the bot guesses the word.

The bot uses a trie data structure to store the dictionary. This allows the bot to efficiently search for words that match the feedback.
