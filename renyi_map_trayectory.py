import os
import struct
from dataclasses import dataclass
from typing import List, Callable, Any

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
from numpy.typing import NDArray

ull = np.uint64
byte = np.uint8


def find_most_significant_bit(value: ull) -> ull:
    msb: ull = ull(0)
    one: ull = ull(1)
    while value > 1:
        value >>= one
        msb += one

    return msb


@dataclass
class LogisticConstants:
    max_x: ull
    t: ull
    r: ull


MAX_ULL = 0xFFFFFFFFFFFFFFFF

logistic_constants: dict[int, LogisticConstants] = {}
for r in range(1, 11):
    t = ull(2) * ull(np.sqrt(MAX_ULL / r))
    max_x = (ull(1) << find_most_significant_bit(t)) - ull(1)

    logistic_constants[r] = LogisticConstants(max_x=max_x, t=t, r=ull(r))


def read_ull(filename: str) -> List[ull]:
    ulls: List[ull] = []
    with open(filename, "rb") as file:
        buffer = file.read(8)
        while buffer:
            try:
                value = struct.unpack("Q", buffer)[0]
                ulls.append(ull(value))
                buffer = file.read(8)
            except struct.error:
                return ulls
    return ulls


def main():
    filenames = ["random_data.bin", "random_data2.bin", "random_data3.bin"]
    for filename in filenames:
        data = read_ull(filename)
        name_without_extension = "".join(filename.split(".")[:-1])
        batch_size = 255
        for i in range(10):
            start = i * batch_size
            end = (i + 1) * batch_size
            if not os.path.exists("img/" + f"{name_without_extension}/"):
                os.makedirs("img/" + f"{name_without_extension}/")
            plt.plot(list(range(start, end)), data[start:end])
            plt.savefig(
                "img/"
                + f"{name_without_extension}/"
                + f"{start:0>4}-{end:0>4}"
                + ".png"
            )
            plt.cla()


def logistic_map(x: NDArray[np.floating[Any]], r: byte):
    return r * x * (logistic_constants[r].t - x)


def circle_map(x: NDArray[np.floating[Any]]):
    a = (2**64 - 1) / 2
    b = (2**64 - 1) * 3 / 8
    r = (2**64 - 1) * 5 / 8
    return np.sqrt(r**2 - (x - a) ** 2) + b


def graficar_funcion(
    func: Callable[[NDArray[np.floating[Any]]], float],
    start: float,
    end: float,
    num_puntos: int,
):
    """_summary_

    Args:
        f (Callable[[float], float]): _description_
        a (float): _description_
        b (float): _description_
        num_puntos (float): _description_
    """
    x_val = np.linspace(start, end, num_puntos)
    y_val = func(x_val)

    plt.plot(x_val, y_val)
    plt.xlabel("x")
    plt.ylabel("f(x)")
    plt.title("Gráfica de f(x)")
    plt.grid(True)
    plt.show()


def logistic_map_test():
    X: list[ull] = []
    Y: list[ull] = []

    for r in range(1, 11):
        x = ull(1)
        for _ in range(100):
            X.append(x)
            x = logistic_map(X[-1], byte(r))
            Y.append(x)
            x = x & logistic_constants[r].max_x
            X.append(x)
            Y.append(x)
        plt.xlim((0, logistic_constants[r].max_x))
        plt.ylim((0, 0xFFFFFFFFFFFFFFFF))
        X = np.array(X)
        Y = np.array(Y)

        plt.plot(X, Y, c="blue")
        plt.savefig(f"img/logistic/r={r}.png")
        X = []
        Y = []
        plt.cla()


if __name__ == "__main__":
    graficar_funcion(circle_map, 0, (2**64), 1000)
