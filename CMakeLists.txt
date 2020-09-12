cmake_minimum_required(VERSION 3.10)
project(glsl-parser)

option(GLSL_PARSER_BUILD_EXAMPLE_APP "Toggle whether to build an example GLSL parser executable" OFF)
option(GLSL_PARSER_BUILD_TESTS "Toggle to build unit tests & enable testing" ON)

# place binaries and libraries according to GNU standards
include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})


set(glslParserSourceList 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glslParser/ast.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glslParser/debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glslParser/ast.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glslParser/util.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glslParser/lexemes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glslParser/lexer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glslParser/util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glslParser/debug.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glslParser/parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glslParser/lexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glslParser/parser.hpp
)

add_library(glslParser SHARED
    ${glslParserSourceList}
)
target_include_directories(glslParser PUBLIC $(CMAKE_CURRENT_SOURCE_DIR)/src)


#------------------------------------------------------------------------------
# Install glslParser lib
#------------------------------------------------------------------------------
install (TARGETS glslParser LIBRARY DESTINATION lib)

set(glslIncludeFilesPrepare ${glslParserSourceList})
list(FILTER glslIncludeFilesPrepare INCLUDE REGEX ".*h")

install (FILES ${glslIncludeFilesPrepare} DESTINATION include/glslParser)

#------------------------------------------------------------------------------
# Add example application
#------------------------------------------------------------------------------
if(${GLSL_PARSER_BUILD_EXAMPLE_APP})
    add_executable(glslParserFrontEnd
        apps/parser.cpp
    )
    target_link_libraries(glslParserFrontEnd PRIVATE glslParser)

    add_executable(dotprinter
        apps/dotprinter.cpp
    )
    target_link_libraries(dotprinter PRIVATE glslParser)

endif()

#------------------------------------------------------------------------------
# Unit tests
#------------------------------------------------------------------------------

if(${GLSL_PARSER_BUILD_TESTS})
    enable_testing()
    find_package(GTest REQUIRED)

    set(GTEST_ROOT "${CMAKE_BINARY_DIR}/bin")
    add_executable(unit_tests test/unit_tests.cpp test/lexer_test.cpp)
    add_test(NAME AllTests COMMAND "$<TARGET_FILE:unit_tests>")
    target_link_libraries(unit_tests ${GTEST_BOTH_LIBRARIES} glslParser)
endif()


