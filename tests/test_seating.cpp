// Test goi A (seating): SeatId, cay ghe, SeatFactory, Room, SeatMap.
// Chi dung API cong khai trong hop dong. Moi ca loi deu kiem tra TRANG THAI SAU LOI.
#include "check.h"

#include "cinema/common/Money.h"
#include "cinema/common/SeatId.h"
#include "cinema/seating/Room.h"
#include "cinema/seating/Seat.h"
#include "cinema/seating/SeatErrors.h"
#include "cinema/seating/SeatFactory.h"
#include "cinema/seating/SeatMap.h"

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace cinema;

namespace {

    const Money kStandardPrice(80000);
    const char* const kLayout = "SSSSSS|SSSSSS|VVVVVV|CCC";  // 21 ghe: A,B Standard; C VIP; D Couple

    std::vector<SeatId> ids(std::initializer_list<const char*> texts) {
        std::vector<SeatId> result;
        for (const char* text : texts) {
            result.push_back(SeatId::parse(text));
        }
        return result;
    }

    SeatId id(const char* text) { return SeatId::parse(text); }

    std::shared_ptr<const Room> makeRoom() {
        return Room::fromLayout("R1", "Room 1", kStandardPrice, kLayout);
    }

    // ---------- SeatId ----------
    void testSeatId() {
        CHECK(SeatId::parse("b12").toString() == "B12");
        CHECK(SeatId::parse("A1") == SeatId('A', 1));
        CHECK(SeatId('A', 2) < SeatId('A', 10));
        CHECK(SeatId('A', 10) < SeatId('B', 1));
        CHECK(SeatId('A', 1) != SeatId('A', 2));
        CHECK_THROWS_AS(SeatId::parse("A0"), SeatNotFoundError);
        CHECK_THROWS_AS(SeatId::parse("AA1"), SeatNotFoundError);
        CHECK_THROWS_AS(SeatId::parse("1A"), SeatNotFoundError);
        CHECK_THROWS_AS(SeatId::parse(""), SeatNotFoundError);
        CHECK_THROWS_AS(SeatId('A', 100), SeatNotFoundError);
    }

    // ---------- 12.2: gia ba loai ghe ----------
    void testPrices() {
        const SeatId seatId('A', 1);
        const auto standard = SeatFactory::create('S', seatId, kStandardPrice);
        const auto vip = SeatFactory::create('V', seatId, kStandardPrice);
        const auto couple = SeatFactory::create('C', seatId, kStandardPrice);

        CHECK(standard->price() == Money(80000));
        CHECK(vip->price() == Money(120000));
        CHECK(couple->price() == Money(144000));

        CHECK(standard->capacity() == 1);
        CHECK(vip->capacity() == 1);
        CHECK(couple->capacity() == 2);

        CHECK(standard->type() == SeatType::Standard);
        CHECK(vip->type() == SeatType::Vip);
        CHECK(couple->type() == SeatType::Couple);

        CHECK(SeatFactory::create('V', SeatId('A', 5), kStandardPrice)->describe() == "A5 [VIP] 120,000 VND");

        CHECK_THROWS_AS(SeatFactory::create('X', seatId, kStandardPrice), InvalidLayoutError);
        CHECK_THROWS_AS(SeatFactory::create('s', seatId, kStandardPrice), InvalidLayoutError);
    }

    // ---------- 12.2: fromLayout ----------
    void testLayout() {
        const auto room = makeRoom();
        CHECK(room->id() == "R1");
        CHECK(room->name() == "Room 1");
        CHECK(room->seatCount() == 21);
        CHECK(room->seat(id("A1")).price() == Money(80000));
        CHECK(room->seat(id("B6")).price() == Money(80000));
        CHECK(room->seat(id("C1")).price() == Money(120000));
        CHECK(room->seat(id("D3")).price() == Money(144000));
        CHECK(room->seat(id("D1")).capacity() == 2);
        CHECK(room->hasSeat(id("D3")));
        CHECK(!room->hasSeat(id("D4")));
        CHECK_THROWS_AS(room->seat(id("Z9")), SeatNotFoundError);

        const auto seats = room->seats();
        CHECK(seats.size() == 21);
        CHECK(seats.front()->id() == id("A1"));
        CHECK(seats.back()->id() == id("D3"));
        CHECK(std::is_sorted(seats.begin(), seats.end(),
            [](const Seat* a, const Seat* b) { return a->id() < b->id(); }));

        // Layout loi
        CHECK_THROWS_AS(Room::fromLayout("R", "n", kStandardPrice, ""), InvalidLayoutError);
        CHECK_THROWS_AS(Room::fromLayout("R", "n", kStandardPrice, "SSS||VVV"), InvalidLayoutError);
        CHECK_THROWS_AS(Room::fromLayout("R", "n", kStandardPrice, "SSS|"), InvalidLayoutError);
        CHECK_THROWS_AS(Room::fromLayout("R", "n", kStandardPrice, "|SSS"), InvalidLayoutError);
        CHECK_THROWS_AS(Room::fromLayout("R", "n", kStandardPrice, "SXS"), InvalidLayoutError);

        // Gioi han 26 hang x 30 ghe
        std::string rows26;
        for (int i = 0; i < 26; ++i) {
            rows26 += (i == 0 ? "S" : "|S");
        }
        CHECK(Room::fromLayout("R", "n", kStandardPrice, rows26)->seatCount() == 26);
        CHECK_THROWS_AS(Room::fromLayout("R", "n", kStandardPrice, rows26 + "|S"), InvalidLayoutError);
        CHECK(Room::fromLayout("R", "n", kStandardPrice, std::string(30, 'S'))->seatCount() == 30);
        CHECK_THROWS_AS(Room::fromLayout("R", "n", kStandardPrice, std::string(31, 'S')), InvalidLayoutError);
    }

    void testRoomAddSeat() {
        Room room("R2", "Room 2");
        room.addSeat(SeatFactory::create('S', id("A1"), kStandardPrice));
        CHECK(room.seatCount() == 1);

        // Trung id: nem loi, phong khong doi, ghe cu con nguyen
        CHECK_THROWS_AS(room.addSeat(SeatFactory::create('V', id("A1"), kStandardPrice)), InvalidLayoutError);
        CHECK(room.seatCount() == 1);
        CHECK(room.seat(id("A1")).type() == SeatType::Standard);
    }

    // ---------- 12.3: hold ----------
    void testHold() {
        SeatMap map(makeRoom());
        CHECK(map.count(SeatState::Available) == 21);

        map.hold(ids({ "A1", "A2" }));
        CHECK(map.stateOf(id("A1")) == SeatState::Held);
        CHECK(map.stateOf(id("A2")) == SeatState::Held);

        // hold lai mot ghe: nem SeatTakenError (dung ghe vi pham), ghe con lai KHONG doi
        try {
            map.hold(ids({ "A3", "A2", "A4" }));
            CHECK(false);  // khong duoc toi day
        }
        catch (const SeatTakenError& error) {
            CHECK(error.seatId() == id("A2"));
        }
        CHECK(map.stateOf(id("A3")) == SeatState::Available);
        CHECK(map.stateOf(id("A4")) == SeatState::Available);
        CHECK(map.stateOf(id("A2")) == SeatState::Held);
        CHECK(map.count(SeatState::Held) == 2);

        // hold ghe khong ton tai: khong ghe nao doi
        CHECK_THROWS_AS(map.hold(ids({ "A3", "Z9" })), SeatNotFoundError);
        CHECK(map.stateOf(id("A3")) == SeatState::Available);
        CHECK(map.count(SeatState::Held) == 2);

        // hold ghe da Sold cung la SeatTakenError
        map.hold(ids({ "B1" }));
        map.confirm(ids({ "B1" }));
        CHECK_THROWS_AS(map.hold(ids({ "B1" })), SeatTakenError);
        CHECK(map.stateOf(id("B1")) == SeatState::Sold);
    }

    // ---------- 12.3: confirm ----------
    void testConfirm() {
        SeatMap map(makeRoom());
        map.hold(ids({ "A2" }));

        // ghe chua Held (A3 Available): khong ghe nao doi, ke ca A2 dang Held
        CHECK_THROWS_AS(map.confirm(ids({ "A3" })), InvalidSeatStateError);
        CHECK_THROWS_AS(map.confirm(ids({ "A2", "A3" })), InvalidSeatStateError);
        CHECK(map.stateOf(id("A2")) == SeatState::Held);
        CHECK(map.stateOf(id("A3")) == SeatState::Available);
        CHECK_THROWS_AS(map.confirm(ids({ "A2", "Z9" })), SeatNotFoundError);
        CHECK(map.stateOf(id("A2")) == SeatState::Held);

        map.confirm(ids({ "A2" }));
        CHECK(map.stateOf(id("A2")) == SeatState::Sold);
        CHECK_THROWS_AS(map.confirm(ids({ "A2" })), InvalidSeatStateError);  // Sold khong phai Held
        CHECK(map.stateOf(id("A2")) == SeatState::Sold);
    }

    // ---------- 12.3: release (idempotent) ----------
    void testRelease() {
        SeatMap map(makeRoom());

        map.hold(ids({ "C1" }));
        map.release(ids({ "C1" }));  // Held -> Available
        CHECK(map.stateOf(id("C1")) == SeatState::Available);

        map.hold(ids({ "C2" }));
        map.confirm(ids({ "C2" }));
        map.release(ids({ "C2" }));  // Sold -> Available
        CHECK(map.stateOf(id("C2")) == SeatState::Available);

        map.release(ids({ "C1" }));  // Available: khong nem, khong doi (idempotent)
        CHECK(map.stateOf(id("C1")) == SeatState::Available);

        // Co ghe khong ton tai: khong ghe nao doi
        map.hold(ids({ "C3" }));
        CHECK_THROWS_AS(map.release(ids({ "C3", "Z9" })), SeatNotFoundError);
        CHECK(map.stateOf(id("C3")) == SeatState::Held);
    }

    // ---------- 12.3: restoreSold ----------
    void testRestoreSold() {
        SeatMap map(makeRoom());
        map.restoreSold(ids({ "D1", "D2" }));
        CHECK(map.stateOf(id("D1")) == SeatState::Sold);
        CHECK(map.stateOf(id("D2")) == SeatState::Sold);

        // D1 da Sold -> SeatTakenError, D3 khong doi
        try {
            map.restoreSold(ids({ "D3", "D1" }));
            CHECK(false);
        }
        catch (const SeatTakenError& error) {
            CHECK(error.seatId() == id("D1"));
        }
        CHECK(map.stateOf(id("D3")) == SeatState::Available);
        CHECK_THROWS_AS(map.restoreSold(ids({ "Z9" })), SeatNotFoundError);
    }

    // ---------- 12.3: danh sach rong va id trung ----------
    void testEmptyAndDuplicateIds() {
        SeatMap map(makeRoom());
        const std::vector<SeatId> none;

        CHECK_THROWS_AS(map.hold(none), InvalidSeatStateError);
        CHECK_THROWS_AS(map.confirm(none), InvalidSeatStateError);
        CHECK_THROWS_AS(map.release(none), InvalidSeatStateError);
        CHECK_THROWS_AS(map.restoreSold(none), InvalidSeatStateError);

        CHECK_THROWS_AS(map.hold(ids({ "A1", "A1" })), InvalidSeatStateError);
        CHECK(map.stateOf(id("A1")) == SeatState::Available);  // khong doi gi

        map.hold(ids({ "A2" }));
        CHECK_THROWS_AS(map.confirm(ids({ "A2", "A2" })), InvalidSeatStateError);
        CHECK_THROWS_AS(map.release(ids({ "A2", "A2" })), InvalidSeatStateError);
        CHECK_THROWS_AS(map.restoreSold(ids({ "A5", "A5" })), InvalidSeatStateError);
        CHECK(map.stateOf(id("A2")) == SeatState::Held);
        CHECK(map.stateOf(id("A5")) == SeatState::Available);
        CHECK(map.count(SeatState::Available) == 20);
        CHECK(map.count(SeatState::Held) == 1);
    }

    // ---------- quote (da hinh) ----------
    void testQuote() {
        SeatMap map(makeRoom());
        CHECK(map.quote({}) == Money(0));
        CHECK(map.quote(ids({ "A1", "A2" })) == Money(160000));
        CHECK(map.quote(ids({ "C1", "C2" })) == Money(240000));
        CHECK(map.quote(ids({ "D1" })) == Money(144000));
        CHECK(map.quote(ids({ "A1", "C1", "D1" })) == Money(344000));  // 3 loai ghe, khong switch
        CHECK_THROWS_AS(map.quote(ids({ "A1", "A1" })), InvalidSeatStateError);
        CHECK_THROWS_AS(map.quote(ids({ "A1", "Z9" })), SeatNotFoundError);
        CHECK(map.count(SeatState::Available) == 21);  // quote khong doi trang thai
    }

    void testQueries() {
        SeatMap map(makeRoom());
        CHECK_THROWS_AS(map.stateOf(id("Z9")), SeatNotFoundError);
        map.hold(ids({ "B1", "A3", "A2" }));
        const auto held = map.seatsInState(SeatState::Held);
        CHECK(held.size() == 3);
        CHECK(held[0] == id("A2"));  // tang dan theo SeatId
        CHECK(held[1] == id("A3"));
        CHECK(held[2] == id("B1"));
    }

    // ---------- Kich ban E1-E8 (phan ghe) ----------
    void testScenarioE1toE8() {
        SeatMap map(makeRoom());

        const auto e1 = ids({ "A1", "A2" });  // E1
        map.hold(e1);
        CHECK(map.quote(e1) == Money(160000));
        map.confirm(e1);
        CHECK(map.count(SeatState::Sold) == 2);

        try {  // E2: A2 da ban, A3 van Available
            map.hold(ids({ "A2", "A3" }));
            CHECK(false);
        }
        catch (const SeatTakenError& error) {
            CHECK(error.seatId() == id("A2"));
        }
        CHECK(map.stateOf(id("A3")) == SeatState::Available);

        map.hold(ids({ "C1" }));  // E4: thanh toan loi -> release, C1 van Available
        map.release(ids({ "C1" }));
        CHECK(map.stateOf(id("C1")) == SeatState::Available);

        CHECK_THROWS_AS(map.hold(ids({ "Z9" })), SeatNotFoundError);  // E5
        CHECK(map.count(SeatState::Sold) == 2);

        const auto e6 = ids({ "C1", "C2" });  // E6
        map.hold(e6);
        CHECK(map.quote(e6) == Money(240000));
        map.confirm(e6);

        const auto e7 = ids({ "D1" });  // E7
        map.hold(e7);
        CHECK(map.quote(e7) == Money(144000));
        map.confirm(e7);

        map.release(e1);  // E8: huy BK0001
        CHECK(map.count(SeatState::Sold) == 3);
        CHECK(map.count(SeatState::Available) == 18);
        CHECK(map.count(SeatState::Held) == 0);
    }

}  // namespace

int main() {
    testSeatId();
    testPrices();
    testLayout();
    testRoomAddSeat();
    testHold();
    testConfirm();
    testRelease();
    testRestoreSold();
    testEmptyAndDuplicateIds();
    testQuote();
    testQueries();
    testScenarioE1toE8();

    return check_summary();  // check.h cua C: in tong ket, tra 0 neu dat, 1 neu co loi
}