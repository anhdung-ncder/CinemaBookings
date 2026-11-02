#pragma once
#include <memory>
#include <string>
#include <vector>
#include "cinema/booking/IPayment.h"
#include "cinema/booking/Ticket.h"
#include "cinema/catalog/Showtime.h"
#include "cinema/common/Money.h"
#include "cinema/common/SeatId.h"

namespace cinema {

    enum class BookingStatus { Confirmed, Cancelled };
    std::string toString(BookingStatus status);            // "CONFIRMED" | "CANCELLED"

    class BookingService;

    class Booking {
    public:
        const std::string& id() const;
        const Showtime& showtime() const;
        const std::vector<SeatId>& seatIds() const;
        const std::string& customer() const;
        Money total() const;
        const std::string& paymentCode() const;
        BookingStatus status() const;
        const std::vector<Ticket>& tickets() const;

    private:
        friend class BookingService;                       // chỉ BookingService được tạo và đổi trạng thái
        Booking(const std::string& id,
            std::shared_ptr<Showtime> showtime,
            const std::vector<SeatId>& seatIds,
            const std::string& customer,
            const Money& total,
            std::shared_ptr<IPayment> payment,
            const std::string& paymentCode,
            const std::vector<Ticket>& tickets);

        std::string id_;
        std::shared_ptr<Showtime> showtime_;
        std::vector<SeatId> seatIds_;
        std::string customer_;
        Money total_;
        std::shared_ptr<IPayment> payment_;                // để hoàn tiền khi hủy
        std::string paymentCode_;                          // lưu chuỗi, không trả tham chiếu tới giá trị tạm
        BookingStatus status_ = BookingStatus::Confirmed;
        std::vector<Ticket> tickets_;
    };

}  