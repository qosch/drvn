add_library(au INTERFACE)
target_include_directories(au SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/external/au)

add_library(doctest INTERFACE)
target_include_directories(doctest SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/external/doctest)

add_executable(host_tests clock_test.cpp)
target_compile_definitions(host_tests PRIVATE DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)
target_include_directories(host_tests PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(host_tests PRIVATE au doctest)

add_test(NAME host_tests COMMAND host_tests)
