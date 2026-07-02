#pragma once

#include "VehicleType.h"

// Java had an abstract Vehicle with Car/Bike/Truck subclasses. The only real
// difference between them is the type, so one plain struct is enough here.
struct Vehicle {
    int id;
    VehicleType type;
};
