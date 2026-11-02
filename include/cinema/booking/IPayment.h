#pragma once
#include <string>
#include "cinema/common/Money.h"

namespace cinema {

    class IPayment {
    public:
        virtual ~IPayment() = default;                       

        virtual std::string code() const = 0;                // "CASH" | "CARD" | "WALLET"
        virtual std::string name() const = 0;                
        virtual Money feeFor(const Money& amount) const = 0; 
        virtual void charge(const Money& total) = 0;         
        virtual void refund(const Money& total) = 0;         
    };

}  