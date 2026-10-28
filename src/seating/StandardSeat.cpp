#include "cinema/seating/StandardSeat.h"

#include <stdexcept>

namespace cinema {

StandardSeat::StandardSeat(const SeatId& id, const Money&) : Seat(id) {
    throw std::logic_error("not implemented");
}

SeatType StandardSeat::type() const { throw std::logic_error("not implemented"); }

Money StandardSeat::price() const { throw std::logic_error("not implemented"); }

int StandardSeat::capacity() const { throw std::logic_error("not implemented"); }

std::string StandardSeat::describe() const { throw std::logic_error("not implemented"); }

}  // namespace cinema
