cmake_minimum_required (VERSION 3.8)

if(WIN32)
  include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()
set(CMAKE_CXX_STANDARD 20)

project ("rocketSimulator")

if(UNIX)
  include_directories("/usr/include/irrlicht")
  link_libraries("/usr/lib/libIrrlicht.so")
else()
  link_libraries(Irrlicht)
endif()

add_executable (rocketSimulator "main.cpp")
