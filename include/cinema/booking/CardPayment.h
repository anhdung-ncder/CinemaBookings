#pragma once
#include "cinema/booking/IPayment.h"

namespace cinema {

    class CardPayment : public IPayment {
    public:
        explicit CardPayment(const Money& limit);             // hạn mức mỗi giao dịch

        std::string code() const override;
        std::string name() const override;
        Money feeFor(const Money& amount) const override;
        void charge(const Money& total) override;
        void refund(const Money& total) override;

    private:
        Money limit_;
    };

} 