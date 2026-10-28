#include "cinema/seating/VipSeat.h"

#include <stdexcept>

namespace cinema {

VipSeat::VipSeat(const SeatId& id, const Money&) : Seat(id) {
    throw std::logic_error("not implemented");
}

SeatType VipSeat::type() const { throw std::logic_error("not implemented"); }

Money VipSeat::price() const { throw std::logic_error("not implemented"); }

int VipSeat::capacity() const { throw std::logic_error("not implemented"); }

std::string VipSeat::describe() const { throw std::logic_error("not implemented"); }

}  // namespace cinema
