# 1) start CytoDC1. Make sure Neuron Simulation selected from OutputOne
# 2) nrniv test1.py -  # note the trailing minus sign.
#        Also note on my cybercyte system the gui often crashes when
#        launching with nrniv -python test1.py. Perhaps due to gui thread.
# 3) Press RUN button in CytoDC1 window.
#    Note: Despite GUI appearance, the DC1 loop is waiting on a
#          semaphore so it can read voltage from NEURON.
# 4) In nrniv terminal window type: prun()
# 5) Press STOP button in CytoDC1 window.
#    Note: after prun, the DC1 loop is waiting on a semaphore.
#    It is likely necessary to sudo kill -1 CytoDC1 even after exiting CytoDC1 GUI.
# 6) In nrniv terminal window (to allow offline analysis of run timings) type: po.writeraw()
# 7) In each nrniv Graph, select View/View=plot

from neuron import h, gui

pc = h.ParallelContext()

s = h.Section(name="soma")
s.L = 3
s.diam = 10

tvec = h.Vector().record(h._ref_t, sec=s).resize(50000)
dtvec = h.Vector().record(h._ref_dt, sec=s).resize(50000)

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
    "dc1ReadCurrentBegin", # 12
    "dc1ReadCurrentEnd", # 13
    "dc1LoopBegin", # 14
    "dc1LoopEnd", # 15
    "msTime", # 16
]

nrnval_labels = [
    "dc1LoopIndex",  # 0 integer
    "nrnFixedStepEntrySimTime",  # ms
    "dc1CurrentIntoRHS",  # ??
    "nrnVoltageUpdateSimTime",  # ms
    "nrnVoltageUpdateValue",  # 4 mV
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
        pickle.dump((nrnclks, nrnvals), f)


def readraw():
    import pickle

    with open("rawtime.dat", "rb") as f:
        data = pickle.load(f)
    for i, v in enumerate(data[0]):
        v.label(nrnclk_labels[i])
    for i, v in enumerate(data[1]):
        v.label(nrnval_labels[i])
    return data


gs = [h.Graph() for i in range(2)]


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
    pc.set_maxstep(1000)
    h.finitialize(-65)
    pc.psolve(tstop)
    for g in gs:
        g.erase()
    dtvec.label("nrndt")
    dtvec.line(gs[1], 1, 1)
    deriv(nrnclks[0]).line(gs[1], 2, 1)
    # nrnclks[11] record takes place before assignment. So rotate toward 0 and
    # copy nrnclk[11] into last element
    nrnclks[11].rotate(-1).x[nrnclks[11].size() - 1] = h.nrnclk[11]

    normalize(nrnclks[1]).line(gs[0], 1, 1)
    normalize(nrnclks[0]).line(gs[0], 2, 1)


def sub(i, j):
    return nrnclks[i].c().sub(nrnclks[j]).mul(1e-6)


def pltnrnvecs():
    grphs = []
    for i in range(1, 10):
        g = h.Graph()
        x = sub(i, i - 1).line(g)
        g.label(0.4, 0.9, "nrnclk[%d] - nrnclk[%d]" % (i, i - 1))
        g.label(nrnclks_labels[i] + " - " + nrnclks_labels[i - 1])
        g.exec_menu("View = plot")
        grphs.append((g, x))
    return grphs


h(
    """
objref po, x
po = new PythonObject()
obfunc prun() { return po.run(1000) }
// x = prun()
// x.deriv(1, 1).line(Graph[2]) // after opening another Graph
"""
)
