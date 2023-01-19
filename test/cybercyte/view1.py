# explore rawtime.dat without need for cybercyte specific code
from read_dc1raw import read_dc1raw

dc1clks = read_dc1raw()

nrnclk_labels = [
    "nrnFixedStepEntry",  # 0
    "dc1BeginLoop",
    "nrnPostVoltageIsReady",
    "nrnWaitForCurrentIsReady",
    "nrnContinueCurrentIsReady",
    "dc1ReadCurrent",
    "dc1WriteVoltageBegin",
    "dc1WriteVoltageEnd",
    "dc1WaitForVoltageIsReady",
    "dc1ContinueVoltageIsReady",
    "nrnVoltageUpdate",
    "nrnFixedStepLeave",  # 11
]

nrnval_labels = [
    "dc1LoopIndex",  # 0 integer
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


nrnclks, nrnvals, foo = readraw()

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


def noncontiguous_dc1_indices():
    print("dc1LoopIndex problems (adjacent difference != 1)")
    ix = nrnvals[0].c().deriv(1, 1).indvwhere("!=", 1)
    ix = [int(i) for i in ix]
    print("context")
    dc1_indices = [int(i) for i in nrnvals[0]]
    iold = -100
    for i in ix:
        if i > iold + 2:
            print(i, dc1_indices[i : i + 7])
        iold = i


noncontiguous_dc1_indices()


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
a(0)
a(339)
a(17261)
"""
