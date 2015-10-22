#!/bin/bash -x

usage_exit() {
    echo "Usage: $0 [--without-python2] [--with-python3] [--with-no-python] [--with-no-test] [--with-vtk] [--with-hdf5] [--prefix=PREFIX] [-h] [--help] [py2|py3]" 1>&2
    exit 1
}

WITH_PYTHON2=1
WITH_PYTHON3=0
WITH_TEST=1
VTK_INCLUDE_PATH=/usr/include/vtk-5.8
WITH_VTK=0
WITH_HDF5=0

if [ "${PREFIX-UNDEF}" = "UNDEF" ]; then
    if [ "$PREFIX" = "" ]; then
        PREFIX=/usr/local
        # PREFIX=${HOME}/local
        # CURDIR=$(cd $(dirname $0); pwd)
        # PREFIX=${CURDIR}/local
        # PREFIX=
    fi
fi

while getopts :h-: OPT
do
    case $OPT in
        -)
            case "$OPTARG" in
                without-python2) WITH_PYTHON2=0;;
                with-python3) WITH_PYTHON3=1;;
                with-no-python)
                    WITH_PYTHON2=0
                    WITH_PYTHON3=0
                    ;;
                with-no-test) WITH_TEST=0;;
                with-vtk) WITH_VTK=1;;
                with-hdf5) WITH_HDF5=1;;
                prefix=*) PREFIX=${OPTARG#*=};;
                help) usage_exit;;
                *) usage_exit;;
            esac;;
        h) usage_exit;;
        \?) usage_exit;;
    esac
done

shift $((OPTIND - 1))

PREFIX=$(cd $(dirname $PREFIX) && pwd)/$(basename $PREFIX)

PYTHON_MAJOR_VERSION=$1
if [ "$PYTHON_MAJOR_VERSION" = "py2" ]; then
    WITH_PYTHON2=1
    WITH_PYTHON3=0
elif [ "$PYTHON_MAJOR_VERSION" = "py3" ]; then
    WITH_PYTHON2=0
    WITH_PYTHON3=1
else
    echo "No Python major version was specified: WITH_PYTHON2=${WITH_PYTHON2} WITH_PYTHON3=${WITH_PYTHON3}"
fi

# make clean; rm CMakeCache.txt
# rm -rf ${PREFIX}/lib/libecell4-*.so ${PREFIX}/include/ecell4;

set -e

cmake -DCMAKE_INSTALL_PREFIX=${PREFIX} -DWITH_HDF5=${WITH_HDF5} -DWITH_VTK=${WITH_VTK} .
make
# cmake -DCMAKE_INSTALL_PREFIX=${PREFIX} -DECELL4_ENABLE_PROFILING=1 .
# make VERBOSE=1
if [ $WITH_TEST != 0 ]; then
    make test
fi
make install

cd python

if [ $WITH_PYTHON2 != 0 ]; then
    # rm -rf build lib/ecell4/*.cpp
    mkdir -p ${PREFIX}/lib/python2.7/site-packages
    if [ "$(uname)" == "Darwin" ]; then
        LD_LIBRARY_PATH=${PREFIX}/lib PYTHONPATH=${PREFIX}/lib/python2.7/site-packages:/usr/local/lib/python2.7/dist-packages:${PYTHONPATH} python setup.py build_ext -L${PREFIX}/lib -I${PREFIX}/include:${VTK_INCLUDE_PATH} install --prefix=${PREFIX}
        PYTHONPATH=${PREFIX}/lib/python2.7/site-packages:/usr/local/lib/python2.7/dist-packages:${PYTHONPATH} LD_LIBRARY_PATH=${PREFIX}/lib python setup.py test
    else
        LD_LIBRARY_PATH=${PREFIX}/lib PYTHONPATH=${PREFIX}/lib/python2.7/site-packages:/usr/local/lib/python2.7/dist-packages:${PYTHONPATH} python2 setup.py build_ext -L${PREFIX}/lib -I${PREFIX}/include:${VTK_INCLUDE_PATH} install --prefix=${PREFIX}
        PYTHONPATH=${PREFIX}/lib/python2.7/site-packages:/usr/local/lib/python2.7/dist-packages:${PYTHONPATH} LD_LIBRARY_PATH=${PREFIX}/lib python2 setup.py test
    fi
fi

if [ $WITH_PYTHON3 != 0 ]; then
    # rm -rf build lib/ecell4/*.cpp
    mkdir -p ${PREFIX}/lib/python3.4/site-packages
    LD_LIBRARY_PATH=${PREFIX}/lib PYTHONPATH=${PREFIX}/lib/python3.4/site-packages:/usr/local/lib/python3.4/dist-packages:${PYTHONPATH} python3 setup.py build_ext -L${PREFIX}/lib -I${PREFIX}/include:${VTK_INCLUDE_PATH} install --prefix=${PREFIX}
    PYTHONPATH=${PREFIX}/lib/python3.4/site-packages:/usr/local/lib/python3.4/dist-packages:${PYTHONPATH} LD_LIBRARY_PATH=${PREFIX}/lib python3 setup.py test
fi
