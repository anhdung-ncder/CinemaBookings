#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <filesystem>
#include <system_error>

#include "cinema/app/DataStore.h"
#include "cinema/catalog/CatalogErrors.h"
#include "cinema/catalog/DateTime.h"
#include "cinema/catalog/Movie.h"
#include "cinema/catalog/Showtime.h"
#include "cinema/seating/Room.h"
#include "cinema/common/Money.h"
#include "cinema/common/SeatId.h"

namespace cinema {
	static std::vector<std::string> split(const std::string& str, char delimiter) {
		std::vector<std::string> tokens;
		std::stringstream ss(str);
		std::string token;
		while (std::getline(ss, token, delimiter)) {
			tokens.push_back(token);
		}
		if (!str.empty() && str.back() == delimiter) {
			tokens.push_back("");
		}
		return tokens;
	}
	DataStore::DataStore(std::string dataDir) : dataDir_(std::move(dataDir)) {
		if (!dataDir_.empty() && dataDir_.back() != '/' && dataDir_.back() != '\\') {
			dataDir_ += "/";
		}
	}


	void DataStore::loadMovies(const std::string& filePath, Cinema& cinema) {
		std::ifstream in(filePath);
		if (!in.is_open()) {
			throw FileError("Khong the mo file: " + filePath);
		}

		std::string line;
		int lineNum = 0;
		while (std::getline(in, line)) {
			++lineNum;
			if (!line.empty() && line.back() == '\r') line.pop_back();
			if (line.empty() || line[0] == '#') continue;
			if (lineNum == 1) continue;
			auto cols = split(line, ',');
			if (cols.size() != 4) {
				throw FileError(filePath + ":" + std::to_string(lineNum) + ": Sai so luong cot");
			}

			try {
				int duration = std::stoi(cols[2]);
				auto movie = std::make_shared<const Movie>(cols[0], cols[1], duration, cols[3]);
				cinema.addMovie(movie);
			}
			catch (const std::exception& e) {
				throw FileError(filePath + ":" + std::to_string(lineNum) + ": " + e.what());
			}
		}
	}

	void DataStore::loadRooms(const std::string& filePath, Cinema& cinema) {
		std::ifstream in(filePath);
		if (!in.is_open()) {
			throw FileError("Khong the mo file: " + filePath);
		}
		std::string line;
		int lineNum = 0;
		while (std::getline(in, line)) {
			++lineNum;
			if (!line.empty() && line.back() == '\r') line.pop_back();
			if (line.empty() || line[0] == '#') continue;
			if (lineNum == 1) continue;

			auto cols = split(line, ',');
			if (cols.size() != 4) {
				throw FileError(filePath + ":" + std::to_string(lineNum) + ": Sai so luong cot");
			}
			try {
				Money price(std::stoll(cols[2]));
				auto room = Room::fromLayout(cols[0], cols[1], price, cols[3]);
				cinema.addRoom(room);
			}
			catch (const std::exception& e) {
				throw FileError(filePath + ":" + std::to_string(lineNum) + ": " + e.what());
			}
		}
	}

	void DataStore::loadShowtimes(const std::string& filePath, Cinema& cinema) {
		std::ifstream in(filePath);
		if (!in.is_open()) {
			throw FileError("Khong the mo file: " + filePath);
		}

		std::string line;
		int lineNum = 0;
		while (std::getline(in, line)) {
			++lineNum;
			if (!line.empty() && line.back() == '\r') line.pop_back();
			if (line.empty() || line[0] == '#') continue;
			if (lineNum == 1) continue;

			auto cols = split(line, ',');
			if (cols.size() != 5) {
				throw FileError(filePath + ":" + std::to_string(lineNum) + ": Sai so luong cot");
			}
			try {
				auto movie = cinema.movie(cols[1]);
				auto room = cinema.room(cols[2]);
				DateTime start = DateTime::parse(cols[3]);

				bool isOpen = true;
				if (cols[4] == "OPEN") {
					isOpen = true;
				}
				else if (cols[4] == "CLOSED") {
					isOpen = false;
				}
				else {
					throw std::invalid_argument("Trang thai suat chieu khong hop le: " + cols[4]);
				}

				auto st = std::make_shared<Showtime>(cols[0], movie, room, start, isOpen);
				cinema.addShowtime(st);
			}
			catch (const std::exception& e) {
				throw FileError(filePath + ":" + std::to_string(lineNum) + ":" + e.what());
			}
		}
	}

	void DataStore::loadBookings(const std::string& filePath, Cinema& cinema, BookingService& service) {
		std::ifstream in(filePath);
		if (!in.is_open()) {
			throw FileError("Khong the mo file:" + filePath);
		}
		std::string line;
		int lineNum = 0;
		while (std::getline(in, line)) {
			++lineNum;
			if (!line.empty() && line.back() == '\r') line.pop_back();
			if (line.empty() || line[0] == '#') continue;
			if (lineNum == 1) continue;

			auto cols = split(line, ',');
			if (cols.size() != 7) {
				throw FileError(filePath + ":" + std::to_string(lineNum) + ": Sai so luong cot");
			}
			try {
				auto showtime = cinema.showtime(cols[1]);

				auto seatStrs = split(cols[3], ';');
				if (seatStrs.empty()) {
					throw std::invalid_argument("Danh sach ghe trong booking bi rong");
				}
				std::vector<SeatId> seatIds;
				for (const auto& s : seatStrs) {
					if (s.empty()) {
						throw std::invalid_argument("Ghe rong khong hop le");
					}
					seatIds.push_back(SeatId::parse(s));
				}
				Money total(std::stoll(cols[4]));

				BookingStatus status = BookingStatus::Confirmed;
				if (cols[6] == "CONFIRMED") {
					status = BookingStatus::Confirmed;
				}
				else if (cols[6] == "CANCELLED") {
					status = BookingStatus::Cancelled;
				}
				else {
					throw std::invalid_argument("Trang thai booking khong hop le:" + cols[6]);
				}

				BookingRecord record{
				cols[0], cols[1], seatIds, cols[2], total, cols[5], status };

				service.importBooking(record, showtime);
			}
			catch (const std::exception& e) {
				throw FileError(filePath + ":" + std::to_string(lineNum) + ":" + e.what());
			}
		}
	}

	void DataStore::load(Cinema& cinema, BookingService& service) {
		Cinema tempCinema;
		BookingService tempService;

		loadMovies(dataDir_ + "movies.csv", tempCinema);
		loadRooms(dataDir_ + "rooms.csv", tempCinema);
		loadShowtimes(dataDir_ + "showtimes.csv", tempCinema);
		loadBookings(dataDir_ + "bookings.csv", tempCinema, tempService);
		cinema = std::move(tempCinema);
		service = std::move(tempService);
	}

	void DataStore::saveMovies(const std::string& filePath, const Cinema& cinema) {
		std::ofstream out(filePath);
		if (!out.is_open()) throw FileError("Khong the ghi file: " + filePath);
		out << "movieId,title,durationMinutes,genre\n";
		for (const auto& m : cinema.movies()) {
			out << m->id() << "," << m->title() << "," << m->durationMinutes() << "," << m->genre() << "\n";
		}
	}

	void DataStore::saveRooms(const std::string& filePath, const Cinema& cinema) {
		std::ofstream out(filePath);
		if (!out.is_open()) throw FileError("Khong the ghi file: " + filePath);
		out << "roomId,name,standardPrice,layout\n";
		for (const auto& r : cinema.rooms()) {
			Money standardPrice(0);
			std::string layout;
			char currentRow = 0;
			for (const auto* s : r->seats()) {
				if (s->type() == SeatType::Standard && standardPrice.dong() == 0) {
					standardPrice = s->price();
				}
				if (currentRow != 0 && s->id().row() != currentRow) {
					layout += '|';
				}
				currentRow = s->id().row();
				if (s->type() == SeatType::Standard) layout += 'S';
				else if (s->type() == SeatType::Vip) layout += 'V';
				else if (s->type() == SeatType::Couple) layout += 'C';
			}
			out << r->id() << "," << r->name() << "," << standardPrice.dong() << "," << layout << "\n";
		}
	}

	void DataStore::saveShowtimes(const std::string& filePath, const Cinema& cinema) {
		std::ofstream out(filePath);
		if (!out.is_open()) throw FileError("Khong the ghi file: " + filePath);
		out << "showtimeId,movieId,roomId,startDateTime,status\n";
		for (const auto& st : cinema.showtimes()) {
			out << st->id() << ","
				<< st->movie().id() << ","
				<< st->room().id() << ","
				<< st->start().toString() << ","
				<< (st->isOpen() ? "OPEN" : "CLOSED") << "\n";
		}
	}
	void DataStore::saveBookings(const std::string& filePath, const BookingService& service) {
		std::ofstream out(filePath);
		if (!out.is_open()) throw FileError("Khong the ghi file: " + filePath);
		out << "bookingId,showtimeId,customer,seats,total,paymentCode,status\n";
		for (const auto& rec : service.exportRecords()) {
			out << rec.bookingId << "," << rec.showtimeId << "," << rec.customer << ",";
			for (size_t i = 0; i < rec.seatIds.size(); ++i) {
				out << rec.seatIds[i].toString();
				if (i + 1 < rec.seatIds.size()) out << ";";
			}
			out << "," << rec.total.dong() << ","
				<< rec.paymentCode << ","
				<< (rec.status == BookingStatus::Confirmed ? "CONFIRMED" : "CANCELLED") << "\n";
		}
	}

	void DataStore::save(const Cinema& cinema, const BookingService& service) {

		std::string mTmp = dataDir_ + "movies.csv.tmp";
		std::string rTmp = dataDir_ + "rooms.csv.tmp";
		std::string sTmp = dataDir_ + "showtimes.csv.tmp";
		std::string bTmp = dataDir_ + "bookings.csv.tmp";

		saveMovies(mTmp, cinema);
		saveRooms(rTmp, cinema);
		saveShowtimes(sTmp, cinema);
		saveBookings(bTmp, service);

		auto replaceFile = [](const std::string& from, const std::string& to) {
			std::remove(to.c_str());
			if (std::rename(from.c_str(), to.c_str()) != 0) {
				throw FileError("Khong the doi ten file tam sang: " + to);
			}
			};

		replaceFile(mTmp, dataDir_ + "movies.csv");
		replaceFile(rTmp, dataDir_ + "rooms.csv");
		replaceFile(sTmp, dataDir_ + "showtimes.csv");
		replaceFile(bTmp, dataDir_ + "bookings.csv");
	}

}
