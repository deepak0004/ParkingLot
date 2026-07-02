cmake_minimum_required(VERSION 4.2)
project(ParkingLot)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ParkingLot main.cpp)

# Headers are included as "parkinglot/<package>/<Class>.h", mirroring Java imports.
target_include_directories(ParkingLot PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Threads REQUIRED)
target_link_libraries(ParkingLot PRIVATE Threads::Threads)
