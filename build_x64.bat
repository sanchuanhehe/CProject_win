@echo off
conan install . --build=missing --profile=conan_profile_x64

mkdir build

cd build

cmake .. -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_Toolchain_FILE="conan_paths.cmake"

cmake --build . --config DEBUG

cd ../out/build/X86_64/

echo ============Running MyProject.exe=============
.\MyProject