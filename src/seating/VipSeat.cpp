#include "cinema/seating/VipSeat.h"

namespace cinema {

    namespace {
        constexpr int kVipPercent = 150;  // gia VIP = 150% gia chuan
    }  // namespace

    VipSeat::VipSeat(const SeatId& id, const Money& standardPrice)
        : Seat(id), price_(standardPrice.scaledPercent(kVipPercent)) {
    }

    SeatType VipSeat::type() const { return SeatType::Vip; }

    Money VipSeat::price() const { return price_; }

    int VipSeat::capacity() const { return 1; }

    std::string VipSeat::describe() const {
        return id().toString() + " [" + toString(type()) + "] " + price().toString();
    }

}  