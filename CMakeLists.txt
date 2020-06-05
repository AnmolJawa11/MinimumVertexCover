cmake_minimum_required(VERSION 2.8)
project(prjece650)
find_package (Threads)

if (EXISTS "${CMAKE_SOURCE_DIR}/minisat")
  add_subdirectory(minisat)
else()
  message (FATAL_ERROR "MiniSat not found. Use `git clone https://github.com/agurfinkel/minisat` to check it out.")
endif()


## ECE650 Specific configuration start here
## Project specific configurations go here

# enable c++11 support
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall ${CMAKE_CXX_FLAGS}")

# expose minisat header files
include_directories(${CMAKE_SOURCE_DIR}/minisat)

# create the main executable
## add additional .cpp files if needed
add_executable(prjece650 ece650-a4.cpp GeneralUtil.cpp GeneralUtil.h GraphProps.cpp Validator.cpp Validator.h)
# link MiniSAT libraries

target_link_libraries(prjece650 minisat-lib-static ${CMAKE_THREAD_LIBS_INIT})

# create the executable for tests
add_executable(test test.cpp)
# link MiniSAT libraries
target_link_libraries(test minisat-lib-static)
