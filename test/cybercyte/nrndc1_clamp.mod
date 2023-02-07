: Current Clamp for Synthetic Cell Mode (gc = 0, injected current ic)
: Voltage Clamp for Electronic Expression Mode (gc very large  (ic =0))
: Single instance defines location of the Clamp

: For Synthetic Cell Mode, DC1 reads external current which gets
: copied to ic. DC1 writes external voltage which comes from v.

: For Electronic Expression Mode, DC1 reads external voltage
: which gets copied to vc. DC1 writes external current which
: comes from i.

: HOC/Python used to set up the vc, ic, i, v connection to DC1.

: The "gc very large" for Electronic Expression Mode means that
: gc is set to a value large enough so that v tracks vc close
: enough to reasonably simulate a voltage clamp.

NEURON {
    POINT_PROCESS NrnDC1Clamp
    ELECTRODE_CURRENT i
    RANGE ic, vc, gc
}

UNITS {
    (nA) = (nanoamp)
    (mV) = (millivolt)
    (uS) = (microsiemens)
}

PARAMETER {
    ic = 0 (nA)
    vc = 0 (mV)
    gc = 0 (uS)
}

ASSIGNED {
    v (mV)
    i (nA)
}

BREAKPOINT {
    CONDUCTANCE gc
    i = gc*(vc - v) : gc = 0 means Synthetic Cell Mode
    i = i + ic
}
