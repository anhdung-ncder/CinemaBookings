#include "cinema/seating/Seat.h"

namespace cinema {

Seat::Seat(SeatId id) : id_(id) {}

const SeatId& Seat::id() const { return id_; }

}  // namespace cinema
