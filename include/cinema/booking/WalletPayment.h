#pragma once
#include "cinema/booking/IPayment.h"

namespace cinema {

    class WalletPayment : public IPayment {
    public:
        explicit WalletPayment(const Money& balance);         // số dư ví

        std::string code() const override;
        std::string name() const override;
        Money feeFor(const Money& amount) const override;
        void charge(const Money& total) override;
        void refund(const Money& total) override;

        Money balance() const;                                

    private:
        Money balance_;
    };

}  