#include "cinema/booking/Booking.h"

namespace cinema {

    std::string toString(BookingStatus status) {
        return status == BookingStatus::Confirmed ? "CONFIRMED" : "CANCELLED";
    }

    Booking::Booking(const std::string& id, std::shared_ptr<Showtime> showtime,
        const std::vector<SeatId>& seatIds, const std::string& customer,
        const Money& total, std::shared_ptr<IPayment> payment,
        const std::string& paymentCode, const std::vector<Ticket>& tickets)
        : id_(id), showtime_(std::move(showtime)), seatIds_(seatIds), customer_(customer),
        total_(total), payment_(std::move(payment)), paymentCode_(paymentCode),
        tickets_(tickets) {
    }

    const std::string& Booking::id() const { return id_; }
    const Showtime& Booking::showtime() const { return *showtime_; }
    const std::vector<SeatId>& Booking::seatIds() const { return seatIds_; }
    const std::string& Booking::customer() const { return customer_; }
    Money Booking::total() const { return total_; }
    const std::string& Booking::paymentCode() const { return paymentCode_; }
    BookingStatus Booking::status() const { return status_; }
    const std::vector<Ticket>& Booking::tickets() const { return tickets_; }

}  