#pragma once

#include <vector>

#include "../vehicle/Vehicle.h"
#include "../spot/Spot.h"
#include "../level/Level.h"

// Which spot type fits which vehicle (Java's SequentialStrategy.matches).
inline bool fits(VehicleType v, SpotType s) {
    if (v == VehicleType::CAR)  return s == SpotType::MED;
    if (v == VehicleType::BIKE) return s == SpotType::SMALL;
    return s == SpotType::BIG;   // TRUCK
}

// Where a free spot lives. found == false means "no spot available".
// We return indices instead of a pointer, keeping the design pointer-free.
struct SpotLocation {
    bool found = false;
    int levelIndex = -1;
    int spotIndex  = -1;
};

// Scan the levels in order and return the first free spot that fits.
// Swap this function for another to change the parking policy.
inline SpotLocation findSpot(const Vehicle& vehicle, std::vector<Level>& levels) {
    for (int li = 0; li < (int) levels.size(); ++li) {
        std::vector<Spot>& spots = levels[li].spots;
        for (int si = 0; si < (int) spots.size(); ++si) {
            if (!spots[si].taken && fits(vehicle.type, spots[si].type))
                return {true, li, si};
        }
    }
    return {};   // nothing fits right now
}
