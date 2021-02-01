cmake_minimum_required(VERSION 3.17)
project(DumpModule)

set(CMAKE_CXX_STANDARD 20)

add_executable(DumpModule src/main.cpp)

# Dependencies
include(cmake/CPM.cmake)

CPMAddPackage(
        NAME Detours
        GITHUB_REPOSITORY microsoft/Detours
        VERSION 4.0.1
)
if (Detours_ADDED)
    # Detours is an nmake library, so we need to prebuild it.
    # We need to do some wack setup to make it work universally, though.
    execute_process(COMMAND ${CMAKE_SOURCE_DIR}/scripts/BuildDetours.bat WORKING_DIRECTORY ${Detours_SOURCE_DIR}/src)
    set(Detours_Lib ${Detours_SOURCE_DIR}/lib.X86/detours.lib)
    target_include_directories(DumpModule PUBLIC ${Detours_SOURCE_DIR}/include)
endif()

target_link_libraries(DumpModule ${Detours_Lib})