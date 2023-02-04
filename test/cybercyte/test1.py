# 1) launch ./run.sh from terminal.
# 2) Press RUN button in CytoDC1 window
# 3) See results with python3.7 view1.py

# Below is old way
# 1) start CytoDC1. Make sure Neuron Simulation selected from OutputOne
# 2) nrniv test1.py -  # note the trailing minus sign.
#        Also note on my cybercyte system the gui often crashes when
#        launching with nrniv -python test1.py. Perhaps due to gui thread.
# 3) Press RUN button in CytoDC1 window.
#    Note: Despite GUI appearance, the DC1 loop is waiting on a
#          semaphore so it can read voltage from NEURON.
# 4) In nrniv terminal window type: prun()
#     When neuron finishes, writeraw() will write the recorded vectors to rawtime.dat
#     and h.fill_dc1_array() will have dc1 write its stored arrays to dc1rawtime.dat
# 5) Press STOP button in CytoDC1 window.
#    Note: after prun, the DC1 loop is waiting on a semaphore.
#    It is likely necessary to sudo kill -1 CytoDC1 even after exiting CytoDC1 GUI.
#
# 6) Exit neuron or ...
# 7) In each nrniv Graph, select View/View=plot

from neuron import h

pc = h.ParallelContext()

s = h.Section(name="soma")
s.L = 3
s.diam = 10
s.insert("hh")
ic = h.IClampQ(s(0.5))
ic.delay = 1
ic.dur = 0.1
ic.amp = 0.3

tvec = h.Vector().record(h._ref_t, sec=s).resize(50000)
dtvec = h.Vector().record(h._ref_dt, sec=s).resize(50000)

nrnclk_labels = [
    "nrnContinueCurrentIsReady",
    "nrnPostVoltageIsReady",
    "waitIFull",
]

nrnval_labels = [
    "nrnFixedStepEntrySimTime",  # ms
    "dc1CurrentIntoRHS",  # ??
    "nrnVoltageUpdateSimTime",  # ms
    "nrnVoltageUpdateValue",  # mV
    "dt",  # ms
]

nrnclks = [
    h.Vector().record(h._ref_nrnclk[i], sec=s).resize(50000).resize(0)
    for i in range(len(nrnclk_labels))
]
for i, v in enumerate(nrnclks):
    v.label(nrnclk_labels[i])

nrnvals = [
    h.Vector().record(h._ref_nrnval[i], sec=s).resize(50000).resize(0)
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
    h.usetable_hh = 0
    pc.set_maxstep(1000)
    h.finitialize(-65)
    pc.psolve(tstop)
    h.fill_dc1_array()
    writeraw()


def sub(i, j):
    return nrnclks[i].c().sub(nrnclks[j]).mul(1e-6)


run(10)
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
