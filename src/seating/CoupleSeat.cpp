#include "cinema/seating/CoupleSeat.h"

namespace cinema {

    namespace {
        constexpr int kCouplePercent = 180;  // gia ghe doi = 180% gia chuan (cho ca 2 nguoi)
    }  // namespace

    CoupleSeat::CoupleSeat(const SeatId& id, const Money& standardPrice)
        : Seat(id), price_(standardPrice.scaledPercent(kCouplePercent)) {
    }

    SeatType CoupleSeat::type() const { return SeatType::Couple; }

    Money CoupleSeat::price() const { return price_; }

    int CoupleSeat::capacity() const { return 2; }

    std::string CoupleSeat::describe() const {
        return id().toString() + " [" + toString(type()) + "] " + price().toString();
    }

}  