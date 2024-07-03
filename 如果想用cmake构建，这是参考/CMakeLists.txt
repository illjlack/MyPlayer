# 指定CMake的最低版本
cmake_minimum_required(VERSION 3.10)

# 工程名
project(MyPlayer)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 如果CMake版本低于3.7.0，则启用CMAKE_INCLUDE_CURRENT_DIR选项
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    message(STATUS "============== CMake version is less than 3.7.0, enabling CMAKE_INCLUDE_CURRENT_DIR option. ==============")
endif()

# 自动处理moc、uic和rcc,不懂去查一下
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置为WIN32可执行文件（仅在Windows下有效）
set(WIN32_EXECUTABLE ON)
message(STATUS "============== Setting WIN32_EXECUTABLE to ON. ==============")

# 设置Qt5的cmake文件路径（可能需要，改一下路径）
# set(Qt5_DIR D:/Qt/5.9.3/msvc2017_64/lib/cmake/Qt5/)
# message(STATUS "============== Setting Qt5_DIR to: ${Qt5_DIR} ==============")

# 查找并包含Qt模块
find_package(Qt5 COMPONENTS Core Gui Widgets Sql Network Multimedia REQUIRED)

# 确保包含Qt5的CMake模块
include_directories(${Qt5Widgets_INCLUDE_DIRS})

# 收集所有源文件和头文件，排除build目录
file(GLOB_RECURSE SOURCES 
    "*.cpp" 
    "*.h" 
    "*.qrc" 
    "*.ui"
)

# 排除build目录下的文件
list(FILTER SOURCES EXCLUDE REGEX "^${CMAKE_CURRENT_SOURCE_DIR}/build/")

message(STATUS "============== Collected all source files: ${SOURCES} ==============")

# 添加可执行文件
add_executable(MyPlayer WIN32 ${SOURCES})

# 链接Qt库
target_link_libraries(MyPlayer Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Sql Qt5::Network Qt5::Multimedia)

# 输出链接信息
message(STATUS "============== Successfully linked Qt libraries. ==============")

# 添加其他设置或配置

# 需要两个opensll的两个.dll,用于https,手移一下吧


