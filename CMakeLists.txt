cmake_minimum_required(VERSION 3.10)
set(CMAKE_TOOLCHAIN_FILE "D:/Library/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(Watermark3D LANGUAGES CXX)

# 设置c++标准 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#################################################
find_package(Qt5 COMPONENTS core gui widgets  REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

#################################################
# 添加ui文件
set(UI_DIRS ui)
file(GLOB_RECURSE UI_FILES ${UI_DIRS}/*.ui)
qt5_wrap_ui(ui_FILES ${UI_FILES})
message("UI Files:")
message(${UI_FILES})
# 将ui文件添加到筛选器
source_group("Form Files" FILES ${UI_FILES})
#################################################
# 添加shader文件
set(SHADER_DIRS shader)
file(GLOB_RECURSE SHADER_FILES ${SHADER_DIRS}/*.frag ${SHADER_DIRS}/*.vert ${SHADER_DIRS}/*.vs ${SHADER_DIRS}/*.fs)
message("SHADER Files:")
message(${SHADER_FILES})
# 将shader文件添加到筛选器
# Note: 只有在 add_executable() 中的文件才会被 source_group 分组. 如果你发现有些文件没有被正确分组, 应该首先确认这些文件是否出现在 add_executable() 的 [source-x] 列表中.
source_group("Shader Files" FILES ${SHADER_FILES}) 

#################################################
# 添加头文件目录
set(HEADER_DIRS src)
include_directories(${HEADER_DIRS})
# 查找源文件并将它们添加到可执行文件
file(GLOB_RECURSE HEADER_FILES ${HEADER_DIRS}/*.h)
message("Header 内容:")
message(${HEADER_FILES})
#################################################
set(SOURCE_DIR src)
file(GLOB SOURCES ${SOURCE_DIR}/*.cpp)
message("Sources 内容:")
message(${SOURCES})
#################################################
# 配置release版本的CMAKE_EXE_LINKER_FLAGS
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
# 配置debug版本的CMAKE_EXE_LINKER_FLAGS
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")

#################################################
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADER_FILES} ${ui_FILES} ${SHADER_FILES})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )
target_link_libraries(${PROJECT_NAME} fmt::fmt)
target_link_libraries(${PROJECT_NAME} assimp::assimp)

# 判断是否是visual studio
if (CMAKE_GENERATOR MATCHES "Visual Studio")
    message("Generating Visual Studio project")
    # 设置visual studio的启动项目
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
else()
    message("Not generating Visual Studio project")
endif()
