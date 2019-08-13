#!/bin/sh
set -e

while [ "$1" != "" ]; do
    key="$1"

    case $key in
        --verbose)
        VERBOSE=1
        EXTRA_MAKE_FLAGS=VERBOSE=1
        shift # past argument
        ;;
        *)    # unknown option
        shift # past argument
        ;;
    esac
done

if [ -n "${VERBOSE}" ]; then
    set -x
fi

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

if [ -z "${REPO_ROOT}" ]; then
    REPO_ROOT=".."
fi

if [ -n "${EXTRA_CMAKE_FLAGS}" ]; then
    CMAKE_FLAGS=`echo ${EXTRA_CMAKE_FLAGS} | envsubst`
fi

CMAKE_FLAGS="$CMAKE_FLAGS -DCMAKE_C_COMPILER=${C_COMPILER} -DCMAKE_CXX_COMPILER=${CXX_COMPILER} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"

if [ -n "${COMPILER_FLAGS}" ]; then
    CMAKE_FLAGS="$CMAKE_FLAGS -DCMAKE_CXX_FLAGS=${COMPILER_FLAGS}"
fi

if [ -n "${VERBOSE}" ]; then
    env
fi

cmake ${CMAKE_FLAGS} ${REPO_ROOT}

make -j$(nproc) ${EXTRA_MAKE_FLAGS}
