

#include <nrnmpi.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include "neuron.h"
#include "section.h"
#include "multisplit.h"
#define nrnoc_fadvance_c
#include "sync.h"

#

double nrnclk[20];
double nrnval[20];

struct timespec start_ts, end_ts;
double timeWindow = 0.0;
long long int timeValues[5];
long long unsigned int deltaTimeArray;
double msTime = 0.0;

// double overall_time = 0;
// int num_iterations = 0;

static const size_t floatmask{0Xfffffffffffff};  // 52 bits
static size_t realtime() {
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    const size_t billion{1000000000};
    return ((billion * ts.tv_sec) + ts.tv_nsec) & floatmask;
}

static int loop_index{0};
static int nrndc1_step();
static double* dc1_read;
static double* dc1_write;

/**
 * @brief A function that can be passed to cyto_barrier_wait() to break out of waiting loop
 * 
 * @param barrier_index The barrier index that is invoking the callback function
 * @param neuron_shared The shared memory segment that among other things contain the kill_models flag used to abort the wait
 */
static void cyto_barrier_break_check(cyto_barrier_id barrier_id, void* data)
{
  neuron_shared_data* neuron_shared = (neuron_shared_data*)data;
  if (neuron_shared->dc1_request_end_nrn_loop) {
    cyto_barrier_break(barrier_id, CYTO_BARRIER_REASON_BREAK);
  }
}

void nrndc1_run() {  // run til a barrier times out

    NrnThread* nth = nrn_threads;
    const char* modename = hoc_gargstr(1);
    if (strcmp(modename, "SyntheticCellMode") == 0) {
        assert(nth->neuron_shared->Synthetic_Cell_Mode_ch1);
    }
    if (strcmp(modename, "ElectronicExpressionMode") == 0) {
        assert(nth->neuron_shared->Electronic_Expression_Mode_ch1);
    }
    dc1_read = hoc_pgetarg(2);
    dc1_write = hoc_pgetarg(3);

    // assert NRN finitialize has been called
    //    or there have been previous calls to nrndc1_run.
    printf("sizeof neuron_shared %zd\n", sizeof(neuron_shared_data));
    // assert DC1 waiting at CytoBarrierStart
    if (nth->neuron_shared->Synthetic_Cell_Mode_ch1) {
        // Voltage output from NrnDC1Clamp.get_segment().v
        // tell DC1 the (starting) voltage for the DAC
        nth->neuron_shared->V_mem_ch1 = *dc1_write;
    } else if (nth->neuron_shared->Electronic_Expression_Mode_ch1) {
        // Current output from NrnDC1Clamp.i (initialized to 0)
        nth->neuron_shared->I_mem_ch1 = *dc1_write;
    }
    loop_index = 0;

    printf("Neuron reached CytoBarrierStart at sim t=%g  Output = %g\n", nth->_t, *dc1_write);
    if (cyto_barrier_wait(CytoBarrierStart, cyto_barrier_break_check, nth->neuron_shared) != CYTO_BARRIER_REASON_OK) {
        printf("NEURON leaving nrndc1_run() due to aborted cyto_barrier_wait()\n");
        return;
    }
    int finish = 0;
    while (finish == 0) {
        finish = nrndc1_step();
    }
    nrnclk[19] = nth->neuron_shared->dc1_rtOrigin;
    printf("NEURON leaves nrndc1_run at sim t=%g loop_index=%d\n", nth->_t, loop_index);
    hoc_retpushx(1.0);
}

static int nrndc1_step() {
    NrnThread* nth = nrn_threads;

    if (nth->neuron_shared->dc1_request_end_nrn_loop) {
        printf("DC1 requested NEURON to exit processing loop\n");
        return 1;
    }

    deliver_net_events(nth);
    nrn_random_play();

    if (cyto_barrier_wait(CytoBarrierBeginNeuron, cyto_barrier_break_check, nth->neuron_shared) != CYTO_BARRIER_REASON_OK) {
        printf("NEURON nrndc1_step() exiting due to breaking out of cyto_barrier_wait()\n");
        return 1;
    }

    nrnclk[2] = realtime();  // after waitIFull
    double dtSoFar = (nrnclk[2] - nth->neuron_shared->dc1_rtOrigin) * 1e-6 - nth->_t;

#if 0
    if (loop_index != nth->neuron_shared->dc1_loop_index) {
        printf("nrn_loop_index=%d  dc1_loop_index=%d\n", loop_index, nth->neuron_shared->dc1_loop_index);
        abort();
    }
#endif
    loop_index++;

    nrnval[0] = nth->_t;  // nrnFixedStepEntrySimTime
    nth->_dt = nth->neuron_shared->msTime;
    nth->_dt = dtSoFar;
    dt = nth->_dt;
    nrnval[4] = dt;
    nth->_t += .5 * nth->_dt;

    nrnclk[0] = realtime();  // nrnContinueCurrentIsReady
    if (nth->neuron_shared->Synthetic_Cell_Mode_ch1) {
        // Current input to NrnDC1Clamp.ic
        *dc1_read = (nth->neuron_shared->I_mem_ch1 * nth->neuron_shared->chan1_nrn_mag *
                         nth->neuron_shared->invertType_ch1);
    } else if (nth->neuron_shared->Electronic_Expression_Mode_ch1) {
        // Voltage input to NrnDC1Clamp.vc
        *dc1_read = nth->neuron_shared->V_mem_ch1;
    }
    nrnval[1] = *dc1_read;  // dc1CurrentIntoRHS

    fixed_play_continuous(nth);
    setup_tree_matrix(nth);
    nrn_solve(nth);
    second_order_cur(nth);
    nrn_update_voltage(nth);  // voltage is at nth->_t + 0.5*nth->_dt

    // Synthetic Cell Mode
    if (nth->neuron_shared->Synthetic_Cell_Mode_ch1) {
        // Voltage output from NrnDC1Clamp.get_segment().v
        nth->neuron_shared->V_mem_ch1 = *dc1_write;
    } else if (nth->neuron_shared->Electronic_Expression_Mode_ch1) {
        // Current output from NrnDC1Clamp.i
        nth->neuron_shared->I_mem_ch1 = *dc1_write;  // Scaling?
    }
    nrnclk[1] = realtime();  // nrnPostVoltageIsReady
    nrnval[2] = nth->_t + .5 * nth->_dt;  // nrnVoltageUpdateSimTime
    nrnval[3] = *dc1_write;   // nrnVoltageUpdateValue

    assert(!nrnthread_v_transfer_);
    nrn_fixed_step_lastpart(nth);  // vector.record here

    if (cyto_barrier_wait(CytoBarrierEndNeuron, cyto_barrier_break_check, nth->neuron_shared) != CYTO_BARRIER_REASON_OK) {
        printf("NEURON nrndc1_step() exiting due to breaking out of cyto_barrier_wait()\n");
        return 1;
    }

    return 0;
}

