from PIL import Image
import os


def convert_jpg_to_pgm(input_directory, output_directory):
    # Crear el directorio de salida si no existe
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # Recorrer todos los archivos y subdirectorios en el directorio de entrada
    for root, _, files in os.walk(input_directory):
        for file in files:
            if file.endswith(".jpg") or file.endswith(".jpeg"):
                # Ruta completa del archivo
                file_path = os.path.join(root, file)

                # Abrir la imagen
                with Image.open(file_path) as img:
                    # Convertir la imagen a escala de grises
                    gray_img = img.convert("L")

                    # Calcular la ruta de salida manteniendo la estructura de directorios
                    relative_path = os.path.relpath(root, input_directory)
                    output_path_dir = os.path.join(output_directory, relative_path)

                    # Crear el directorio de salida si no existe
                    if not os.path.exists(output_path_dir):
                        os.makedirs(output_path_dir)

                    # Cambiar la extensión a .pgm
                    base_filename = os.path.splitext(file)[0]
                    output_filename = base_filename + ".pgm"
                    output_path = os.path.join(output_path_dir, output_filename)

                    # Guardar la imagen en formato PGM
                    gray_img.save(output_path, format="PPM")

                    print(f"Convertido: {file_path} -> {output_path}")


# Directorio de entrada y salida
input_directory = "img/animal_data/"
output_directory = "img/animal_data_pgm/"

# Llamar a la función de conversión
convert_jpg_to_pgm(input_directory, output_directory)
