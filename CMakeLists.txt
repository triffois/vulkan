cmake_minimum_required(VERSION 3.22.0)
project(TriangularOmnissiah)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER g++)
list(APPEND CMAKE_PREFIX_PATH "/usr/lib/" "/usr/lib/x86_64-linux-gnu")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

#find_package(Vulkan REQUIRED)
#find_package(X11 REQUIRED)
#find_package(glfw3 REQUIRED)

#find_library(LIBXXF86VM Xxf86vm)
#find_library(LIBXRANDR Xrandr)
#find_library(LIBXI Xi)

set(SOURCES
    src/main.cpp
    src/AppInstance.cpp
)

add_executable(Triangle ${SOURCES})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#target_link_libraries(Triangle PRIVATE "/usr/lib/x86_64-linux-gnu/libglfw3.so.3")
#target_link_libraries(Triangle PRIVATE Vulkan::Vulkan)
#target_link_libraries(Triangle PRIVATE X11::X11)
#target_link_libraries(Triangle PRIVATE dl)
#target_link_libraries(Triangle PRIVATE pthread)
#target_link_libraries(Triangle PRIVATE ${LIBXXF86VM})
#target_link_libraries(Triangle PRIVATE ${LIBXI})
#target_link_libraries(Triangle PRIVATE ${LIBXRANDR})
list(APPEND CMAKE_EXE_LINKER_FLAGS  "-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi")