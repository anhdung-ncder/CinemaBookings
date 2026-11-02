#include "cinema/booking/Ticket.h"

namespace cinema {

    Ticket::Ticket(const std::string& bookingId, const std::string& showtimeId,
        const SeatId& seatId, const Money& price)
        : ticketId_(bookingId + "-" + seatId.toString()),
        bookingId_(bookingId),
        showtimeId_(showtimeId),
        seatId_(seatId),
        price_(price) {
    }

    const std::string& Ticket::ticketId() const { return ticketId_; }
    const std::string& Ticket::bookingId() const { return bookingId_; }
    const std::string& Ticket::showtimeId() const { return showtimeId_; }
    const SeatId& Ticket::seatId() const { return seatId_; }
    Money Ticket::price() const { return price_; }

}  