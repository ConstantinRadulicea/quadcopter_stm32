cmake_minimum_required(VERSION 3.22)

# Setup project
project(LwLibPROJECT)

if(NOT PROJECT_IS_TOP_LEVEL)
    add_subdirectory(lwshell)
else()
    enable_testing()
    add_executable(${PROJECT_NAME})
    target_sources(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/dev/main.c
        ${CMAKE_CURRENT_LIST_DIR}/tests/test.c
    )
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/dev
    )

    # Add subdir with lwshell and link to project
    set(LWSHELL_OPTS_FILE ${CMAKE_CURRENT_LIST_DIR}/dev/lwshell_opts.h)
    add_subdirectory(lwshell)
    target_link_libraries(${PROJECT_NAME} lwshell)

    # Add compile options to the library, which will propagate options to executable through public link
    target_compile_definitions(lwshell PUBLIC WIN32 _DEBUG CONSOLE LWSHELL_DEV)
    target_compile_options(lwshell PUBLIC -Wall -Wextra -Wpedantic)

    # Add test
    add_test(NAME Test COMMAND $<TARGET_FILE:${CMAKE_PROJECT_NAME}>)
endif()
