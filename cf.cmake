# Visual Studio

case "$1" in
vs|VS) VS=1 ;;
linux|nix|l|x) unset VS ;;
*) echo "Must specify env: vs|linux"; exit 1; ;;
esac

if test "x$VS" = x1 ; then
  if test "x$2" = xsetup ; then
    VSSETUP=1
  else
    unset VSSETUP
  fi
fi

if test "x$VSSETUP" = x1 ; then
CFG="Debug"
else
CFG="Release"
fi

FLAGS="$FLAGS -DCMAKE_INSTALL_PREFIX=/tmp/cpoco"

#FLAGS="$FLAGS -DENABLE_PTHREAD=false"
#FLAGS="$FLAGS -DENABLE_MUTEX=false"

rm -fr build
mkdir build
cd build

if test "x$VS" != x ; then
# Visual Studio
CFG="Release"
cmake -DCMAKE_BUILD_TYPE=${CFG} $FLAGS ..
cmake --build . --config ${CFG}
cmake --build . --config ${CFG} --target test
else
# GCC
cmake "${G}" $FLAGS ..
make all
make test
fi
exit
