@echo off
rem 删除 build 目录及其内容
if exist build (
    rmdir /s /q build
)

rem 创建 build 目录
mkdir build

rem 进入 build 目录
cd build

rem 运行 CMake 配置
cmake ..

rem 运行构建
cmake --build .
