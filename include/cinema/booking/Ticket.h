#pragma once
#include <string>
#include "cinema/common/Money.h"
#include "cinema/common/SeatId.h"

namespace cinema {

    class Ticket {
    public:
        Ticket(const std::string& bookingId, const std::string& showtimeId,
            const SeatId& seatId, const Money& price);

        const std::string& ticketId() const;
        const std::string& bookingId() const;
        const std::string& showtimeId() const;
        const SeatId& seatId() const;
        Money price() const;

    private:
        std::string ticketId_;
        std::string bookingId_;
        std::string showtimeId_;
        SeatId seatId_;
        Money price_;
    };

}  