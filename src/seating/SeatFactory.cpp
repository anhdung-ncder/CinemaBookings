#include "cinema/seating/SeatFactory.h"

#include <stdexcept>

namespace cinema {

std::unique_ptr<Seat> SeatFactory::create(char, const SeatId&, const Money&) {
    throw std::logic_error("not implemented");
}

}  // namespace cinema
