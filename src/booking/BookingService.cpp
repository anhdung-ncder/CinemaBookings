#include "cinema/booking/BookingService.h"
#include <stdexcept>

namespace cinema {

    BookingService::BookingService() = default;

    std::shared_ptr<const Booking> BookingService::book(std::shared_ptr<Showtime>,
        const std::vector<SeatId>&, std::shared_ptr<IPayment>, const std::string&) {
        throw std::logic_error("BookingService::book chua lam");
    }
    Money BookingService::cancel(const std::string&) {
        throw std::logic_error("BookingService::cancel chua lam");
    }
    std::shared_ptr<const Booking> BookingService::get(const std::string&) const {
        throw std::logic_error("BookingService::get chua lam");
    }
    std::vector<std::shared_ptr<const Booking>> BookingService::all() const {
        throw std::logic_error("BookingService::all chua lam");
    }
    std::vector<BookingRecord> BookingService::exportRecords() const {
        throw std::logic_error("BookingService::exportRecords chua lam");
    }
    void BookingService::importBooking(const BookingRecord&, std::shared_ptr<Showtime>) {
        throw std::logic_error("BookingService::importBooking chua lam");
    }

}  