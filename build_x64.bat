@echo off

REM 安装依赖项
conan install . --build=missing --profile=conan_profile_x64

REM 创建构建目录
mkdir build

cd build

REM 配置 CMake 项目
cmake .. -G "Ninja" ^
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
    -DCMAKE_BUILD_TYPE=Debug ^
    --preset conan-release

REM 构建项目
cmake --build . --config DEBUG

cd ../out

echo ============Running MyProject.exe=============
.\MyProject

pause

cd ..