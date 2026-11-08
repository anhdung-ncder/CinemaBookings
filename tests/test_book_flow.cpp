#include <iostream>
#include <memory>
#include <vector>
#include "cinema/booking/BookingErrors.h"
#include "cinema/booking/BookingService.h"
#include "cinema/booking/PaymentFactory.h"
#include "cinema/catalog/DateTime.h"
#include "cinema/catalog/Movie.h"
#include "cinema/catalog/Showtime.h"
#include "cinema/common/Money.h"
#include "cinema/common/SeatId.h"
#include "cinema/seating/Room.h"
#include "cinema/seating/SeatErrors.h"
#include "cinema/seating/SeatTypes.h"

using namespace cinema;

namespace {
    int g_total = 0;
    int g_failed = 0;
}

#define CHECK(cond)                                                              \
    do {                                                                         \
        ++g_total;                                                               \
        if (!(cond)) { ++g_failed; std::cerr << "FAIL dong " << __LINE__         \
                                             << ": " #cond "\n"; }               \
    } while (0)

#define CHECK_THROWS_AS(expr, ExType)                                            \
    do {                                                                         \
        ++g_total;                                                               \
        bool ok = false;                                                         \
        try { (void)(expr); } catch (const ExType&) { ok = true; } catch (...) {}\
        if (!ok) { ++g_failed; std::cerr << "FAIL dong " << __LINE__             \
                                         << ": can nem " #ExType "\n"; }         \
    } while (0)

int main() {
    // Du lieu mau (muc 10 hop dong): phong R1 gom 21 ghe, suat S1
    auto movie = std::make_shared<const Movie>("M1", "Space Odyssey", 120, "SciFi");
    auto room = Room::fromLayout("R1", "Room 1", Money(80000), "SSSSSS|SSSSSS|VVVVVV|CCC");
    auto showtime = std::make_shared<Showtime>("S1", movie, room, DateTime(2026, 12, 1, 18, 0));
    BookingService service;

    const SeatId A1('A', 1), A2('A', 2), A3('A', 3), C1('C', 1), C2('C', 2), D1('D', 1);

    // E1: CASH, A1 + A2 -> BK0001, 160,000, 2 ve
    {
        const std::vector<SeatId> ids = { A1, A2 };
        auto b = service.book(showtime, ids, PaymentFactory::create("CASH"), "An");
        CHECK(b->id() == "BK0001");
        CHECK(b->total() == Money(160000));
        CHECK(b->tickets().size() == 2);
        CHECK(b->tickets()[0].ticketId() == "BK0001-A1");
        CHECK(b->tickets()[0].price() == Money(80000));
        CHECK(showtime->seatMap().stateOf(A1) == SeatState::Sold);
        CHECK(showtime->seatMap().stateOf(A2) == SeatState::Sold);
    }

    // E2: A2 da ban -> SeatTakenError, A3 van Available, all().size() == 1
    {
        const std::vector<SeatId> ids = { A2, A3 };
        CHECK_THROWS_AS(service.book(showtime, ids, PaymentFactory::create("CASH"), "Binh"),
            SeatTakenError);
        CHECK(showtime->seatMap().stateOf(A3) == SeatState::Available);
        CHECK(service.all().size() == 1);
    }

    // E4: vi 100,000 dat C1 (120,000) -> PaymentFailedError, C1 van Available
    {
        const std::vector<SeatId> ids = { C1 };
        CHECK_THROWS_AS(service.book(showtime, ids,
            PaymentFactory::create("WALLET", Money(100000)), "Binh"),
            PaymentFailedError);
        CHECK(showtime->seatMap().stateOf(C1) == SeatState::Available);
        CHECK(service.all().size() == 1);
    }

    // E6: CARD, C1 + C2 -> BK0002, 240,000 + 4,800 = 244,800
    {
        const std::vector<SeatId> ids = { C1, C2 };
        auto b = service.book(showtime, ids, PaymentFactory::create("CARD"), "Chi");
        CHECK(b->id() == "BK0002");
        CHECK(b->total() == Money(244800));
        CHECK(showtime->seatMap().stateOf(C1) == SeatState::Sold);
        CHECK(showtime->seatMap().stateOf(C2) == SeatState::Sold);
    }

    // E7: CASH, D1 (ghe Couple) -> BK0003, 144,000
    {
        const std::vector<SeatId> ids = { D1 };
        auto b = service.book(showtime, ids, PaymentFactory::create("CASH"), "Dung");
        CHECK(b->id() == "BK0003");
        CHECK(b->total() == Money(144000));
    }

    std::cout << (g_total - g_failed) << "/" << g_total << " checks passed\n";
    return g_failed == 0 ? 0 : 1;
}