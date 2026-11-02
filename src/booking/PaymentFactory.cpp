#include "cinema/booking/PaymentFactory.h"
#include <stdexcept>

namespace cinema {

    std::shared_ptr<IPayment> PaymentFactory::create(const std::string&) {
        throw std::logic_error("PaymentFactory::create chua lam");
    }
    std::shared_ptr<IPayment> PaymentFactory::create(const std::string&, const Money&) {
        throw std::logic_error("PaymentFactory::create chua lam");
    }

}  