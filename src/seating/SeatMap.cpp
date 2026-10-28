#include "cinema/seating/SeatMap.h"

#include <stdexcept>

namespace cinema {

SeatMap::SeatMap(std::shared_ptr<const Room>) { throw std::logic_error("not implemented"); }

const Room& SeatMap::room() const { throw std::logic_error("not implemented"); }

SeatState SeatMap::stateOf(const SeatId&) const { throw std::logic_error("not implemented"); }

std::vector<SeatId> SeatMap::seatsInState(SeatState) const {
    throw std::logic_error("not implemented");
}

std::size_t SeatMap::count(SeatState) const { throw std::logic_error("not implemented"); }

Money SeatMap::quote(const std::vector<SeatId>&) const {
    throw std::logic_error("not implemented");
}

void SeatMap::hold(const std::vector<SeatId>&) { throw std::logic_error("not implemented"); }

void SeatMap::confirm(const std::vector<SeatId>&) { throw std::logic_error("not implemented"); }

void SeatMap::release(const std::vector<SeatId>&) { throw std::logic_error("not implemented"); }

void SeatMap::restoreSold(const std::vector<SeatId>&) {
    throw std::logic_error("not implemented");
}

}  // namespace cinema
