# explore rawtime.dat without need for cybercyte specific code

nrnvec_labels = [
    "nrnFixedStepEntry",
    "nrnPostVoltageIsReady",
    "nrnWaitForCurrentIsReady",
    "nrnContinueCurrentIsReady",
    "nrnVoltageUpdate",
    "nrnVoltageUpdateSimTime",
    "nrnFixedStepLeave",
    "dc1WriteVoltageBegin",
    "dc1WriteVoltageEnd",
    "dc1ReadCurrent",
]

# rawtimes are Vector.record from fadvance.cpp from the following statements


def readraw():
    import pickle

    with open("rawtime.dat", "rb") as f:
        data = pickle.load(f)
    for i, v in enumerate(data):
        v.label(nrnvec_labels[i])
    return data


nrnvecs = readraw()

# first record values (finitialize) are useless
for v in nrnvecs:
    v.remove(0)

# nrnFixedStepLeave is after the record so rotate toward 0 and (for now)
# copy the last value into into last place.
sz = nrnvecs[6].size()
last = nrnvecs[6][sz - 1]
nrnvecs[6].rotate(-1).x[sz - 1] = last

# translate all time vectors relative to first value of nrnFixedStepEntry
torigin = nrnvecs[0][0]
for v in nrnvecs:
    if "nrnVoltageUpdateSimTime" != v.label():
        v.sub(torigin)

# Here are the first few times
for v in nrnvecs:
    print(v[0], v[1], v[2], v[3], v.label())
