#include "cinema/booking/CardPayment.h"
#include "cinema/booking/BookingErrors.h"

namespace cinema {

    CardPayment::CardPayment(const Money& limit) : limit_(limit) {}

    std::string CardPayment::code() const { return "CARD"; }
    std::string CardPayment::name() const { return "Credit card"; }

    Money CardPayment::feeFor(const Money& amount) const {
        return amount.scaledPercent(2);                                  // phí 2% trên tiền hàng
    }

    void CardPayment::charge(const Money& total) {
        if (total > limit_) {                                            
            throw PaymentFailedError("Amount exceeds card limit");
        }
    }

    void CardPayment::refund(const Money&) {}

}  