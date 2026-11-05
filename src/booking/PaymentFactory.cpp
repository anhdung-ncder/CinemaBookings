#include "cinema/booking/PaymentFactory.h"
#include <memory>
#include "cinema/booking/BookingErrors.h"
#include "cinema/booking/CardPayment.h"
#include "cinema/booking/CashPayment.h"
#include "cinema/booking/WalletPayment.h"

namespace cinema {

    std::shared_ptr<IPayment> PaymentFactory::create(const std::string& code) {
        return create(code, Money(10000000));                            // funds mặc định 10,000,000 VND
    }

    std::shared_ptr<IPayment> PaymentFactory::create(const std::string& code, const Money& funds) {
        if (code == "CASH")   return std::make_shared<CashPayment>(funds);
        if (code == "CARD")   return std::make_shared<CardPayment>(funds);
        if (code == "WALLET") return std::make_shared<WalletPayment>(funds);
        throw BookingError("Unknown payment code: " + code);             // chữ thường cũng bị từ chối
    }

}  