#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
#include "cinema/booking/BookingErrors.h"
#include "cinema/booking/BookingService.h"
#include "cinema/booking/PaymentFactory.h"
#include "cinema/catalog/CatalogErrors.h"
#include "cinema/catalog/DateTime.h"
#include "cinema/catalog/Movie.h"
#include "cinema/catalog/Showtime.h"
#include "cinema/common/Money.h"
#include "cinema/common/SeatId.h"
#include "cinema/seating/Room.h"
#include "cinema/seating/SeatErrors.h"
#include "cinema/seating/SeatTypes.h"

using namespace cinema;

static_assert(std::is_default_constructible<BookingService>::value,
    "BookingService phai khoi tao mac dinh duoc");
static_assert(std::is_move_constructible<BookingService>::value,
    "BookingService phai di chuyen duoc");

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
    auto movie = std::make_shared<const Movie>("M1", "Space Odyssey", 120, "SciFi");
    auto room = Room::fromLayout("R1", "Room 1", Money(80000), "SSSSSS|SSSSSS|VVVVVV|CCC");
    auto showtime = std::make_shared<Showtime>("S1", movie, room, DateTime(2026, 12, 1, 18, 0));
    auto closedShowtime =
        std::make_shared<Showtime>("S2", movie, room, DateTime(2026, 12, 1, 21, 0), false);
    BookingService service;

    const SeatId A1('A', 1), A2('A', 2), A3('A', 3), B1('B', 1);
    const SeatId C1('C', 1), C2('C', 2), D1('D', 1), Z9('Z', 9);

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
    }

    // E7: CASH, D1 (Couple) -> BK0003, 144,000
    {
        const std::vector<SeatId> ids = { D1 };
        auto b = service.book(showtime, ids, PaymentFactory::create("CASH"), "Dung");
        CHECK(b->id() == "BK0003");
        CHECK(b->total() == Money(144000));
    }

    // E3: suat S2 da dong -> ShowtimeClosedError, ghe khong doi
    {
        const std::vector<SeatId> ids = { A1 };
        CHECK_THROWS_AS(service.book(closedShowtime, ids, PaymentFactory::create("CASH"), "An"),
            ShowtimeClosedError);
        CHECK(closedShowtime->seatMap().stateOf(A1) == SeatState::Available);
    }

    // E5: ghe Z9 khong ton tai -> SeatNotFoundError, khong tao booking
    {
        const std::vector<SeatId> ids = { Z9 };
        CHECK_THROWS_AS(service.book(showtime, ids, PaymentFactory::create("CASH"), "An"),
            SeatNotFoundError);
        CHECK(service.all().size() == 3);
    }

    // Truy van
    CHECK(service.get("BK0002")->customer() == "Chi");
    CHECK_THROWS_AS(service.get("BK9999"), BookingError);

    // E8: huy BK0001 -> hoan 160,000; A1, A2 Available; Sold = 3, Available = 18
    {
        const Money refunded = service.cancel("BK0001");
        CHECK(refunded == Money(160000));
        CHECK(service.get("BK0001")->status() == BookingStatus::Cancelled);
        CHECK(service.all().size() == 3);              // booking huy van nam trong all()
        CHECK(showtime->seatMap().stateOf(A1) == SeatState::Available);
        CHECK(showtime->seatMap().stateOf(A2) == SeatState::Available);
        CHECK(showtime->seatMap().count(SeatState::Sold) == 3);
        CHECK(showtime->seatMap().count(SeatState::Available) == 18);
    }

    // E9: huy lan hai va huy id khong ton tai -> BookingError
    CHECK_THROWS_AS(service.cancel("BK0001"), BookingError);
    CHECK_THROWS_AS(service.cancel("BK9999"), BookingError);

    // exportRecords roi importBooking vao BookingService moi (E10: id ke tiep = BK0004)
    {
        const std::vector<BookingRecord> records = service.exportRecords();
        CHECK(records.size() == 3);
        CHECK(records[0].bookingId == "BK0001");
        CHECK(records[0].status == BookingStatus::Cancelled);
        CHECK(records[0].total == Money(160000));
        CHECK(records[1].paymentCode == "CARD");

        auto showtime2 = std::make_shared<Showtime>("S1", movie, room, DateTime(2026, 12, 1, 18, 0));
        BookingService service2;
        for (const BookingRecord& record : records) {
            service2.importBooking(record, showtime2);
        }
        CHECK(service2.all().size() == 3);
        CHECK(showtime2->seatMap().count(SeatState::Sold) == 3);
        CHECK(showtime2->seatMap().stateOf(A1) == SeatState::Available);  // BK0001 da huy: khong dung ghe
        CHECK(showtime2->seatMap().stateOf(C1) == SeatState::Sold);
        CHECK(service2.get("BK0001")->status() == BookingStatus::Cancelled);
        CHECK(service2.get("BK0002")->tickets().size() == 2);
        CHECK_THROWS_AS(service2.importBooking(records[0], showtime2), BookingError);  // trung id

        const std::vector<SeatId> ids = { B1 };
        auto b = service2.book(showtime2, ids, PaymentFactory::create("CASH"), "Em");
        CHECK(b->id() == "BK0004");

        BookingService moved(std::move(service2));       // di chuyen duoc
        CHECK(moved.all().size() == 4);
    }

    // Huy roi dat lai dung ghe do duoc
    {
        const std::vector<SeatId> ids = { A1, A2 };
        auto b = service.book(showtime, ids, PaymentFactory::create("CASH"), "Gia");
        CHECK(b->id() == "BK0004");
        CHECK(showtime->seatMap().stateOf(A1) == SeatState::Sold);
    }

    std::cout << (g_total - g_failed) << "/" << g_total << " checks passed\n";
    return g_failed == 0 ? 0 : 1;
}