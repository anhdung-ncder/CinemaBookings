#include "cinema/seating/Room.h"

#include <stdexcept>

namespace cinema {

Room::Room(std::string, std::string) { throw std::logic_error("not implemented"); }

std::shared_ptr<Room> Room::fromLayout(const std::string&, const std::string&, const Money&,
                                       const std::string&) {
    throw std::logic_error("not implemented");
}

void Room::addSeat(std::unique_ptr<Seat>) { throw std::logic_error("not implemented"); }

const std::string& Room::id() const { throw std::logic_error("not implemented"); }

const std::string& Room::name() const { throw std::logic_error("not implemented"); }

bool Room::hasSeat(const SeatId&) const { throw std::logic_error("not implemented"); }

const Seat& Room::seat(const SeatId&) const { throw std::logic_error("not implemented"); }

std::vector<const Seat*> Room::seats() const { throw std::logic_error("not implemented"); }

std::size_t Room::seatCount() const { throw std::logic_error("not implemented"); }

}  // namespace cinema
