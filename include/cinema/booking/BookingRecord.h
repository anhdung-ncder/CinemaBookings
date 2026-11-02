#pragma once
#include <string>
#include <vector>
#include "cinema/booking/Booking.h"
#include "cinema/common/Money.h"
#include "cinema/common/SeatId.h"

namespace cinema {

    
    struct BookingRecord {
        std::string bookingId;
        std::string showtimeId;
        std::vector<SeatId> seatIds;
        std::string customer;
        Money total;
        std::string paymentCode;
        BookingStatus status = BookingStatus::Confirmed;
    };
}  