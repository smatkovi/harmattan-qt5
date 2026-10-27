#! /bin/sh

LCOV=`which lcov`

if test "x$LCOV" = x; then
    echo "You need to install lcov to get actual reports!";
    echo "See http://ltp.sf.net/coverage/lcov.php";
    echo "!!! ATTENTION !!!"
    echo "The current lcov-1.8 contains some silly bug.";
    echo "Please, use a newer one or the cvs version."
    exit 1;
fi

export SBOX_USE_CCACHE=no

rm -fr ./coverage/*

# For better branch coverage, lets filter out the debug messages
CXXFLAGS="-DPUBLIC_RELEASE -O0 -Wnone -g"
export CXXFLAGS=${CXXFLAGS}
export QMAKE_CXXFLAGS=${CXXFLAGS}

echo "qmake BUILD_FEATURES=coverage"
qmake BUILD_FEATURES=coverage

echo "make clean"
make clean

echo "./configure"
./configure

echo "make coverage"
make coverage

ls -lh ./coverage/index.html

