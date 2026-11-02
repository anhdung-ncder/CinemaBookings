#include "cinema/booking/CashPayment.h"
#include <stdexcept>

namespace cinema {

    CashPayment::CashPayment(const Money& tendered) : tendered_(tendered) {}

    std::string CashPayment::code() const { return "CASH"; }
    std::string CashPayment::name() const { return "Cash"; }

    Money CashPayment::feeFor(const Money&) const {
        throw std::logic_error("CashPayment::feeFor chua lam");
    }
    void CashPayment::charge(const Money&) {
        throw std::logic_error("CashPayment::charge chua lam");
    }
    void CashPayment::refund(const Money&) {
        throw std::logic_error("CashPayment::refund chua lam");
    }

}  