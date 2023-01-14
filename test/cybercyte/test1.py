# 1) start CytoDC1. Make sure Neuron Simulation selected from OutputOne
# 2) nrniv test1.py -  # note the trailing minus sign.
#        Also note on my cybercyte system the gui often crashes when
#        launching with nrniv -python test1.py. Perhaps due to gui thread.
# 3) Press RUN button in CytoDC1 window.
# 4) In nrniv terminal window type: prun()
# 5) Press STOP button in CytoDC1 window.
# 6) In each nrniv Graph, select View/View=plot

from neuron import h, gui

pc = h.ParallelContext()

s = h.Section(name="soma")
s.L = 3
s.diam = 10

dc1_tvec = h.Vector().record(h._ref_dc1_time, sec=s).resize(50000)
tvec = h.Vector().record(h._ref_t, sec=s).resize(50000)
dtvec = h.Vector().record(h._ref_dt, sec=s).resize(50000)
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
nrnvecs = [
    h.Vector().record(h._ref_nrnclk[i], sec=s).resize(50000).resize(0)
    for i in range(len(nrnvec_labels))
]
for i, v in enumerate(nrnvecs):
    v.label(nrnvec_labels[i])


def writeraw():
    import pickle

    with open("rawtime.dat", "wb") as f:
        pickle.dump(nrnvecs, f)


def readraw():
    import pickle

    with open("rawtime.dat", "rb") as f:
        data = pickle.load(f)
    for i, v in enumerate(data):
        v.label(nrnvec_labels[i])
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
    deriv(nrnvecs[0]).line(gs[1], 2, 1)
    # nrnvecs[6] record takes place before assignment. So rotate toward 0 and
    # copy nrnclk[6] into last element
    # nrnvecs[6].rotate(-1).x[nrnvecs[6].size() -1] = h.nrnclk[6]

    normalize(dc1_tvec).line(gs[0], 1, 1)
    normalize(nrnvecs[0]).line(gs[0], 2, 1)


def sub(i, j):
    return nrnvecs[i].c().sub(nrnvecs[j]).mul(1e-6)


def pltnrnvecs():
    grphs = []
    for i in range(1, 10):
        g = h.Graph()
        x = sub(i, i - 1).line(g)
        g.label(0.4, 0.9, "nrnclk[%d] - nrnclk[%d]" % (i, i - 1))
        g.label(nrnvec_labels[i] + " - " + nrnvec_labels[i - 1])
        g.exec_menu("View = plot")
        grphs.append((g, x))
    return grphs


h(
    """
objref po, x
po = new PythonObject()
obfunc prun() { return po.run(1000) }
x = po.dc1_tvec
// x = prun()
// x.deriv(1, 1).line(Graph[2]) // after opening another Graph
"""
)
