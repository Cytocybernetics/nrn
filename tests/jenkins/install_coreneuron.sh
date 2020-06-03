#!/usr/bin/bash

set -e
set -x

source ${JENKINS_DIR:-.}/_env_setup.sh

reportinglib_dir=$(spack cd -i reportinglib%intel && pwd)

CORENRN_TYPE="$1"

if [ "${CORENRN_TYPE}" = "GPU-non-unified" ] || [ "${CORENRN_TYPE}" = "GPU-unified" ]; then
    module load pgi/19.10 cuda hpe-mpi cmake boost

    mkdir build_${CORENRN_TYPE}
else
    module load boost intel hpe-mpi cmake
    export CC=mpicc
    export CXX=mpicxx

    mkdir build_${CORENRN_TYPE} build_intel_${CORENRN_TYPE}
fi

# default partition is interactive. during night use production
hour=`date +%H`
weekday=`date +%u`
if [ "$hour" -ge "19" ] || [ "$hour" -lt "8" ] || [ $weekday -gt 5 ]; then
  export SALLOC_PARTITION="prod";
else
  export SALLOC_PARTITION="interactive"
fi

cd $WORKSPACE/build_${CORENRN_TYPE}

echo "${CORENRN_TYPE} build"
if [ "${CORENRN_TYPE}" = "GPU-non-unified" ]; then
    cmake \
        -DCORENRN_ENABLE_GPU=ON \
        -DCORENRN_ENABLE_CUDA_UNIFIED_MEMORY=OFF \
        -DCMAKE_INSTALL_PREFIX=$WORKSPACE/install_${CORENRN_TYPE}/ \
        -DTEST_MPI_EXEC_BIN="srun;--account=proj16;--partition=$SALLOC_PARTITION;--constraint=volta;--gres=gpu:2;--mem;0;-t;00:05:00" \
        -DTEST_EXEC_PREFIX="srun;-n;6;--account=proj16;--partition=$SALLOC_PARTITION;--constraint=volta;--gres=gpu:2;--mem;0;-t;00:05:00" \
        -DAUTO_TEST_WITH_SLURM=OFF \
        -DAUTO_TEST_WITH_MPIEXEC=OFF \
        $WORKSPACE/
elif [ "${CORENRN_TYPE}" = "GPU-unified" ]; then
    cmake \
        -DCORENRN_ENABLE_GPU=ON \
        -DCORENRN_ENABLE_CUDA_UNIFIED_MEMORY=ON \
        -DCMAKE_INSTALL_PREFIX=$WORKSPACE/install_${CORENRN_TYPE}/ \
        -DTEST_MPI_EXEC_BIN="srun;--account=proj16;--partition=$SALLOC_PARTITION;--constraint=volta;--gres=gpu:2;--mem;0;-t;00:05:00" \
        -DTEST_EXEC_PREFIX="srun;-n;6;--account=proj16;--partition=$SALLOC_PARTITION;--constraint=volta;--gres=gpu:2;--mem;0;-t;00:05:00" \
        -DAUTO_TEST_WITH_SLURM=OFF \
        -DAUTO_TEST_WITH_MPIEXEC=OFF \
        $WORKSPACE/
elif [ "${CORENRN_TYPE}" = "AoS" ] || [ "${CORENRN_TYPE}" = "SoA" ]; then
    CORENRN_ENABLE_SOA=ON
    ENABLE_OPENMP=ON
    if [ "${CORENRN_TYPE}" = "AoS" ]; then
        CORENRN_ENABLE_SOA=OFF
        ENABLE_OPENMP=OFF
    fi
    cmake  \
      -G 'Unix Makefiles'  \
      -DCMAKE_INSTALL_PREFIX=$WORKSPACE/install_${CORENRN_TYPE}/ \
      -DCMAKE_BUILD_TYPE=Debug  \
      -DCORENRN_ENABLE_SOA=$CORENRN_ENABLE_SOA \
      -DCORENRN_ENABLE_OPENMP=$ENABLE_OPENMP \
      -DCORENRN_ENABLE_BIN_REPORTS=ON \
      -DCMAKE_PREFIX_PATH=$reportinglib_dir \
      -DTEST_MPI_EXEC_BIN="mpirun" \
      -DTEST_EXEC_PREFIX="mpirun;-n;2" \
      -DAUTO_TEST_WITH_SLURM=OFF \
      -DAUTO_TEST_WITH_MPIEXEC=OFF \
      $WORKSPACE/
else
    echo "Not a valid CORENRN_TYPE"
    exit 1
fi

make VERBOSE=1 -j8
make install
