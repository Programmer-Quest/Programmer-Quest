cmake_minimum_required(VERSION 3.16)

project(ProgrammerQuest)

add_executable(ProgrammerQuest Main.cpp)

target_compile_features(ProgrammerQuest PUBLIC cxx_std_20)

find_package(Crow)

target_link_libraries(ProgrammerQuest PUBLIC Crow::Crow)