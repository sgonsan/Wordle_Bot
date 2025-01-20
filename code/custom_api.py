# filepath: /home/sgonsan/Projects/Wordle_Bot/code/custom_api.py
from flask import Flask, jsonify, request

app = Flask(__name__)

# Cargar palabras desde un archivo
# words_file_path = "/home/sgonsan/Projects/Wordle_Bot/data/spanish_words.txt"
words_file_path = "/home/sgonsan/Projects/Wordle_Bot/data/dictionary.txt"
with open(words_file_path, "r", encoding="utf-8") as file:
    words = [line.strip() for line in file.readlines()]


# Endpoint para obtener palabras por longitud
@app.route("/words", methods=["GET"])
def get_words():
    length = request.args.get("length", type=int)
    if not length:
        return jsonify({"error": "Please provide a word length"}), 400

    # Filtrar palabras por longitud
    filtered_words = [word for word in words if len(word) == length]

    # Quitar las tildes de las palabras
    filtered_words = [
        word.replace("á", "a")
        .replace("é", "e")
        .replace("í", "i")
        .replace("ó", "o")
        .replace("ú", "u")
        for word in filtered_words
    ]

    if not filtered_words:
        return jsonify({"error": "No words found"}), 404

    # Devolver un subconjunto limitado para eficiencia
    return jsonify(filtered_words)


if __name__ == "__main__":
    app.run(debug=True)
