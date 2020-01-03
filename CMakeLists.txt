cmake_minimum_required(VERSION 3.13)
project(boost_playground)

set(CMAKE_CXX_STANDARD 17)
if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
endif()

# Find Boost:
set(BOOST_VERSION 1.71.0 CACHE STRING "Boost Version to use")
set(Boost_NO_BOOST_CMAKE TRUE)

set(BOOST_VERSION_MATCH ${BOOST_VERSION})
set(Boost_ADDITIONAL_VERSIONS "1.71.0" "1.71")
set(BOOST_PYTHON_VERSION 37)
#set(BOOST_ROOT "/usr/local/Cellar")
find_package(PythonLibs 3.7 REQUIRED)
message(STATUS "Python Include Dirs: " ${PYTHON_INCLUDE_DIRS})
find_package(Boost COMPONENTS python37 graph REQUIRED)
set(boost_link_libraries ${Boost_PYTHON${BOOST_PYTHON_VERSION}_LIBRARY})
message(STATUS "Boost libraries: " ${Boost_LIBRARIES})
message(STATUS "Boost link libraries: " ${boost_link_libraries})
message(STATUS "Boost include directory: " ${Boost_INCLUDE_DIRS})

# Find Python:
set(Python_ADDITIONAL_VERSION 3.7 ${Boost_PYTHON_VERSION})
find_package(Python3 COMPONENTS Interpreter REQUIRED)
message(STATUS "Python version: " ${Python_VERSION})
message(STATUS "  exe: " ${Python3_EXECUTABLE})
message(STATUS "  lib: " ${PYTHON_LIBRARIES})
message(STATUS "  include: " ${Python3_INCLUDE_DIRS})

# Find dlib:
set(dlib_DIR $ENV{SHYFT_WORKSPACE}/shyft_dependencies/lib/cmake/dlib)
find_package(dlib REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/graph)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(/usr/local/include)
include_directories(${dlib_INCLUDE_DIRS})

# Set the library dir:
set( LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib" )
add_subdirectory(src)
add_subdirectory(graph)
add_subdirectory(test)

add_library(_playground SHARED pyapi/pyapi.cpp test/test_network.cpp)
target_link_libraries(_playground ${boost_link_libraries}
        ${PYTHON_LIBRARIES}
        MyClass
        ParentChild
        KeepingReference)
set_target_properties(_playground PROPERTIES PREFIX "")
set_target_properties(_playground PROPERTIES SUFFIX ".so")

