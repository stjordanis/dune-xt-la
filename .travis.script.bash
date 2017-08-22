#!/bin/bash

# ****** THIS FILE IS AUTOGENERATED, DO NOT EDIT **********
# this file is treated as a jinja2 template

set -e
set -x

WAIT="${SUPERDIR}/scripts/bash/travis_wait_new.bash 45"

${SRC_DCTRL} ${BLD} --only=${MY_MODULE} configure
${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${BUILD_CMD}
if [ x"${TESTS}" == x ] ; then
    ${WAIT} ${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ninja -v test_binaries
else
    ${WAIT} ${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ninja -v test_binaries_builder_${TESTS}
fi
if [[ "${CC}" == "gcc"* ]] ; then
    lcov -q --gcov-tool ${GCOV} -b ${SUPERDIR}/${MY_MODULE} -d ${DUNE_BUILD_DIR}/${MY_MODULE} -c -o ${HOME}/baseline.lcov --no-external --initial
fi

source ${OPTS}
CTEST="ctest -V --timeout ${DXT_TEST_TIMEOUT:-300} -j ${DXT_TEST_PROCS:-2}"

if [ x"${TESTS}" == x ] ; then
    ${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${CTEST}
else
    ${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${CTEST} -L "^builder_${TESTS}$"
fi
${SUPERDIR}/.ci/init_sshkey.bash ${encrypted_862ca47045d1_key} ${encrypted_862ca47045d1_iv} keys/dune-community/dune-xt-la-testlogs
# retry this step becuase of the implicated race condition in cloning and pushing with multiple builder running in parallel
${SUPERDIR}/scripts/bash/travis_upload_test_logs.bash ${DUNE_BUILD_DIR}/${MY_MODULE}/dune/xt/*/test/

${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${BUILD_CMD} headercheck
${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${BUILD_CMD} install | grep -v "Installing"
${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${BUILD_CMD} package_source

# ****** THIS FILE IS AUTOGENERATED, DO NOT EDIT **********