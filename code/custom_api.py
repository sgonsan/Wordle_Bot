from flask import Flask, jsonify, request
import json
import random

app = Flask(__name__)

# Cargar el diccionario generado
with open(
    "/home/sgonsan/Projects/Wordle_Bot/data/dictionary.json", "r", encoding="utf-8"
) as file:
    dictionary = json.load(file)


# Endpoint para obtener palabras por longitud
@app.route("/words", methods=["GET"])
def get_words():
    length = request.args.get("length", type=int)
    if not length:
        return jsonify({"error": "Please provide a word length"}), 400

    # Filtrar palabras por longitud
    filtered_words = [word for word in dictionary.keys() if len(word) == length]

    # Si no hay palabras disponibles
    if not filtered_words:
        return jsonify({"error": f"No words of length {length} available."}), 404

    # Elegir un subconjunto limitado de palabras
    max_words = 50  # Número máximo de palabras por solicitud
    selected_words = random.sample(filtered_words, min(len(filtered_words), max_words))

    return jsonify(selected_words)


# Endpoint para obtener palabras más frecuentes
@app.route("/words/frequent", methods=["GET"])
def get_frequent_words():
    length = request.args.get("length", type=int)
    if not length:
        return jsonify({"error": "Please provide a word length"}), 400

    # Filtrar palabras por longitud
    filtered_words = [
        (word, count) for word, count in dictionary.items() if len(word) == length
    ]

    # Ordenar por frecuencia y limitar el número de palabras
    filtered_words.sort(key=lambda x: x[1], reverse=True)
    top_words = [word for word, count in filtered_words[:10]]  # Las 10 más frecuentes

    return jsonify(top_words)


if __name__ == "__main__":
    app.run(debug=True)
