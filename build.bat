cmake -H. -Bbuild/windows -G "Visual Studio 15 2017" -DCMAKE_TOOLCHAIN_FILE=%cd%\cmake\toolchain.cmake
cmake --build build/windows --config Release