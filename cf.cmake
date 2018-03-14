# Is visual studio being used?
#VS=yes
CYGWIN=yes

if test "x$VS" = x ; then
CC=gcc
fi

export CC

#FLAGS="$FLAGS -DCMAKE_INSTALL_PREFIX=/usr/local"
#FLAGS="-DCMAKE_PREFIX_PATH=/usr/local"
#FLAGS="$FLAGS -DENABLE_MUTEX=false"

rm -fr build
mkdir build
cd build

cmake $FLAGS ..
cmake --build .
cmake --build . --target test
#cmake --build . --target install
#CTEST_OUTPUT_ON_FAILURE=1 cmake --build . --target test
