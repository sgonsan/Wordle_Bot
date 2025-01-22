import os
import re
from collections import Counter
import json

# Configuración

# Archivo de salida
json_file = "/home/sgonsan/Projects/Wordle_Bot/data/dictionary.json"
txt_file = "/home/sgonsan/Projects/Wordle_Bot/data/dictionary.txt"

# Carpeta donde están los textos
text_folder = "/home/sgonsan/Projects/Wordle_Bot/texts"


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
word_counter = Counter()
for filename in os.listdir(text_folder):
    if filename.endswith(".txt"):
        with open(os.path.join(text_folder, filename), "r", encoding="utf-8") as file:
            text = file.read()
            text = clean_text(text)
            words = text.split()
            word_counter.update(words)

# Crear diccionario ordenado por frecuencia
sorted_words = word_counter.most_common()
dictionary = {word: count for word, count in sorted_words}

# Guardar como JSON
with open(json_file, "w", encoding="utf-8") as file:
    json.dump(dictionary, file, ensure_ascii=False, indent=4)

# Guardar como archivo de texto
with open(txt_file, "w", encoding="utf-8") as file:
    for word, _ in sorted_words:
        file.write(f"{word}\n")

print(f"Diccionario generado con {len(dictionary)} palabras.")
