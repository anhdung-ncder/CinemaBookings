#include "cinema/seating/CoupleSeat.h"

#include <stdexcept>

namespace cinema {

CoupleSeat::CoupleSeat(const SeatId& id, const Money&) : Seat(id) {
    throw std::logic_error("not implemented");
}

SeatType CoupleSeat::type() const { throw std::logic_error("not implemented"); }

Money CoupleSeat::price() const { throw std::logic_error("not implemented"); }

int CoupleSeat::capacity() const { throw std::logic_error("not implemented"); }

std::string CoupleSeat::describe() const { throw std::logic_error("not implemented"); }

}  // namespace cinema
