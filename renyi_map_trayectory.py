import os
import struct
from dataclasses import dataclass
from typing import List

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np

ull = np.uint64
byte = np.uint8


@dataclass
class LogisticConstants:
    max_x: ull
    t: ull
    r: ull


logistic_constants = {
    1: LogisticConstants(max_x=ull(2**33 - 1), t=ull(2**33), r=ull(1)),
    2: LogisticConstants(max_x=ull(2**32 - 1), t=ull(2**32), r=ull(2)),
    3: LogisticConstants(max_x=ull(2**32 - 1), t=ull(2**32), r=ull(3)),
    4: LogisticConstants(max_x=ull(2**32 - 1), t=ull(2**32), r=ull(4)),
    5: LogisticConstants(max_x=ull(2**31 - 1), t=ull(2**31), r=ull(5)),
    6: LogisticConstants(max_x=ull(2**31 - 1), t=ull(2**31), r=ull(6)),
    7: LogisticConstants(max_x=ull(2**31 - 1), t=ull(2**31), r=ull(7)),
    8: LogisticConstants(max_x=ull(2**31 - 1), t=ull(2**31), r=ull(8)),
    9: LogisticConstants(max_x=ull(2**31 - 1), t=ull(2**31), r=ull(9)),
    10: LogisticConstants(max_x=ull(2**31 - 1), t=ull(2**31), r=ull(10)),
}


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


def logistic_map(x: ull, r: byte):
    return r * x * (logistic_constants[r].t - x)


def logistic_map_test():
    X: list[ull] = []
    Y: list[ull] = []

    for r in range(1, 11):
        x = ull(1)
        identity = [
            [ull(0), ull(0)],
            [ull(1), ull(1)],
        ]
        for _ in range(255):
            X.append(x)
            x = logistic_map(X[-1], byte(r))
            Y.append(x)
            x = x & logistic_constants[r].max_x
            X.append(x)
            Y.append(x)
        # plt.xlim(logistic_constants[r].max_x)
        # plt.ylim(logistic_constants[r].t)
        X = np.array(X)
        Y = np.array(Y)

        plt.plot(X, Y)
        plt.savefig(f"img/logistic/r={r}.png")
        X = []
        Y = []
        plt.cla()


if __name__ == "__main__":
    logistic_map_test()
