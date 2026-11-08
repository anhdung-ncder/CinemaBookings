#include "cinema/booking/BookingService.h"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include "cinema/booking/BookingErrors.h"
#include "cinema/booking/PaymentFactory.h"

namespace cinema {

    namespace {

        // 1 -> "BK0001", 12 -> "BK0012"
        std::string makeBookingId(int number) {
            std::ostringstream out;
            out << "BK" << std::setw(4) << std::setfill('0') << number;
            return out.str();
        }

        // "BK0004" -> 4; không đúng dạng "BK" + chữ số -> 0
        int parseBookingNumber(const std::string& id) {
            if (id.size() < 3 || id.size() > 11 || id.compare(0, 2, "BK") != 0) {
                return 0;
            }
            for (std::size_t i = 2; i < id.size(); ++i) {
                if (!std::isdigit(static_cast<unsigned char>(id[i]))) {
                    return 0;
                }
            }
            return std::stoi(id.substr(2));
        }

        // Mỗi ghế một vé, giá vé = giá của ghế đó
        std::vector<Ticket> makeTickets(const std::string& bookingId,
            const std::shared_ptr<Showtime>& showtime,
            const std::vector<SeatId>& seatIds) {
            std::vector<Ticket> tickets;
            for (const SeatId& seat : seatIds) {
                const Money price = showtime->quote(std::vector<SeatId>{seat});
                tickets.push_back(Ticket(bookingId, showtime->id(), seat, price));
            }
            return tickets;
        }

    }  // namespace

    BookingService::BookingService() = default;

    
    std::shared_ptr<const Booking> BookingService::book(
        std::shared_ptr<Showtime> showtime,
        const std::vector<SeatId>& seatIds,
        std::shared_ptr<IPayment> payment,
        const std::string& customer) {

        if (!showtime)        throw BookingError("showtime is null");
        if (!payment)         throw BookingError("payment is null");
        if (seatIds.empty())  throw BookingError("no seats selected");
        if (customer.empty()) throw BookingError("customer name is empty");

        showtime->holdSeats(seatIds);              

        bool charged = false;
        Money total;

        try {
            const Money quote = showtime->quote(seatIds);
            const Money fee = payment->feeFor(quote);
            total = quote + fee;

            payment->charge(total);
            charged = true;

            showtime->confirmSeats(seatIds);

            const std::string id = makeBookingId(nextNumber_);
            const std::vector<Ticket> tickets = makeTickets(id, showtime, seatIds);

            std::shared_ptr<Booking> booking(new Booking(
                id, showtime, seatIds, customer, total, payment, payment->code(), tickets));

            bookings_.push_back(booking);
            ++nextNumber_;                         // chỉ tăng khi đã lưu thành công
            return booking;
        }
        catch (...) {
            if (charged) {
                payment->refund(total);
            }
            showtime->releaseSeats(seatIds);
            throw;
        }
    }

   
    Money BookingService::cancel(const std::string& bookingId) {
        std::shared_ptr<Booking> booking;
        for (const auto& item : bookings_) {
            if (item->id() == bookingId) {
                booking = item;
                break;
            }
        }
        if (!booking) {
            throw BookingError("booking not found: " + bookingId);
        }
        if (booking->status_ == BookingStatus::Cancelled) {
            throw BookingError("booking already cancelled: " + bookingId);
        }

        const Money refundAmount = booking->total_;

        // Hoàn tiền TRƯỚC: nếu ném ở đây thì booking vẫn Confirmed, ghế vẫn Sold.
        booking->payment_->refund(refundAmount);

        try {
            booking->showtime_->releaseSeats(booking->seatIds_);
        }
        catch (...) {
            booking->payment_->charge(refundAmount);   // bù: thu lại khoản vừa hoàn
            throw;
        }

        booking->status_ = BookingStatus::Cancelled;   // bước cuối cùng, không ném
        return refundAmount;
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

    std::vector<BookingRecord> BookingService::exportRecords() const {
        std::vector<BookingRecord> records;
        for (const auto& booking : bookings_) {        // theo thứ tự tạo
            BookingRecord record;
            record.bookingId = booking->id();
            record.showtimeId = booking->showtime().id();
            record.seatIds = booking->seatIds();
            record.customer = booking->customer();
            record.total = booking->total();
            record.paymentCode = booking->paymentCode();
            record.status = booking->status();
            records.push_back(record);
        }
        return records;
    }

    
    void BookingService::importBooking(const BookingRecord& record,
        std::shared_ptr<Showtime> showtime) {
        if (!showtime) {
            throw BookingError("showtime is null");
        }
        for (const auto& item : bookings_) {
            if (item->id() == record.bookingId) {
                throw BookingError("duplicate booking id: " + record.bookingId);
            }
        }

        // Dựng mọi thứ ở biến cục bộ trước; chưa đổi trạng thái chung.
        std::shared_ptr<IPayment> payment = PaymentFactory::create(record.paymentCode);
        const std::vector<Ticket> tickets = makeTickets(record.bookingId, showtime, record.seatIds);

        std::shared_ptr<Booking> booking(new Booking(
            record.bookingId, showtime, record.seatIds, record.customer,
            record.total, payment, record.paymentCode, tickets));
        booking->status_ = record.status;

        bookings_.reserve(bookings_.size() + 1);       // để push_back phía dưới không thể ném

        if (record.status == BookingStatus::Confirmed) {
            showtime->restoreSoldSeats(record.seatIds);    // Cancelled thì không đụng ghế
        }

        bookings_.push_back(booking);
        nextNumber_ = std::max(nextNumber_, parseBookingNumber(record.bookingId) + 1);
    }

}  