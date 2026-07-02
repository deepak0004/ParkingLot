// The ENTIRE parking lot in one file — the interview form.
//
// Notice what's GONE compared to the parkinglot/ folder version:
//   - no `#pragma once`   (nothing is included twice)
//   - no `inline`         (one definition of each function, one .cpp)
//   - no `namespace`      (nothing to collide with)
//   - no cross-file #includes
// All that ceremony existed ONLY because the code was split into headers.
// One file, read top-to-bottom, define-before-use.
//
// Build: c++ -std=c++20 -pthread single_file_version.cpp -o demo && ./demo

#include <iostream>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
using namespace std;   // fine in an interview / single file; never in a real header

// ---------- data model ----------
enum class VehicleType { CAR, BIKE, TRUCK };
enum class SpotType    { BIG, MED, SMALL };

struct Vehicle { int id; VehicleType type; };
struct Spot    { int id; SpotType type; bool taken = false; };
struct Ticket  { int ticketId; int spotId; bool valid; };   // valid==false => no spot
struct Level   { int number; vector<Spot> spots; };

// which spot type fits which vehicle
bool fits(VehicleType v, SpotType s) {
    if (v == VehicleType::CAR)  return s == SpotType::MED;
    if (v == VehicleType::BIKE) return s == SpotType::SMALL;
    return s == SpotType::BIG;   // TRUCK
}

// ---------- the coordinator ----------
class ParkingLot {
    vector<Level> levels;
    mutex lock;                       // one lock guards the whole lot
    int nextTicketId = 1;
    map<int, Ticket> vehicleToTicket; // vehicleId -> its active ticket

public:
    ParkingLot(vector<Level> levels) : levels(std::move(levels)) {}

    Ticket reserve(const Vehicle& v) {
        lock_guard<mutex> guard(lock);

        auto it = vehicleToTicket.find(v.id);
        if (it != vehicleToTicket.end()) return it->second;   // already parked

        for (Level& level : levels)
            for (Spot& spot : level.spots)
                if (!spot.taken && fits(v.type, spot.type)) {
                    spot.taken = true;
                    Ticket t{nextTicketId++, spot.id, true};
                    vehicleToTicket[v.id] = t;
                    return t;
                }
        return Ticket{0, 0, false};    // nothing fits
    }

    void unreserve(const Vehicle& v, const Ticket& t) {
        lock_guard<mutex> guard(lock);
        auto it = vehicleToTicket.find(v.id);
        if (it == vehicleToTicket.end() || it->second.ticketId != t.ticketId) return;
        for (Level& level : levels)
            for (Spot& spot : level.spots)
                if (spot.id == t.spotId) spot.taken = false;
        vehicleToTicket.erase(it);
    }
};

// ---------- demo ----------
void printResult(int vehicleId, const Ticket& t) {
    if (!t.valid) cout << "No spot available for vehicle " << vehicleId << "\n";
    else cout << "Parked vehicle " << vehicleId << " at spot " << t.spotId
              << " (ticket " << t.ticketId << ")\n";
}

int main() {
    cout << "===== Case 1: single-threaded =====\n";
    {
        Level level0{0, {{0, SpotType::SMALL}, {1, SpotType::BIG}}};
        ParkingLot lot({level0});
        Vehicle bike1{123, VehicleType::BIKE};
        printResult(bike1.id, lot.reserve(bike1));
        Vehicle bike2{223, VehicleType::BIKE};
        printResult(bike2.id, lot.reserve(bike2));   // no SMALL left -> rejected
    }

    cout << "\n===== Case 2: multi-threaded =====\n";
    {
        Level level0{0, {{0, SpotType::SMALL}, {1, SpotType::SMALL}, {2, SpotType::SMALL}}};
        ParkingLot lot({level0});
        vector<thread> threads;
        for (int i = 0; i < 5; ++i) {
            int vehicleId = 1000 + i;
            threads.emplace_back([&lot, vehicleId] {
                printResult(vehicleId, lot.reserve(Vehicle{vehicleId, VehicleType::BIKE}));
            });
        }
        for (thread& t : threads) t.join();
    }
    return 0;
}
