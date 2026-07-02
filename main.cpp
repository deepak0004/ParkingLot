// Runnable demo — the same two cases as the Java Main:
//   1. single-threaded: park two bikes, second is rejected (no SMALL spot left)
//   2. multi-threaded : 5 bikes race for 3 SMALL spots, 3 win, 2 rejected
//
// Each class lives in its own tiny header under parkinglot/, mirroring the Java
// package layout. No pointers anywhere.

#include <iostream>
#include <thread>
#include <vector>

#include "parkinglot/lot/ParkingLot.h"
#include "parkinglot/vehicle/Vehicle.h"

// Same idea as Java's printResult.
void printResult(int vehicleId, const Ticket& t) {
    if (!t.valid)
        std::cout << "No spot available for vehicle " << vehicleId << "\n";
    else
        std::cout << "Parked vehicle " << vehicleId
                  << " at spot " << t.spotId
                  << " (ticket " << t.ticketId << ")\n";
}

// Case 1 — one SMALL and one BIG spot; two bikes, only one SMALL fits.
void caseOneSingleThreaded() {
    Level level0{0, {{0, SpotType::SMALL}, {1, SpotType::BIG}}};
    ParkingLot lot({level0});

    Vehicle bike1{123, VehicleType::BIKE};
    printResult(bike1.id, lot.reserve(bike1));

    Vehicle bike2{223, VehicleType::BIKE};
    printResult(bike2.id, lot.reserve(bike2));   // no SMALL spot left -> rejected
}

// Case 2 — 3 SMALL spots, 5 bikes parking at the same time.
void caseTwoMultiThreaded() {
    Level level0{0, {{0, SpotType::SMALL}, {1, SpotType::SMALL}, {2, SpotType::SMALL}}};
    ParkingLot lot({level0});

    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        int vehicleId = 1000 + i;
        threads.emplace_back([&lot, vehicleId] {
            printResult(vehicleId, lot.reserve(Vehicle{vehicleId, VehicleType::BIKE}));
        });
    }
    for (std::thread& t : threads)
        t.join();
}

int main() {
    std::cout << "===== Case 1: single-threaded =====\n";
    caseOneSingleThreaded();

    std::cout << "\n===== Case 2: multi-threaded =====\n";
    caseTwoMultiThreaded();
    return 0;
}
