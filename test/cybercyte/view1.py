# explore rawtime.dat without need for cybercyte specific code

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


nrnclks, nrnvals = readraw()

# first record values (finitialize) are useless
for v in nrnclks:
    v.remove(0)
for v in nrnvals:
    v.remove(0)

# translate all time vectors relative to first value of nrnFixedStepEntry
torigin = nrnclks[0][0]
for v in nrnclks:
    if "nrnVoltageUpdateSimTime" != v.label():
        v.sub(torigin)

# Here are the first few times
print("nrnclks")
for v in nrnclks:
    print(v[0], v[1], v[2], v[3], v[4], v.label())

print("nrnvals")
for v in nrnvals:
    print(v[0], v[1], v[2], v[3], v[4], v.label())
