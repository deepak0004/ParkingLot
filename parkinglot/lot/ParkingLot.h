#pragma once

#include <vector>
#include <map>
#include <mutex>

#include "../vehicle/Vehicle.h"
#include "../level/Level.h"
#include "../ticket/Ticket.h"
#include "../strategy/SequentialStrategy.h"

class ParkingLot {
    std::vector<Level> levels_;
    std::mutex lock_;                          // one lock guards the whole lot —
                                               // the simplest correct design.
    int nextTicketId_ = 1;
    std::map<int, Ticket> vehicleToTicket_;    // vehicleId -> its active ticket

public:
    ParkingLot(std::vector<Level> levels) : levels_(std::move(levels)) {}

    // Park the vehicle and return its ticket, or an invalid ticket if full.
    Ticket reserve(const Vehicle& v) {
        std::lock_guard<std::mutex> guard(lock_);   // whole method = one critical section

        // Already parked? Return the same ticket (idempotent, like Java).
        auto it = vehicleToTicket_.find(v.id);
        if (it != vehicleToTicket_.end())
            return it->second;

        SpotLocation loc = findSpot(v, levels_);
        if (!loc.found)
            return Ticket{0, 0, false};             // no spot fits right now

        Spot& spot = levels_[loc.levelIndex].spots[loc.spotIndex];
        spot.taken = true;
        Ticket t{nextTicketId_++, spot.id, true};
        vehicleToTicket_[v.id] = t;
        return t;
    }

    // Free the vehicle's spot. No-op if the ticket isn't its current one.
    void unreserve(const Vehicle& v, const Ticket& t) {
        std::lock_guard<std::mutex> guard(lock_);

        auto it = vehicleToTicket_.find(v.id);
        if (it == vehicleToTicket_.end() || it->second.ticketId != t.ticketId)
            return;

        for (Level& level : levels_)
            for (Spot& spot : level.spots)
                if (spot.id == t.spotId)
                    spot.taken = false;

        vehicleToTicket_.erase(it);
    }
};
