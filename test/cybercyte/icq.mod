COMMENT
Since this is an electrode current, positive values of i depolarize the cell
and in the presence of the extracellular mechanism there will be a change
in vext since i is not a transmembrane current but a current injected
directly to the inside of the cell.
ENDCOMMENT

: Modified nrn/src/nrnoc/stim.mod so that if discontinuities are not on
: fixed time step boundaries, the total charge delivered will still be
: the same.

NEURON {
    POINT_PROCESS IClampQ
    RANGE del, dur, amp, i, invl
    ELECTRODE_CURRENT i
}
UNITS {
    (nA) = (nanoamp)
}

PARAMETER {
    del (ms)
    dur (ms)    <0,1e9>
    amp (nA)
    invl = 20 (ms)
}

ASSIGNED {
    i (nA)
    dt (ms)
    strt (ms)
    stp (ms)
}

INITIAL {
    i = 0
    strt = del
    stp = del + dur
}

BEFORE BREAKPOINT {
    : fixed step t is at tentry + dt/2
    LOCAL tm, tp, frac
  if (!cvode_active_) {
    tm = t - 0.5*dt
    tp = t + 0.5*dt
    if (tp < strt) { : before pulse
        i = 0
    } else if (tm > stp) { : after pulse
        i = 0
    } else { : in pulse
        i = amp : but will be modified if pulse does not span step
        if (tm <= del) { : pulse turns on
            : subtract portion from tm to del
            frac = (del - tm)/dt
            i = i - amp*frac
        }
        if (tp >= stp) { : pulse turns off
            : subtract portion from stp to tp
            frac = (tp - stp)/dt
            i = i - amp*frac
            strt = strt + invl
            stp = stp + invl
        }
    }
  }
}

BREAKPOINT {
    at_time(strt)
    at_time(stp)
    if (cvode_active_) {
        if ( t >= strt && t < stp) {
            i = amp
        }else{
            if (t > stp) {
                strt = strt + invl
                stp = stp + invl
            }
            i = 0
        }
    } else {
        i = i + 0 : computed in BEFORE BREAKPOINT
    }
}
