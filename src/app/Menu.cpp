#include "cinema/app/Menu.h"
#include "cinema/app/Reports.h"
#include "cinema/booking/BookingErrors.h"
#include "cinema/booking/PaymentFactory.h"
#include "cinema/catalog/CatalogErrors.h"
#include "cinema/common/CinemaError.h"
#include "cinema/common/Money.h"
#include "cinema/common/SeatId.h"
#include "cinema/seating/SeatErrors.h"
#include "cinema/seating/SeatTypes.h"

#include <iostream>
#include <sstream>
#include <vector>

namespace cinema {
	Menu::Menu(Cinema& cinema, BookingService& service, DataStore& store)
		: cinema_(cinema), service_(service), store_(store) {
	}

	std::string Menu::readline(const std::string& prompt) {
		std::cout << prompt;
		std::string line;
		if (!std::getline(std::cin, line)) {
			return "";
		}
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		return line;
	}

	int Menu::readInt(const std::string& prompt) {
		while (true) {
			std::string line = readline(prompt);
			if (line.empty()) {
				std::cout << "[Loi] Vui long nhap mot so nguyen!\n";
				continue;
			}
			try {
				std::size_t pos = 0;
				int val = std::stoi(line, &pos);
				if (pos == line.size()) {
					return val;
				}
			}
			catch (...) {
			}
			std::cout << "[Loi] Gia tri khong phai so hop le. Thu lai!\n";

		}
	}

	void Menu::printSeatMap(const Showtime& showtime) {
		std::cout << "\n--- SO DO GHE SUAT " << showtime.id()
			<< " (" << showtime.movie().title()
			<< " - " << showtime.start().toString() << ") ---\n";
		std::cout << "Trang thai: " << (showtime.isOpen() ? "OPEN" : "CLOSED") << "\n";
		std::cout << "Chu thich: [S/V/C]=Trong   [H]=Dang giu   [X]=Da ban\n\n";

		char currentRow = 0;
		for (const Seat* s : showtime.room().seats()) {
			if (currentRow != 0 && s->id().row() != currentRow) {
				std::cout << "\n";
			}
			if (s->id().row() != currentRow) {
				currentRow = s->id().row();
				std::cout << "Hang " << currentRow << ": ";
			}
			SeatState st = showtime.seatMap().stateOf(s->id());
			std::string mark;
			if (st == SeatState::Available) {
				mark = std::string(1, toCode(s->type()));
			}
			else if (st == SeatState::Held) {
				mark = "H";
			}
			else {
				mark = "X";
			}
			std::cout << s->id().toString() << "[" << mark << "] ";
		}
		std::cout << "\n\n";
	}

	void Menu::run() {
		while (true) {
			std::cout << "\n================ CINEMA BOOKING MENU ================\n";
			std::cout << "1. Xem lich chieu & So do ghe\n";
			std::cout << "2. Dat ve xem phim\n";
			std::cout << "3. Huy ve (theo ma booking)\n";
			std::cout << "4. Bao cao & Tim kiem\n";
			std::cout << "5. Luu/ Nap du lieu (CSV)\n";
			std::cout << "6. Mo/Dong suat chieu\n";
			std::cout << "0. Thoat chuong trinh\n";
			std::cout << "=====================================================\n";

			int choice = readInt("Chon chuc nang: ");
			if (choice == 0) {
				std::cout << "Tam biet\n";
				break;
			}


			try {
				switch (choice) {
				case 1: handleListShowtimesAndSeats(); break;
				case 2: handleBookingTickets(); break;
				case 3: handleCancelBooking(); break;
				case 4: handleReports(); break;
				case 5: handleStorage(); break;
				case 6: handleToggleShowtime(); break;
				default:
					std::cout << "[Loi] Lua chon khong ton tai. Chon 0..6\n";
					break;
				}
			}
			catch (const SeatTakenError& e) {
				std::cout << "[Loi ghe da ban] " << e.what() << "\n";
			}
			catch (const ShowtimeClosedError& e) {
				std::cout << "[Loi suat da dong] " << e.what() << "\n";
			}
			catch (const PaymentFailedError& e) {
				std::cout << "[Loi thanh toan] " << e.what() << "\n";
			}
			catch (const BookingError& e) {
				std::cout << "[Loi dat ve] " << e.what() << "\n";
			}
			catch (const FileError& e) {
				std::cout << "[Loi file du lieu] " << e.what() << "\n";
			}
			catch (const CinemaError& e) {
				std::cout << "[Loi he thong] " << e.what() << "\n";
			}
			catch (const std::exception& e) {
				std::cout << "[Loi nhap lieu] " << e.what() << "\n";
			}
		}
	}

	void Menu::handleListShowtimesAndSeats() const {
		const auto list = cinema_.showtimes();
		if (list.empty()) {
			std::cout << "Hien chua co suat chieu nao. \n";
			return;
		}
		std::cout << "\n--- DANH SACH SUAT CHIEU ---\n";
		for (const auto& st : list) {
			std::cout << st->id() << " | " << st->movie().title()
				<< " | Phong: " << st->room().name()
				<< " | " << st->start().toString()
				<< " | " << (st->isOpen() ? "OPEN" : "CLOSED")
				<< " | Trong: " << st->seatMap().count(SeatState::Available) << "\n";
		}

		std::string id = readline("Nhap ma suat chieu de xem ghe (Enter de bo qua): ");
		if (!id.empty()) {
			auto st = cinema_.showtime(id);
			printSeatMap(*st);
		}
	}

	void Menu::handleBookingTickets() {
		std::string showtimeId = readline("Nhap ma suat chieu (vd S1): ");
		auto st = cinema_.showtime(showtimeId);
		printSeatMap(*st);

		std::string seatsLine = readline("Nhap danh sach ghe (cach nhau bang dau cach, vs A1 A2): ");
		std::stringstream ss(seatsLine);
		std::string token;
		std::vector<SeatId> seatIds;
		while (ss >> token) {
			seatIds.push_back(SeatId::parse(token));
		}

		std::string customer = readline("Nhap ten khach hang: ");
		std::string payCode = readline("Hinh thuc thanh toan (CASH / CARD / WALLET): ");
		std::string fundsStr = readline("Nhap so tien/han muc /so du (Enter de dung mac dinh 10,000,000): ");

		std::shared_ptr<IPayment> payment;
		if (fundsStr.empty()) {
			payment = PaymentFactory::create(payCode);
		}
		else {
			std::size_t pos = 0;
			long long fundsVal = std::stoll(fundsStr, &pos);
			if (pos != fundsStr.size()) {
				throw std::invalid_argument("So tien khong hop le: " + fundsStr);
			}
			payment = PaymentFactory::create(payCode, Money(fundsVal));
		}

		auto booking = service_.book(st, seatIds, payment, customer);

		std::cout << "\n>>> DAT VE THANH CONG! <<<\n";
		std::cout << "Ma don: " << booking->id() << "\n";
		std::cout << "Khach hang: " << booking->customer() << "\n";
		std::cout << "Thanh toan: " << booking->paymentCode() << "\n";
		std::cout << "Tong tien (da gom phi): " << booking->total().toString() << "\n";
		std::cout << "Danh sach ve:\n";
		for (const auto& t : booking->tickets()) {
			std::cout << " -" << t.ticketId()
				<< " | Ghe: " << t.seatId().toString()
				<< " | Gia: " << t.price().toString() << "\n";
		}
	}

	void Menu::handleCancelBooking() {
		std::string id = readline("Nhap ma booking can huy (VD BK001): ");
		Money refunded = service_.cancel(id);
		std::cout << ">>> HUY VE THANH CONG! Da hoan lai: " << refunded.toString() << "\n";
	}

	void Menu::handleReports() const {
		std::cout << "\n--- BAO CAO & TIM KIEM ---\n";
		std::cout << "1. Suat chieu theo ngay (YYYY-MM-DD)\n";
		std::cout << "2. Suat chieu dang mo & con ghe trong\n";
		std::cout << "3. Tim phim theo tu khoa\n";
		std::cout << "4. Doanh thu theo tung phim (Confirmed)\n";
		std::cout << "5. Lich su dat ve cua khach hang\n";

		int sub = readInt("Chon bao cao: ");
		if (sub == 1) {
			std::string d = readline("Nhap ngay (vd 2026-12-01): ");
			auto res = Reports::showtimesByDate(cinema_, d);
			std::cout << "Tim thay " << res.size() << " suat:\n";
			for (const auto& st : res) {
				std::cout << " -" << st->id() << " |" << st->movie().title()
					<< " | " << st->start().toString() << "\n";
			}
		}
		else if (sub == 2) {
			auto res = Reports::availableShowtime(cinema_);
			std::cout << "Co " << res.size() << " suat con ghe trong:\n";
			for (const auto& st : res) {
				std::cout << " -" << st->id() << " | " << st->movie().title()
					<< " | Con " << st->seatMap().count(SeatState::Available) << " ghe\n";
			}
		}
		else if (sub == 3) {
			std::string kw = readline("Nhap tu khoa ten phim: ");
			auto res = Reports::searchMovies(cinema_, kw);
			std::cout << "Tim thay " << res.size() << " phim\n";
			for (const auto& m : res) {
				std::cout << "  - " << m->id() << " | " << m->title()
					<< " (" << m->durationMinutes() << " phut," << m->genre() << ")\n";
			}
		}
		else if (sub == 4) {
			auto rev = Reports::revenueByMovie(service_);
			if (rev.empty()) {
				std::cout << "Chua co doanh thu nao.\n";
			}
			for (const auto& pair : rev) {
				std::cout << "  - Phim " << pair.first << ": " << pair.second.toString() << "\n";
			}
		}
		else if (sub == 5) {
			std::string cust = readline("Nhap ten khach hang: ");
			auto res = Reports::bookingsByCustomer(service_, cust);
			std::cout << "Tim thay " << res.size() << " don:\n";
			for (const auto& b : res) {
				std::cout << "  - " << b->id() << " | Suat: " << b->showtime().id()
					<< " | " << b->total().toString()
					<< " | " << toString(b->status()) << "\n";
			}
		}
		else {
			std::cout << "[LOI] Lua chon bao cao khong hop le.\n ";
		}
	}

	void Menu::handleStorage() {
		std::cout << "\n--- LUU / NAP FILE CSV (" << store_.dataDir() << ") ---\n";
		std::cout << "1. Luu du lieu hien tai ra file CSV\n";
		std::cout << "2. Nap lai du lieu tu file CSV\n";
		int sub = readInt("Chon: ");
		if (sub == 1) {
			store_.save(cinema_, service_);
			std::cout << ">>> Luu du lieu thanh cong!\n";
		}
		else if (sub == 2) {
			store_.load(cinema_, service_);
			std::cout << ">>> Nap du lieu thanh cong!\n";
		}
		else {
			std::cout << "[LOI] Lua chon khong hop le.\n";
		}
	}

	void Menu::handleToggleShowtime() {
		std::string id = readline("Nhap ma suat chieu can mo/dong: ");
		auto st = cinema_.showtime(id);
		if (st->isOpen()) {
			st->close();
			std::cout << "Da DONG suat chieu " << st->id() << ".\n";
		}
		else {
			st->open();
			std::cout << "Da MO suat chieu " << st->id() << ".\n";
		}
	}

}
