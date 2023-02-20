call vcvarsall.bat x64

cmake --list-presets=all .
cmake --preset=x64-clang-debug .
cmake --build --preset=build-x64-clang-debug