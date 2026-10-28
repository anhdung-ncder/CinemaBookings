#pragma once

#include "cinema/common/Money.h"
#include "cinema/common/SeatId.h"
#include "cinema/seating/Seat.h"

#include <memory>

namespace cinema {

class SeatFactory {
public:
    // typeCode 'S' | 'V' | 'C' (chu hoa); ma khac -> InvalidLayoutError
    static std::unique_ptr<Seat> create(char typeCode, const SeatId& id, const Money& standardPrice);
};

}  // namespace cinema
