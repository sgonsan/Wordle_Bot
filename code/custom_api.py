# filepath: /home/sgonsan/Projects/Wordle_Bot/code/custom_api.py
from flask import Flask, jsonify, request

app = Flask(__name__)

# Cargar palabras desde un archivo
# words_file_path = "/home/sgonsan/Projects/Wordle_Bot/data/spanish_words.txt"
files_path = "/home/sgonsan/Projects/Wordle_Bot/data/dictionary"


def load_words(languaje):
    words = []
    with open(files_path + "_" + languaje + ".txt", "r", encoding="utf-8") as file:
        for line in file:
            words.append(line.strip())
    return words


# Endpoint para obtener palabras por longitud
@app.route("/words/es", methods=["GET"])
def get_words_es():
    length = request.args.get("length", type=int)
    if not length:
        return jsonify({"error": "Please provide a word length"}), 400

    words = load_words("es")

    # Filter words by length
    filtered_words = [word for word in words if len(word) == length]

    if not filtered_words:
        return jsonify({"error": "No words found"}), 404

    # Return words as JSON
    return jsonify(filtered_words)


# Endpoint para obtener palabras por longitud
@app.route("/words/en", methods=["GET"])
def get_words_en():
    length = request.args.get("length", type=int)
    if not length:
        return jsonify({"error": "Please provide a word length"}), 400

    words = load_words("en")

    # Filter words by length
    filtered_words = [word for word in words if len(word) == length]

    if not filtered_words:
        return jsonify({"error": "No words found"}), 404

    # Return words as JSON
    return jsonify(filtered_words)


if __name__ == "__main__":
    app.run(debug=True)
