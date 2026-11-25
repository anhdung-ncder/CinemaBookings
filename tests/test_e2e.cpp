#include "check.h"

#include "cinema/app/DataStore.h"
#include "cinema/app/Reports.h"
#include "cinema/booking/BookingErrors.h"
#include "cinema/booking/BookingService.h"
#include "cinema/booking/PaymentFactory.h"
#include "cinema/catalog/CatalogErrors.h"
#include "cinema/catalog/Cinema.h"
#include "cinema/catalog/DateTime.h"
#include "cinema/catalog/Movie.h"
#include "cinema/catalog/Showtime.h"
#include "cinema/common/Money.h"
#include "cinema/common/SeatId.h"
#include "cinema/seating/Room.h"
#include "cinema/seating/SeatErrors.h"
#include "cinema/seating/SeatTypes.h"

#include <cstdio>
#include <fstream>
#include <string>

using namespace cinema;

namespace {

    void testDateTime() {
        CHECK_NOTHROW(DateTime(2024, 2, 29, 18, 0));
        CHECK_THROWS_AS(DateTime(2023, 2, 29, 18, 0), std::invalid_argument);
        CHECK_THROWS_AS(DateTime(2024, 12, 1, 24, 0), std::invalid_argument);

        DateTime dt = DateTime::parse("2026-12-03 18:04");
        CHECK(dt.year() == 2026);
        CHECK(dt.hour() == 18);
    }

    void testMovie() {
        CHECK_THROWS_AS(Movie("M1", "Fast and Furious", 0, "Actions"), std::invalid_argument);
        CHECK_THROWS_AS(Movie("M1", "Fast and Furious", 601, "Actions"), std::invalid_argument);

        Movie m("M1", "Avatar", 180, "Sci-Fi");
        CHECK(m.title() == "Avatar");
    }

    void testCinema() {
        Cinema c;
        CHECK_THROWS_AS(c.movie("M_UNKNOWN"), NotFoundError);
    }

    void writeCsv(const std::string& path, const std::string& content) {
        std::ofstream out(path);
        CHECK(out.is_open());
        out << content;
    }

    void testE2EScenariosE1toE12() {
        auto cleanup = []() {
            std::remove("movies.csv");
            std::remove("rooms.csv");
            std::remove("showtimes.csv");
            std::remove("bookings.csv");
        };
        cleanup();

        // Chuan bi 4 file du lieu mau chuan Muc 10 hop dong
        writeCsv("movies.csv",
            "movieId,title,durationMinutes,genre\n"
            "M1,Space Odyssey,120,SciFi\n");
        writeCsv("rooms.csv",
            "roomId,name,standardPrice,layout\n"
            "R1,Room 1,80000,SSSSSS|SSSSSS|VVVVVV|CCC\n");
        writeCsv("showtimes.csv",
            "showtimeId,movieId,roomId,startDateTime,status\n"
            "S1,M1,R1,2026-12-01 18:00,OPEN\n"
            "S2,M1,R1,2026-12-01 21:00,CLOSED\n");
        writeCsv("bookings.csv",
            "bookingId,showtimeId,customer,seats,total,paymentCode,status\n");

        DataStore store(".");
        Cinema cinema;
        BookingService service;
        store.load(cinema, service);

        auto s1 = cinema.showtime("S1");
        auto s2 = cinema.showtime("S2");

        const SeatId A1('A', 1), A2('A', 2), A3('A', 3);
        const SeatId C1('C', 1), C2('C', 2), D1('D', 1), Z9('Z', 9);

        // E1: CASH mac dinh, A1 + A2 -> BK0001, 160,000 VND, 2 ve, A1/A2 Sold
        {
            auto b1 = service.book(s1, { A1, A2 }, PaymentFactory::create("CASH"), "An");
            CHECK(b1->id() == "BK0001");
            CHECK(b1->total() == Money(160000));
            CHECK(b1->tickets().size() == 2);
            CHECK(b1->tickets()[0].ticketId() == "BK0001-A1");
            CHECK(b1->tickets()[0].price() == Money(80000));
            CHECK(b1->tickets()[1].ticketId() == "BK0001-A2");
            CHECK(b1->tickets()[1].price() == Money(80000));
            CHECK(s1->seatMap().stateOf(A1) == SeatState::Sold);
            CHECK(s1->seatMap().stateOf(A2) == SeatState::Sold);
        }

        // E2: A2 da ban, dat [A2, A3] -> SeatTakenError(A2), A3 van Available, all().size() == 1
        {
            bool caught = false;
            try {
                service.book(s1, { A2, A3 }, PaymentFactory::create("CASH"), "Binh");
            }
            catch (const SeatTakenError& e) {
                caught = true;
                CHECK(e.seatId() == A2);
            }
            CHECK(caught);
            CHECK(s1->seatMap().stateOf(A3) == SeatState::Available);
            CHECK(service.all().size() == 1);
        }

        // E3: Dat suat S2 dang dong -> ShowtimeClosedError, khong ghe nao doi
        {
            bool caught = false;
            try {
                service.book(s2, { A1 }, PaymentFactory::create("CASH"), "Binh");
            }
            catch (const ShowtimeClosedError& e) {
                caught = true;
                CHECK(e.showtimeId() == "S2");
            }
            CHECK(caught);
            CHECK(s2->seatMap().count(SeatState::Available) == 21);
        }

        // E4: Vi 100,000 dat C1 (120,000) -> PaymentFailedError, C1 van Available
        {
            CHECK_THROWS_AS(
                service.book(s1, { C1 }, PaymentFactory::create("WALLET", Money(100000)), "Binh"),
                PaymentFailedError);
            CHECK(s1->seatMap().stateOf(C1) == SeatState::Available);
            CHECK(service.all().size() == 1);
        }

        // E5: Dat ghe khong ton tai Z9 -> SeatNotFoundError
        {
            CHECK_THROWS_AS(
                service.book(s1, { Z9 }, PaymentFactory::create("CASH"), "Binh"),
                SeatNotFoundError);
            CHECK(service.all().size() == 1);
        }

        // E6: CARD mac dinh, C1 + C2 -> BK0002, 240,000 + 4,800 = 244,800 VND
        {
            auto b2 = service.book(s1, { C1, C2 }, PaymentFactory::create("CARD"), "Chi");
            CHECK(b2->id() == "BK0002");
            CHECK(b2->total() == Money(244800));
            CHECK(s1->seatMap().stateOf(C1) == SeatState::Sold);
            CHECK(s1->seatMap().stateOf(C2) == SeatState::Sold);
        }

        // E7: CASH mac dinh, D1 (Couple, capacity = 2) -> BK0003, 144,000 VND
        {
            CHECK(s1->room().seat(D1).capacity() == 2);
            auto b3 = service.book(s1, { D1 }, PaymentFactory::create("CASH"), "Dung");
            CHECK(b3->id() == "BK0003");
            CHECK(b3->total() == Money(144000));
            CHECK(s1->seatMap().stateOf(D1) == SeatState::Sold);
        }

        // E8: cancel("BK0001") -> hoan 160,000 VND; BK0001 Cancelled; Sold = 3, Available = 18
        {
            Money refunded = service.cancel("BK0001");
            CHECK(refunded == Money(160000));
            CHECK(service.get("BK0001")->status() == BookingStatus::Cancelled);
            CHECK(service.all().size() == 3);
            CHECK(s1->seatMap().stateOf(A1) == SeatState::Available);
            CHECK(s1->seatMap().stateOf(A2) == SeatState::Available);
            CHECK(s1->seatMap().count(SeatState::Sold) == 3);
            CHECK(s1->seatMap().count(SeatState::Available) == 18);
        }

        // E9: cancel("BK0001") lan nua -> BookingError, khong doi gi
        {
            CHECK_THROWS_AS(service.cancel("BK0001"), BookingError);
            CHECK(s1->seatMap().count(SeatState::Sold) == 3);
            CHECK(s1->seatMap().count(SeatState::Available) == 18);
        }

        // E12: Doanh thu phim M1 sau E8 = 388,800 VND (244,800 + 144,000)
        {
            auto rev = Reports::revenueByMovie(service);
            CHECK(rev.size() == 1);
            CHECK(rev.at("M1") == Money(388800));
        }

        // E10: Luu 4 file, tao chuong trinh moi, nap lai -> y het, booking tiep theo la BK0004
        store.save(cinema, service);

        Cinema cinema2;
        BookingService service2;
        store.load(cinema2, service2);

        auto s1Reloaded = cinema2.showtime("S1");
        CHECK(s1Reloaded->seatMap().count(SeatState::Sold) == 3);
        CHECK(s1Reloaded->seatMap().stateOf(C1) == SeatState::Sold);
        CHECK(s1Reloaded->seatMap().stateOf(C2) == SeatState::Sold);
        CHECK(s1Reloaded->seatMap().stateOf(D1) == SeatState::Sold);
        CHECK(s1Reloaded->seatMap().stateOf(A1) == SeatState::Available);
        CHECK(service2.all().size() == 3);
        CHECK(service2.get("BK0001")->status() == BookingStatus::Cancelled);

        auto b4 = service2.book(s1Reloaded, { A1 }, PaymentFactory::create("CASH"), "Em");
        CHECK(b4->id() == "BK0004");

        // E11: Nap file co 1 dong hong (A1;;A2) -> FileError co ten file & so dong, trang thai khong doi
        {
            std::ofstream bad("bookings.csv", std::ios::app);
            bad << "BK0005,S1,Loi,A1;;A2,160000,CASH,CONFIRMED\n";
        }
        {
            bool caught = false;
            try {
                store.load(cinema2, service2);
            }
            catch (const FileError& e) {
                caught = true;
                const std::string msg = e.what();
                CHECK(msg.find("bookings.csv") != std::string::npos);
                CHECK(msg.find(":5:") != std::string::npos);
            }
            CHECK(caught);
            CHECK(service2.all().size() == 4);
            CHECK(cinema2.showtime("S1")->seatMap().stateOf(A1) == SeatState::Sold);
        }

        cleanup();
    }

} // namespace

int main() {
    testDateTime();
    testMovie();
    testCinema();
    testE2EScenariosE1toE12();

    return check_summary();
}
