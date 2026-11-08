#include "cinema/booking/BookingService.h"
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include "cinema/booking/BookingErrors.h"

namespace cinema {

    namespace {
        // 1 -> "BK0001", 12 -> "BK0012"
        std::string makeBookingId(int number) {
            std::ostringstream out;
            out << "BK" << std::setw(4) << std::setfill('0') << number;
            return out.str();
        }
    }  // namespace

    BookingService::BookingService() = default;

    std::shared_ptr<const Booking> BookingService::book(
        std::shared_ptr<Showtime> showtime,
        const std::vector<SeatId>& seatIds,
        std::shared_ptr<IPayment> payment,
        const std::string& customer) {

        // kiểm tra đầu vào (chưa đụng tới ghế) 
        if (!showtime)        throw BookingError("showtime is null");
        if (!payment)         throw BookingError("payment is null");
        if (seatIds.empty())  throw BookingError("no seats selected");
        if (customer.empty()) throw BookingError("customer name is empty");

        //giữ ghế (all-or-nothing; nếu ném thì chưa có gì bị giữ) 
        showtime->holdSeats(seatIds);

        bool charged = false;      // đã thu tiền chưa
        Money total;               // số tiền đã thu (để refund đúng số)

        try {
            
            const Money quote = showtime->quote(seatIds);
            const Money fee = payment->feeFor(quote);
            total = quote + fee;

            // thu tiền (không đủ -> PaymentFailedError)
            payment->charge(total);
            charged = true;

            //  xác nhận ghế
            showtime->confirmSeats(seatIds);

            //  tạo Booking + Ticket từng ghế, lưu, tăng bộ đếm id
            const std::string id = makeBookingId(nextNumber_);

            std::vector<Ticket> tickets;
            for (const SeatId& seat : seatIds) {
                const Money seatPrice = showtime->quote(std::vector<SeatId>{seat});
                tickets.push_back(Ticket(id, showtime->id(), seat, seatPrice));
            }

            
            std::shared_ptr<Booking> booking(new Booking(
                id, showtime, seatIds, customer, total, payment, payment->code(), tickets));

            bookings_.push_back(booking);
            ++nextNumber_;                         // chỉ tăng khi đã lưu thành công
            return booking;
        }
        catch (...) {
           
            if (charged) {
                payment->refund(total);            // đã thu tiền thì hoàn lại trước
            }
            showtime->releaseSeats(seatIds);       // rồi trả ghế về Available
            throw;                                 // ném lại lỗi gốc, không nuốt lỗi
        }
    }

    std::shared_ptr<const Booking> BookingService::get(const std::string& bookingId) const {
        for (const auto& booking : bookings_) {
            if (booking->id() == bookingId) {
                return booking;
            }
        }
        throw BookingError("booking not found: " + bookingId);
    }

    std::vector<std::shared_ptr<const Booking>> BookingService::all() const {
        return std::vector<std::shared_ptr<const Booking>>(bookings_.begin(), bookings_.end());
    }

    Money BookingService::cancel(const std::string&) {
        throw std::logic_error("BookingService::cancel chua lam");
    }
    std::vector<BookingRecord> BookingService::exportRecords() const {
        throw std::logic_error("BookingService::exportRecords chua lam");
    }
    void BookingService::importBooking(const BookingRecord&, std::shared_ptr<Showtime>) {
        throw std::logic_error("BookingService::importBooking chua lam");
    }

} 