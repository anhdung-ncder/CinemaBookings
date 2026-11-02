#include "cinema/booking/WalletPayment.h"
#include <stdexcept>

namespace cinema {

    WalletPayment::WalletPayment(const Money& balance) : balance_(balance) {}

    std::string WalletPayment::code() const { return "WALLET"; }
    std::string WalletPayment::name() const { return "E-wallet"; }

    Money WalletPayment::feeFor(const Money&) const {
        throw std::logic_error("WalletPayment::feeFor chua lam");
    }
    void WalletPayment::charge(const Money&) {
        throw std::logic_error("WalletPayment::charge chua lam");
    }
    void WalletPayment::refund(const Money&) {
        throw std::logic_error("WalletPayment::refund chua lam");
    }

    Money WalletPayment::balance() const { return balance_; }

}  