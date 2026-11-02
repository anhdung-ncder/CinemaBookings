#include "cinema/booking/CardPayment.h"
#include <stdexcept>

namespace cinema {

    CardPayment::CardPayment(const Money& limit) : limit_(limit) {}

    std::string CardPayment::code() const { return "CARD"; }
    std::string CardPayment::name() const { return "Credit card"; }

    Money CardPayment::feeFor(const Money&) const {
        throw std::logic_error("CardPayment::feeFor chua lam");
    }
    void CardPayment::charge(const Money&) {
        throw std::logic_error("CardPayment::charge chua lam");
    }
    void CardPayment::refund(const Money&) {
        throw std::logic_error("CardPayment::refund chua lam");
    }

}  