# Электронный путеводитель. Сборка программы и создание базы из SQL.
cmake_minimum_required(VERSION 3.24)
project(ElectronicGuide LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

add_executable(guide_seed tools/seed.cpp)
add_executable(ElectronicGuide WIN32 src/main.cpp src/catalog.cpp)
target_include_directories(ElectronicGuide PRIVATE include)
target_compile_definitions(ElectronicGuide PRIVATE UNICODE _UNICODE NOMINMAX)
target_link_libraries(ElectronicGuide PRIVATE comctl32 uxtheme)
foreach(target IN ITEMS guide_seed ElectronicGuide)
    target_compile_features(${target} PRIVATE cxx_std_17)
    target_compile_options(${target} PRIVATE /utf-8 /W4)
    target_link_libraries(${target} PRIVATE winsqlite3)
endforeach()

set(seed_sql "${CMAKE_CURRENT_SOURCE_DIR}/resources/catalog.sql")
set(database "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/catalog.sqlite3")
add_custom_command(OUTPUT "${database}"
    COMMAND guide_seed "${seed_sql}" "${database}"
    DEPENDS guide_seed "${seed_sql}"
    VERBATIM)
add_custom_target(guide_database DEPENDS "${database}")
add_dependencies(ElectronicGuide guide_database)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ElectronicGuide)
