import json
import unicodedata

# Ruta del archivo de entrada y salida
input_file = "spanish_words.txt"  # Archivo de palabras en español, una por línea
output_file = "dictionary.json"


def remove_accents(word):
    """
    Elimina los acentos de una palabra usando unicodedata.
    """
    return "".join(
        char
        for char in unicodedata.normalize("NFD", word)
        if unicodedata.category(char) != "Mn"
    )


def create_dictionary(input_file):
    """
    Crea un diccionario organizado por longitud de palabras y elimina duplicados.
    """
    word_dict = {}
    try:
        with open(input_file, "r", encoding="utf-8") as file:
            for line in file:
                word = line.strip().lower()
                word = remove_accents(word)  # Elimina los acentos
                length = len(word)
                if length not in word_dict:
                    word_dict[length] = []
                word_dict[length].append(word)

        # Eliminar duplicados y ordenar las palabras por longitud
        for length in word_dict:
            word_dict[length] = sorted(list(set(word_dict[length])))

        # Ordenar el diccionario por longitud de palabras
        word_dict = dict(sorted(word_dict.items()))

    except FileNotFoundError:
        print(f"Error: {input_file} no encontrado.")
        return None

    return word_dict


def save_dictionary(dictionary, output_file):
    """
    Guarda el diccionario como un archivo JSON.
    """
    with open(output_file, "w", encoding="utf-8") as file:
        json.dump(dictionary, file, ensure_ascii=False, indent=4)


# Crear y guardar el diccionario
word_dict = create_dictionary(input_file)
if word_dict:
    save_dictionary(word_dict, output_file)
    print(f"Diccionario generado y guardado en {output_file}")
