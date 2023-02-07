# 1) launch ./run.sh from terminal.
# 2) Press RUN button in CytoDC1 window
# 3) After some sim time Press STOP button in CytoDC1 window
# 4) Exit from CytoDC1
# 5) See results with python3.7 view1.py

from neuron import h

pc = h.ParallelContext()

# The NEURON model to be simulated (e.g. hh patch)
s = h.Section(name="soma")
s.L = 3
s.diam = 10
s.insert("hh")
s.gkbar_hh = 0.036
ic = h.IClampQ(s(0.5))
ic.delay = 1
ic.dur = 0.1
ic.amp = 0.3

# DC1 access location
# pp = h.NrnDC1Clamp(segment)
# Synthetic Cell Mode (must be consistent with DC1)
# DC1 reads external current and that gets copied to pp.ic
# DC1 writes external voltage from v (at the location of pp)

# For Electronic Expression Mode, pp.gc should be set to a large value
# so that segment.v tracks pp.vc very closely.
# DC1 reads external voltage and that gets copied to pp.vc
# DC1 writes external current from pp.i
# It likely makes sense to set segment.cm of this location to 0. For
# a single compartment, that means total ionic current (without
# capacitance current (but with stimulus current (which should
# not exist))).  For a multicompartment (whole cell) model, that
# means total ionic current plus net axial current into the
# compartment.


class DC1Access:
    def __init__(self, seg, mode="SyntheticCellMode", rest=-65.0):
        self.clamp = h.NrnDC1Clamp(seg)
        if mode == "SyntheticCellMode":
            self.SyntheticCellMode()
        elif mode == "ElectronicExpressionMode":
            self.ElectronicExpressionMode(rest)
        else:
            raise ValueError("Invalid mode name")

    def relocate(self, seg):
        self.clamp.loc(seg)

    def SyntheticCellMode(self):
        self.mode = "SyntheticCellMode"
        c = self.clamp
        c.gc = 0
        self.dc1read = c._ref_ic
        self.dc1write = c.get_segment()._ref_v

    def ElectronicExpressionMode(self, rest=-65.0):
        self.mode = "ElectronicExpressionMode"
        c = self.clamp
        c.gc = 10  # (uS) (10uS equivalent to 0.1 MOhm)
        c.vc = rest
        c.ic = 0
        self.dc1read = c._ref_vc
        self.dc1write = c._ref_i


dc1access = DC1Access(s(0.5))

ARRAYSIZE = 1000000

nrnclk_labels = [
    "nrnContinueCurrentIsReady",
    "nrnPostVoltageIsReady",
    "waitIFull",
    #    "msTime",
]

nrnval_labels = [
    "nrnFixedStepEntrySimTime",  # ms
    "dc1CurrentIntoRHS",  # ??
    "nrnVoltageUpdateSimTime",  # ms
    "nrnVoltageUpdateValue",  # mV
    "dt",  # ms
]

nrnclks = [
    h.Vector().record(h._ref_nrnclk[i], sec=s).resize(ARRAYSIZE).resize(0)
    for i in range(len(nrnclk_labels))
]
for i, v in enumerate(nrnclks):
    v.label(nrnclk_labels[i])

nrnvals = [
    h.Vector().record(h._ref_nrnval[i], sec=s).resize(ARRAYSIZE).resize(0)
    for i in range(len(nrnval_labels))
]
for i, v in enumerate(nrnvals):
    v.label(nrnval_labels[i])


def writeraw():
    import pickle

    with open("rawtime.dat", "wb") as f:
        pickle.dump((nrnclks, nrnvals, h.nrnclk[19]), f)  # last is dc1_rtOrigin


def readraw():
    import pickle

    with open("rawtime.dat", "rb") as f:
        data = pickle.load(f)
    for i, v in enumerate(data[0]):
        v.label(nrnclk_labels[i])
    for i, v in enumerate(data[1]):
        v.label(nrnval_labels[i])
    return data


def normalize(v):
    x = v.c()
    x.x[0] = x.x[1]
    x.sub(x.x[0]).mul(1e-6)
    return x


def deriv(v):
    x = v.c()
    x.x[0] = x.x[1]
    x.sub(x.x[0])
    x.deriv(1, 1)
    x.append(x.x[x.size() - 1])
    return x.mul(1e-6)


def run(tstop):
    try:
        # may or may not exist depending on cmake configuration
        h.usetable_hh = 0  # with variable dt, do not recompute tables
    except:
        pass
    pc.set_maxstep(1000)
    h.finitialize(-65)
    h.nrndc1_run(dc1access.mode, dc1access.dc1read, dc1access.dc1write)
    writeraw()


def sub(i, j):
    return nrnclks[i].c().sub(nrnclks[j]).mul(1e-6)


run(20)
import time

time.sleep(2)
quit()

h(
    """
objref po, x
po = new PythonObject()
obfunc prun() { return po.run(1000) }
// x = prun()
// x.deriv(1, 1).line(Graph[2]) // after opening another Graph
"""
)
