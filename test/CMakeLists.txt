find_package(doctest REQUIRED)

set(test_sources
        test_network.cpp model_networks.h test_boost_graph.cpp test_virtual.cpp test_binary_visitor.cpp)

set(target "test_playground")
add_definitions("-D__UNIT_TEST__ -DVERBOSE=0 ")
add_executable(${target} ${test_sources})
target_include_directories(${target} PRIVATE ".")
target_link_libraries(${target} ${boost_link_librarie} dlib::dlib)

#set_target_properties(${target} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
set_target_properties(${target} PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
install(TARGETS ${target} CONFIGURATIONS Release DESTINATION ${CMAKE_SOURCE_DIR}/bin/Release)
install(TARGETS ${target} CONFIGURATIONS Debug DESTINATION ${CMAKE_SOURCE_DIR}/bin/Debug)
if(BUILD_COVERAGE)
    include(${PROJECT_SOURCE_DIR}/build_support/CodeCoverage.cmake)
    APPEND_COVERAGE_COMPILER_FLAGS()
    set(COVERAGE_EXCLUDES '/usr/include/*' '*/boost/*' '*/armadillo*' '*/dlib/*' '*/doctest/*' '${SHYFT_DEPENDENCY_DIR}/include/*' )
    SETUP_TARGET_FOR_COVERAGE(
            NAME ${target}_coverage
            EXECUTABLE ${target}
            DEPENDENCIES ${target}
    )
endif()
