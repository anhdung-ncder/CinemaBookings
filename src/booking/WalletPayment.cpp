#include "cinema/booking/WalletPayment.h"
#include "cinema/booking/BookingErrors.h"

namespace cinema {

    WalletPayment::WalletPayment(const Money& balance) : balance_(balance) {}

    std::string WalletPayment::code() const { return "WALLET"; }
    std::string WalletPayment::name() const { return "E-wallet"; }

    Money WalletPayment::feeFor(const Money&) const { return Money(0); }

    void WalletPayment::charge(const Money& total) {
        if (balance_ < total) {                                          // kiểm tra TRƯỚC khi trừ
            throw PaymentFailedError("Wallet balance is not enough");
        }
        balance_ -= total;
    }

    void WalletPayment::refund(const Money& total) {
        balance_ += total;
    }

    Money WalletPayment::balance() const { return balance_; }

}  