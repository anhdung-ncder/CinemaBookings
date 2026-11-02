#pragma once
#include <memory>
#include <string>
#include <vector>
#include "cinema/booking/Booking.h"
#include "cinema/booking/BookingRecord.h"
#include "cinema/booking/IPayment.h"
#include "cinema/catalog/Showtime.h"
#include "cinema/common/Money.h"
#include "cinema/common/SeatId.h"

namespace cinema {

    class BookingService {
    public:
        BookingService();

        std::shared_ptr<const Booking> book(std::shared_ptr<Showtime> showtime,
            const std::vector<SeatId>& seatIds,
            std::shared_ptr<IPayment> payment,
            const std::string& customer);
        Money cancel(const std::string& bookingId);
        std::shared_ptr<const Booking> get(const std::string& bookingId) const;
        std::vector<std::shared_ptr<const Booking>> all() const;
        std::vector<BookingRecord> exportRecords() const;
        void importBooking(const BookingRecord& record, std::shared_ptr<Showtime> showtime);

    private:
        std::vector<std::shared_ptr<Booking>> bookings_;
        int nextNumber_ = 1;                               // BK0001, BK0002...
    };

}  