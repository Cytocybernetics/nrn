#!/usr/bin/bash

set -e
source ${JENKINS_DIR:-.}/_env_setup.sh
module load intel

set -x
TEST_DIR="$1"
CORENRN_TYPE="$2"

cd $WORKSPACE/${TEST_DIR}
rm -rf ${CORENRN_TYPE}
mkdir -p ${CORENRN_TYPE}

pushd ${CORENRN_TYPE}

set +x
if [ "${TEST_DIR}" = "ringtest" ]; then
    echo "Running install_${CORENRN_TYPE}/bin/nrnivmodl-core ."
    $WORKSPACE/install_${CORENRN_TYPE}/bin/nrnivmodl-core .
else
    echo "Running install_${CORENRN_TYPE}/bin/nrnivmodl-core ../mod"
    $WORKSPACE/install_${CORENRN_TYPE}/bin/nrnivmodl-core ../mod
fi
set -x

# rpath $ORIGIN should make it relocatable
find * -mindepth 1 -maxdepth 1 -type f -exec mv "{}" ./ \;

popd
