# explore rawtime.dat without need for cybercyte specific code
from read_dc1raw import read_dc1raw

dc1clks = read_dc1raw()

nrnclk_labels = ["nrnContinueCurrentIsReady", "nrnPostVoltageIsReady", "waitIFull"]

nrnval_labels = [
    "nrnFixedStepEntrySimTime",  # ms
    "dc1CurrentIntoRHS",  # ??
    "nrnVoltageUpdateSimTime",  # ms
    "nrnVoltageUpdateValue",  # 4 mV
]

# rawtimes are Vector.record from fadvance.cpp from the following statements


def readraw():
    import pickle

    with open("rawtime.dat", "rb") as f:
        data = pickle.load(f)
    for i, v in enumerate(data[0]):
        v.label(nrnclk_labels[i])
    for i, v in enumerate(data[1]):
        v.label(nrnval_labels[i])
    return data


nrnclks, nrnvals = readraw()

# first record values (finitialize) are useless
for v in nrnclks:
    v.remove(0)
for v in nrnvals:
    v.remove(0)

# translate all time vectors relative to first value of nrnFixedStepEntry
torigin = nrnclks[0][0]
for v in nrnclks:
    v.sub(torigin)

for v in dc1clks:
    v.sub(torigin)


def a(i):
    # Print first few times starting at index i for each nrnclk vector
    # shifted by the ith time of nrnFixedStepEntry
    torigin = nrnclks[0][i]
    print("nrnclks  torigin=", torigin)
    for v in nrnclks:
        print(v.c().sub(torigin).to_python()[i : i + 5], v.label())

    print("nrnvals")
    for v in nrnvals:
        print(v.to_python()[i : i + 5], v.label())

    print("dc1clks")
    for v in dc1clks:
        print(v.c().sub(torigin).to_python()[i : i + 5], v.label())


"""
dc1 Iadc should always be before nrn Icontrib
nrn Vcalc should always be before dc1 Vdac

dc1 Iadc: dc1_adc_read_array dc1_labels[0]
nrn Icontrib: nrnContinueCurrentIsReady nrnclk_labels[0]
nrn Vcalc: nrnPostVoltageIsReady nrnclk_labels[1]
dc1 Vdac: dc1_write_voltage_array  dc1_labels[1]

except for size issues, Icontrib - Iadc, every element positive
Vdac - Vcalc, every element positive
"""
szv = nrnclks[0].size() - 1
assert nrnclks[0].c(0, szv).sub(dc1clks[0].c(0, szv)).indvwhere("<=", 0).size() == 0
assert dc1clks[2].c(0, szv).sub(nrnclks[1].c(0, szv)).indvwhere("<=", 0).size() == 0

from neuron import h, gui


last = szv - 1
z = [
    dc1clks[0].cl(0, last),  # beforeIadc
    dc1clks[1].cl(0, last),  # afterIadc
    nrnclks[2].cl(0, last),  # waitIFull
    nrnclks[0].cl(0, last),  # Icontrib
    nrnclks[1].cl(0, last),  # Vcalc
    dc1clks[2].cl(0, last),  # beforeVdac
    dc1clks[3].cl(0, last),  # afterVdac
]
nz = len(z)
orig = z[2][0]
for v in z:
    v.sub(orig)
z[0][0] = -10000
z[1][0] = -9000
for i, v in enumerate(z):
    print(i, v.c(0, 5).to_python(), v.label())
for i in range(nz - 1):
    i1 = (i + 1) % nz
    print(i, z[i].c().sub(z[i1]).indvwhere(">", 0).size())
print(nz, z[nz - 1].c(0, last - 1).sub(z[0].c(1, last)).indvwhere(">", 0).size())

graphs = []


def stepsize():
    g = h.Graph()
    graphs.append(g)
    g.label(0.1, 0.9)
    for i in range(1):
        z[0].c().deriv(1, 1).line(g)
    g.exec_menu("View = plot")


stepsize()


def event_pattern(begin=0, size=100):
    g = h.Graph()
    graphs.append(g)
    g.label(0.1, 0.9)
    origin = z[0][begin]
    for i, v in enumerate(z):
        x = v.c(begin, begin + size - 1)
        x.c().fill(i + 1).mark(g, x, "|")
        g.label("%d %s" % (i, v.label()))
    g.exec_menu("View = plot")


def tdiff():
    g = h.Graph()
    graphs.append(g)
    g.label(0.1, 0.9)
    for i in range(len(z) - 1):
        z[i + 1].c().sub(z[i]).line(g, 1, i + 1, 1)
        g.color(i + 1)
        g.label("%s - %s" % (z[i + 1].label(), z[i].label()))
    z[0].c(1, last).sub(z[nz - 1].c(0, last - 1)).line(g, 1, nz, 1)
    g.color(nz)
    g.label("%s  - %s" % (z[0].label(), z[nz - 1].label()))
    g.exec_menu("View = plot")


tdiff()


def pltv():
    g = h.Graph()
    graphs.append(g)
    g.label(0.1, 0.9, "v(.5)")
    nrnvals[3].c().line(g, nrnvals[2], 1, 1)
    g.exec_menu("View = plot")


pltv()
