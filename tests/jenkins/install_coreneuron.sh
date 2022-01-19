#!/usr/bin/bash

set -e
set -x

source ${JENKINS_DIR:-.}/_env_setup.sh

reportinglib_dir=$(spack install reportinglib%intel | cut -d' '  -f2)
libsonata_report_dir=$(spack install libsonata-report%intel | cut -d' ' -f2)

CORENRN_TYPE="$1"

if [ "${CORENRN_TYPE}" = "GPU-non-unified" ] || [ "${CORENRN_TYPE}" = "GPU-unified" ]; then
    module load gcc nvhpc cuda hpe-mpi cmake boost
    mkdir build_${CORENRN_TYPE}
else
    module load boost intel-oneapi-compilers hpe-mpi cmake
    export CC=mpicc
    export CXX=mpicxx

    mkdir build_${CORENRN_TYPE} build_intel_${CORENRN_TYPE}
fi

export SALLOC_PARTITION="prod";

cd $WORKSPACE/build_${CORENRN_TYPE}

echo "${CORENRN_TYPE} build"
if [ "${CORENRN_TYPE}" = "GPU-non-unified" ]; then
    cmake \
        -DCORENRN_ENABLE_GPU=ON \
        -DCMAKE_CUDA_COMPILER=nvcc \
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
        -DCMAKE_CUDA_COMPILER=nvcc \
        -DCORENRN_ENABLE_CUDA_UNIFIED_MEMORY=ON \
        -DCMAKE_INSTALL_PREFIX=$WORKSPACE/install_${CORENRN_TYPE}/ \
        -DTEST_MPI_EXEC_BIN="srun;--account=proj16;--partition=$SALLOC_PARTITION;--constraint=volta;--gres=gpu:2;--mem;0;-t;00:05:00" \
        -DTEST_EXEC_PREFIX="srun;-n;6;--account=proj16;--partition=$SALLOC_PARTITION;--constraint=volta;--gres=gpu:2;--mem;0;-t;00:05:00" \
        -DAUTO_TEST_WITH_SLURM=OFF \
        -DAUTO_TEST_WITH_MPIEXEC=OFF \
        $WORKSPACE/
elif [ "${CORENRN_TYPE}" = "non-gpu" ]; then
    ENABLE_OPENMP=ON
    cmake  \
      -G 'Unix Makefiles'  \
      -DCMAKE_INSTALL_PREFIX=$WORKSPACE/install_${CORENRN_TYPE}/ \
      -DCMAKE_BUILD_TYPE=Debug  \
      -DCORENRN_ENABLE_OPENMP=$ENABLE_OPENMP \
      -DCORENRN_ENABLE_REPORTING=ON \
      -DCMAKE_PREFIX_PATH="$reportinglib_dir;$libsonata_report_dir" \
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
