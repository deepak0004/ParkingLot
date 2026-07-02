#pragma once

#include "SpotType.h"

struct Spot {
    int id;
    SpotType type;
    bool taken = false;   // Java used an AtomicBoolean. Here one mutex inside
                          // ParkingLot guards every read/write, so a plain
                          // bool is safe and much easier to read.
};
