#pragma once
#include <memory>
#include <string>
#include "cinema/booking/IPayment.h"

namespace cinema {

    class PaymentFactory {
    public:
        // Mặc định funds = 10,000,000 VNĐ
        static std::shared_ptr<IPayment> create(const std::string& code);
        static std::shared_ptr<IPayment> create(const std::string& code, const Money& funds);
    };

} 