#pragma once

struct Ticket {
    int ticketId;
    int spotId;
    bool valid;   // valid == false means "no spot" — the equivalent of the
                  // null Ticket that Java returned when the lot was full.
};
