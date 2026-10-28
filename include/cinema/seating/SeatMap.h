#pragma once

#include "cinema/common/Money.h"
#include "cinema/common/SeatId.h"
#include "cinema/seating/Room.h"
#include "cinema/seating/SeatTypes.h"

#include <cstddef>
#include <map>
#include <memory>
#include <vector>

namespace cinema {

// Trang thai ghe cua MOT suat chieu.
class SeatMap {
public:
    explicit SeatMap(std::shared_ptr<const Room> room);   // moi ghe bat dau Available

    const Room& room() const;
    SeatState stateOf(const SeatId& id) const;
    std::vector<SeatId> seatsInState(SeatState state) const;   // tang dan theo SeatId
    std::size_t count(SeatState state) const;
    Money quote(const std::vector<SeatId>& ids) const;

    // ALL-OR-NOTHING: nem exception thi KHONG ghe nao doi.
    void hold(const std::vector<SeatId>& ids);          // Available -> Held
    void confirm(const std::vector<SeatId>& ids);       // Held -> Sold
    void release(const std::vector<SeatId>& ids);       // Held/Sold -> Available (idempotent)
    void restoreSold(const std::vector<SeatId>& ids);   // Available -> Sold

private:
    std::shared_ptr<const Room> room_;
    std::map<SeatId, SeatState> states_;
};

}  // namespace cinema
