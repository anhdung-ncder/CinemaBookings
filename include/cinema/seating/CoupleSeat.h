#pragma once

#include "cinema/seating/Seat.h"

#include <string>

namespace cinema {

class CoupleSeat : public Seat {
public:
    CoupleSeat(const SeatId& id, const Money& standardPrice);

    SeatType type() const override;
    Money price() const override;
    int capacity() const override;
    std::string describe() const override;

private:
    Money price_;
};

}  // namespace cinema
