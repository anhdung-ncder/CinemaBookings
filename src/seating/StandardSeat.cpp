#include <cinema/seating/StandardSeat.h>
namespace cinema {

    StandardSeat::StandardSeat(const SeatId& id, const Money& standardPrice)
        : Seat(id), price_(standardPrice) {
    }

    SeatType StandardSeat::type() const { return SeatType::Standard; }

    Money StandardSeat::price() const { return price_; }

    int StandardSeat::capacity() const { return 1; }

    std::string StandardSeat::describe() const {
        return id().toString() + " [" + toString(type()) + "] " + price().toString();
    }

}