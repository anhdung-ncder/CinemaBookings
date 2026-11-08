#include "cinema/seating/Room.h"
#include "cinema/seating/SeatFactory.h"
#include <stdexcept>
#include "cinema/seating/SeatErrors.h"
#include <utility>
#include <string>
#include <vector>

namespace cinema {

Room::Room(std::string id, std::string name) : id_(std::move(id)), name_(std::move(name)) {}

std::shared_ptr<Room> Room::fromLayout(
    const std::string& id,
    const std::string& name,
    const Money& standardPrice,
    const std::string& layout
) {
    // 1. Kiem tra layout rong
    if (layout.empty()) {
        throw InvalidLayoutError("Layout cannot be empty");
    }

    // 2. Luu cac hang ghe
    std::vector<std::string> rows;

    std::size_t start = 0;

    // 3. Tach layout theo dau '|'
    while (true) {
        const std::size_t separator = layout.find('|', start);

        std::string row;

        if (separator == std::string::npos) {
            row = layout.substr(start);
        }
        else {
            row = layout.substr(start, separator - start);
        }

        // 4. Kiem tra hang rong
        if (row.empty()) {
            throw InvalidLayoutError("Empty row in layout");
        }

        // 5. Kiem tra so ghe trong hang
        if (row.size() > 30) {
            throw InvalidLayoutError("Too many seats in a row");
        }

        rows.push_back(row);

        // 6. Kiem tra so hang
        if (rows.size() > 26) {
            throw InvalidLayoutError("Too many rows in layout");
        }

        // 7. Neu khong con dau phan cach thi dung
        if (separator == std::string::npos) {
            break;
        }

        start = separator + 1;
    }

    // 8. Tao phong (bien cuc bo: neu co loi giua chung, Room tu huy, khong ro ri)
    auto room = std::make_shared<Room>(id, name);

    // 9. Duyet tung hang, tung ghe
    for (std::size_t i = 0; i < rows.size(); ++i) {
        const char rowLetter = static_cast<char>('A' + i);
        for (std::size_t j = 0; j < rows[i].size(); ++j) {
            const SeatId seatId(rowLetter, static_cast<int>(j) + 1);
            // Ky tu la (khong phai S/V/C) se do SeatFactory nem InvalidLayoutError
            room->addSeat(SeatFactory::create(rows[i][j], seatId, standardPrice));
        }
    }

    return room;
}

void Room::addSeat(std::unique_ptr<Seat> seat) {
    if (!seat) {
        throw InvalidLayoutError("Cannot add a null seat");
    }

    const SeatId seatId = seat->id();

    if (seats_.find(seatId) != seats_.end()) {
        throw InvalidLayoutError(
            "Duplicate seat ID: " + seatId.toString()
        );
    }

    seats_.emplace(seatId, std::move(seat));
}

const std::string& Room::id() const { 
    return id_;
}

const std::string& Room::name() const { 
    return name_; 
}

bool Room::hasSeat(const SeatId& id) const { 
    return seats_.find(id) != seats_.end();
}

const Seat& Room::seat(const SeatId& id) const { 
    auto a = seats_.find(id);
    if (a == seats_.end()) {
        throw SeatNotFoundError("Seat not found: " + id.toString());
    }
    
    return *a->second;
    
}

std::vector<const Seat*> Room::seats() const { 
    std::vector<const Seat*> result;

    result.reserve(seats_.size());

    for (const auto& entry : seats_) {
        result.push_back(entry.second.get());
    }

    return result;
}

std::size_t Room::seatCount() const { 
    return seats_.size();
}

} 
