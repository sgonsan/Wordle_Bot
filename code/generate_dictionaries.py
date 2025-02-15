import os
import re
from collections import Counter
import json

# Configuración

# Archivo de salida
output_file = "/home/sgonsan/Proyects/Wordle_Bot/data/dictionary"

# Carpeta donde están los textos
texts_folder = "/home/sgonsan/Proyects/Wordle_Bot/texts"


# Función para limpiar y normalizar texto
def clean_text(text):
    # Eliminar todo excepto letras y espacios
    text = re.sub(r"[^a-zA-ZáéíóúÁÉÍÓÚüÜñÑ\s]", "", text)
    # Reemplazar múltiples espacios por uno solo
    text = re.sub(r"\s+", " ", text)
    # Convertir a minúsculas
    text = text.lower()
    # Eliminar espacios iniciales y finales
    return text.strip()


# Cargar y procesar textos
for languaje in os.listdir(texts_folder):
    word_counter = Counter()
    languaje_folder = os.path.join(texts_folder, languaje)
    for file_name in os.listdir(languaje_folder):
        file_path = os.path.join(languaje_folder, file_name)
        with open(file_path, "r", encoding="utf-8") as file:
            text = file.read()
            text = clean_text(text)
            words = text.split()
            word_counter.update(words)
    # Crear diccionario ordenado por frecuencia
    sorted_words = word_counter.most_common()
    dictionary = {word: count for word, count in sorted_words}

    # Guardar como JSON
    with open(output_file + "_" + languaje + ".json", "w", encoding="utf-8") as file:
        json.dump(dictionary, file, ensure_ascii=False, indent=4)

    # Guardar como archivo de texto
    with open(output_file + "_" + languaje + ".txt", "w", encoding="utf-8") as file:
        for word, _ in sorted_words:
            file.write(f"{word}\n")

    print(f"Saving dictionary for {languaje} with {len(dictionary)} words")
