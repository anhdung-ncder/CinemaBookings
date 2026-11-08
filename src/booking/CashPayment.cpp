#include "cinema/booking/CashPayment.h"
#include "cinema/booking/BookingErrors.h"

namespace cinema {

    CashPayment::CashPayment(const Money& tendered) : tendered_(tendered) {}

    std::string CashPayment::code() const { return "CASH"; }
    std::string CashPayment::name() const { return "Cash"; }

    Money CashPayment::feeFor(const Money&) const { return Money(0); }   // tiền mặt không có phí

    void CashPayment::charge(const Money& total) {
        if (tendered_ < total) {                                         
            throw PaymentFailedError("Cash tendered is less than total");
        }
    }

    void CashPayment::refund(const Money&) {}                            

}  