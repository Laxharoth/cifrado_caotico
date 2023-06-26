import struct
import matplotlib.pyplot as plt
import os


def plot_random_data(filename: str):
    lista_ull = []
    with open(filename, "rb") as archivo:
        # Desempaquetar los datos en valores de unsigned long long
        tamaño_ull = struct.calcsize("Q")
        for datos_binarios in iter(lambda: archivo.read(tamaño_ull), ""):
            try:
                dato_ull = struct.unpack("Q", datos_binarios)[0]
                lista_ull.append(dato_ull)
            except struct.error as e:
                print("Finish reading file")
                break
    X = list(range(len(lista_ull)))
    plt.plot(X, lista_ull)
    plt.savefig(f"img/{''.join(filename.split('.')[:-1])}/.{0}-{X[-1]}.png")
    for i in range(10):
        plt.cla()
        plt.plot(X[255 * i : 255 * (i + 1)], lista_ull[255 * i : 255 * (i + 1)])
        if not os.path.exists(f"img/{''.join(filename.split('.')[:-1])}"):
            os.makedirs(f"img/{''.join(filename.split('.')[:-1])}")
        plt.savefig(f"img/{''.join(filename.split('.')[:-1])}/{255*i}-{255*(i+1)}.png")


def main():
    plot_random_data("random_data.bin")
    plot_random_data("random_data_2.bin")


if __name__ == "__main__":
    main()
