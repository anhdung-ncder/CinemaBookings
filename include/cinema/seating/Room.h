#pragma once

#include "cinema/common/Money.h"
#include "cinema/common/SeatId.h"
#include "cinema/seating/Seat.h"

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace cinema {

class Room {
public:
    Room(std::string id, std::string name);

    // layout: cac hang ngan cach boi '|', moi ky tu 'S'|'V'|'C' la mot ghe.
    static std::shared_ptr<Room> fromLayout(const std::string& id, const std::string& name,
                                            const Money& standardPrice, const std::string& layout);

    void addSeat(std::unique_ptr<Seat> seat);   // trung SeatId -> InvalidLayoutError

    const std::string& id() const;
    const std::string& name() const;
    bool hasSeat(const SeatId& id) const;
    const Seat& seat(const SeatId& id) const;   // khong co -> SeatNotFoundError
    std::vector<const Seat*> seats() const;     // tang dan theo SeatId; con tro KHONG so huu
    std::size_t seatCount() const;

private:
    std::string id_;
    std::string name_;
    std::map<SeatId, std::unique_ptr<Seat>> seats_;   // Room la chu so huu duy nhat cua ghe
};

}  // namespace cinema
