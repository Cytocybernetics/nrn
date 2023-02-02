# read the dc1rawtime.dat file into Vectors
from neuron import h
import struct

dc1raw_labels = [
    "dc1_beforeIadc",
    "dc1_afterIadc",
    "dc1_beforeVdac",
    "dc1_afterVdac",
]


def read_dc1raw():
    with open("dc1rawtime.dat", "rb") as f:
        line = f.readline()
        [n, sz] = [int(i) for i in line.split()]
        contents = f.read()
    print(n, sz)
    vec = [h.Vector().resize(sz) for _ in range(n)]
    for i in range(n):
        vec[i].label(dc1raw_labels[i])
    ix = 0
    for i in range(n):
        for j in range(sz):
            val = contents[ix : ix + 8]
            x = struct.unpack("N", val)
            vec[i][j] = x[0]
            ix += 8

    return vec


if __name__ == "__main__":
    vec = read_dc1raw()

    torigin = vec[0][1]
    for v in vec:
        v.sub(torigin)

    j = 1
    for i in range(len(vec)):
        print(j, [int(x) for x in vec[i].c(j, j + 4)], vec[i].label())
