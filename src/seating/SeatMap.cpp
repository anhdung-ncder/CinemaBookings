#include "cinema/seating/SeatMap.h"

#include "cinema/seating/SeatErrors.h"

#include <algorithm>
#include <numeric>
#include <set>
#include <utility>

namespace cinema {

    namespace {

        using StateMap = std::map<SeatId, SeatState>;
        using IdList = std::vector<SeatId>;

        // Moi id phai ton tai trong phong (SeatNotFoundError), roi khong duoc trung (InvalidSeatStateError).
        void checkExistAndUnique(const StateMap& states, const IdList& ids) {
            for (const SeatId& id : ids) {
                if (states.find(id) == states.end()) {
                    throw SeatNotFoundError("Seat not found in room: " + id.toString());
                }
            }
            std::set<SeatId> seen;
            for (const SeatId& id : ids) {
                if (!seen.insert(id).second) {  // insert tra second = false neu da co
                    throw InvalidSeatStateError("Duplicate seat id: " + id.toString());
                }
            }
        }

        // Dung cho hold/confirm/release/restoreSold: rong -> loi, roi kiem tra nhu tren.
        void validateForChange(const StateMap& states, const IdList& ids) {
            if (ids.empty()) {
                throw InvalidSeatStateError("Seat list is empty");
            }
            checkExistAndUnique(states, ids);
        }

        // Tra ve ghe DAU TIEN (theo thu tu ids) khong o trang thai 'expected'; ids.end() neu khong co.
        IdList::const_iterator firstNotInState(const StateMap& states, const IdList& ids,
            SeatState expected) {
            return std::find_if(ids.begin(), ids.end(), [&](const SeatId& id) {
                return states.find(id)->second != expected;  // id da duoc validate nen find luon thay
                });
        }

        // Buoc COMMIT: chi gan gia tri vao node co san, khong cap phat bo nho, khong nem.
        void assignAll(StateMap& states, const IdList& ids, SeatState newState) {
            for (const SeatId& id : ids) {
                states.find(id)->second = newState;
            }
        }

    }  // namespace

    SeatMap::SeatMap(std::shared_ptr<const Room> room) : room_(std::move(room)) {
        if (!room_) {
            throw InvalidLayoutError("SeatMap needs a room");
        }
        // Dung emplace, KHONG dung states_[id] vi operator[] doi SeatId co constructor mac dinh.
        for (const Seat* seat : room_->seats()) {
            states_.emplace(seat->id(), SeatState::Available);
        }
    }

    const Room& SeatMap::room() const { return *room_; }

    SeatState SeatMap::stateOf(const SeatId& id) const {
        const auto it = states_.find(id);
        if (it == states_.end()) {
            throw SeatNotFoundError("Seat not found in room: " + id.toString());
        }
        return it->second;
    }

    std::vector<SeatId> SeatMap::seatsInState(SeatState state) const {
        std::vector<SeatId> result;
        for (const auto& entry : states_) {  // da tang dan theo SeatId
            if (entry.second == state) {
                result.push_back(entry.first);
            }
        }
        return result;
    }

    std::size_t SeatMap::count(SeatState state) const {
        return static_cast<std::size_t>(
            std::count_if(states_.begin(), states_.end(),
                [state](const auto& entry) { return entry.second == state; }));
    }

    Money SeatMap::quote(const std::vector<SeatId>& ids) const {
        if (ids.empty()) {
            return Money();  // hop dong: rong -> Money(0), KHONG nem
        }
        checkExistAndUnique(states_, ids);
        return std::accumulate(ids.begin(), ids.end(), Money(),
            [this](Money sum, const SeatId& id) {
                return sum + room_->seat(id).price();  // da hinh qua Seat::price()
            });
    }

    void SeatMap::hold(const std::vector<SeatId>& ids) {
        validateForChange(states_, ids);                                   // pha 1: kiem tra
        const auto bad = firstNotInState(states_, ids, SeatState::Available);
        if (bad != ids.end()) {
            throw SeatTakenError(*bad);
        }
        assignAll(states_, ids, SeatState::Held);                          // pha 2: ghi
    }

    void SeatMap::confirm(const std::vector<SeatId>& ids) {
        validateForChange(states_, ids);
        const auto bad = firstNotInState(states_, ids, SeatState::Held);
        if (bad != ids.end()) {
            throw InvalidSeatStateError("Seat is not held: " + bad->toString());
        }
        assignAll(states_, ids, SeatState::Sold);
    }

    void SeatMap::release(const std::vector<SeatId>& ids) {
        validateForChange(states_, ids);
        // Khong co dieu kien trang thai: Held/Sold -> Available, Available giu nguyen (idempotent).
        assignAll(states_, ids, SeatState::Available);
    }

    void SeatMap::restoreSold(const std::vector<SeatId>& ids) {
        validateForChange(states_, ids);
        const auto bad = firstNotInState(states_, ids, SeatState::Available);
        if (bad != ids.end()) {
            throw SeatTakenError(*bad);
        }
        assignAll(states_, ids, SeatState::Sold);
    }

}  // namespace cinema