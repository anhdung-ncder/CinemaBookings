# CinemaBookings — Hệ Thống Đặt Vé Xem Phim (C++17)

Đồ án môn Lập trình Hướng đối tượng (OOP) — Chủ đề 6: **Cinema Booking**.  
Dự án được xây dựng theo chuẩn **C++17**, quản lý cấu hình bằng **CMake**, tuân thủ nguyên tắc thiết kế hướng đối tượng (SOLID), áp dụng các Design Patterns (Factory, Strategy, Facade) và xử lý ngoại lệ an toàn (Exception Safety & Rollback).

---

## 👥 Phân Công Trách Nhiệm (3 Gói Công Việc)

Dự án được phân chia thành 3 gói độc lập, giao tiếp thông qua **Hợp đồng chung v1.0**:

* **Gói A — Ghế & Trạng thái đặt chỗ (`seating`)**:
  * Mô tả ghế vật lý đa hình: `Seat` (`StandardSeat`, `VipSeat`, `CoupleSeat`).
  * Mẫu thiết kế `SeatFactory`, quản lý phòng chiếu `Room`.
  * Quản lý trạng thái ghế của từng suất chiếu: `SeatMap` (Available / Held / Sold) đảm bảo Strong Exception Safety (All-or-Nothing).
* **Gói B — Thanh toán, Booking & Vé (`booking`)**:
  * Mẫu thiết kế Strategy cho thanh toán: `IPayment` (`CashPayment`, `CardPayment`, `WalletPayment`), `PaymentFactory`.
  * Quản lý đặt vé và phát vé: `Ticket`, `Booking`, `BookingRecord`.
  * Dịch vụ đặt vé `BookingService`: Quy trình 6 bước đặt vé an toàn ngoại lệ và cơ chế Rollback hoàn tiền/trả ghế khi lỗi.
* **Gói C — Phim, Suất chiếu, Dữ liệu & Ghép chương trình (`catalog` + `app`)**:
  * Quản lý danh mục: `Movie`, `DateTime`, `Cinema`.
  * Mẫu thiết kế Facade kết nối sang ghế: `Showtime` (kiểm tra đóng/mở suất).
  * Đọc/Ghi dữ liệu CSV an toàn All-or-Nothing: `DataStore`.
  * Báo cáo thống kê bằng STL & Lambda: `Reports`.
  * Giao diện dòng lệnh (Console UI): `Menu`, `main.cpp`.
  * Quản lý cấu hình `CMakeLists.txt`, bộ kiểm thử `tests/check.h` và kịch bản nghiệm thu tích hợp `tests/test_e2e.cpp`.

---

## 📂 Cấu Trúc Thư Mục Dự Án

```text
CinemaBookings/
├── CMakeLists.txt                # Cấu hình biên dịch CMake (cinema_core, cinema_app, tests)
├── README.md                     # Tài liệu hướng dẫn dự án
├── .gitignore                    # Bỏ qua thư mục build, cache IDE (.vs, out, build)
│
├── include/cinema/               # Các tệp Header (.h)
│   ├── common/                   # Dữ liệu & lỗi nền tảng (CinemaError, SeatId, Money)
│   ├── seating/                  # Gói A: Quản lý ghế, phòng và sơ đồ trạng thái ghế
│   ├── catalog/                  # Gói C: Quản lý phim, suất chiếu, ngày giờ, kho rạp
│   ├── booking/                  # Gói B: Thanh toán, vé, đơn đặt vé và dịch vụ booking
│   └── app/                      # Gói C: DataStore, Reports, Menu
│
├── src/                          # Mã nguồn triển khai (.cpp tương ứng với header)
│   ├── common/
│   ├── seating/
│   ├── catalog/
│   ├── booking/
│   └── app/main.cpp              # Điểm khởi đầu của ứng dụng (cinema_app)
│
├── tests/                        # Bộ kiểm thử đơn vị & kiểm thử tích hợp
│   ├── check.h                   # Micro testing framework (CHECK, CHECK_THROWS_AS)
│   ├── test_seating.cpp          # Kiểm thử Gói A
│   ├── test_booking.cpp          # Kiểm thử Gói B
│   └── test_e2e.cpp              # Kiểm thử tích hợp E2E kịch bản E1-E12 (Gói C)
│
├── data/                         # Tệp dữ liệu CSV lưu trữ trạng thái
│   ├── movies.csv                # Danh mục phim
│   ├── rooms.csv                 # Sơ đồ phòng chiếu
│   ├── showtimes.csv             # Lịch chiếu
│   └── bookings.csv              # Lịch sử đặt vé
│
└── docs/                         # Tài liệu thiết kế & sơ đồ
    ├── uml/                      # Sơ đồ PlantUML (seating.puml, booking.puml, catalog.puml)
    └── report/                   # Báo cáo đồ án
```

---

## 🛠️ Hướng Dẫn Biên Dịch & Chạy Chương Trình

Yêu cầu môi trường: Trình biên dịch hỗ trợ **C++17** (MSVC Visual Studio 2022 hoặc GCC / Clang) và **CMake 3.20+**.

### Cách 1: Sử dụng Visual Studio 2022 (Khuyên dùng)
1. Mở Visual Studio 2022.
2. Chọn **File** $\rightarrow$ **Open** $\rightarrow$ **Folder...** và chọn thư mục `CinemaBookings`.
3. Đợi Visual Studio tự động nạp cấu hình CMake (thông báo *CMake generation finished* ở cửa sổ Output).
4. Chọn mục khởi chạy ở thanh công cụ phía trên:
   * Chạy ứng dụng chính: chọn **`cinema_app.exe`** (hoặc `CinemaBookings.exe`).
5. Bấm **Ctrl + F5** (hoặc nhấn nút xanh *Start Without Debugging*) để chạy chương trình.

### Cách 2: Sử dụng dòng lệnh (Terminal / PowerShell)

```powershell
# 1. Di chuyển vào thư mục dự án
cd CinemaBookings

# 2. Tạo thư mục build và cấu hình CMake
cmake -S . -B build

# 3. Biên dịch toàn bộ dự án
cmake --build build --config Debug

# 4. Chạy chương trình
.\build\cinema_app.exe
```

---

## 🧪 Hướng Dẫn Chạy Kiểm Thử (Tests)

Dự án sử dụng micro-framework tự viết [`tests/check.h`](tests/check.h) (không dùng thư viện ngoài) và tự động cấu hình qua CTest:

### Cách 1: Trong Visual Studio 2022
Ở ô **Select Startup Item** trên thanh công cụ, chọn target muốn chạy rồi bấm **Ctrl + F5**:
* `test_seating.exe` — Kiểm thử Gói A (`SeatId`, đa hình 3 loại ghế, `Room`, `SeatMap`).
* `test_e2e.exe` — Kiểm thử tích hợp toàn hệ thống theo **12 kịch bản E1–E12**.

### Cách 2: Bằng dòng lệnh (Terminal / PowerShell)

```powershell
# Chạy toàn bộ test qua CTest
ctest --test-dir build --output-on-failure

# Hoặc chạy từng file test trực tiếp
.\build\test_seating.exe
.\build\test_e2e.exe
```

*(Lưu ý: Với bộ test `test_book_flow.cpp` của Gói B, biên dịch lẻ bằng `g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_book_flow.cpp -Lbuild -lcinema_core -o build/test_book_flow.exe` rồi chạy `.\build\test_book_flow.exe`.)*

---

## 📁 Dữ Liệu Mẫu (`data/`)

Khi khởi động, `cinema_app` tự động tìm và nạp 4 file CSV (UTF-8, không dấu ngoặc kép) từ thư mục `data/`:
* `movies.csv`: Phim `M1` (*Space Odyssey*, 120 phút, SciFi).
* `rooms.csv`: Phòng `R1` (21 ghế: `A1–A6`, `B1–B6` Standard `80,000`; `C1–C6` VIP `120,000`; `D1–D3` Couple `144,000`) và phòng `R2`.
* `showtimes.csv`: Suất `S1` (`2026-12-01 18:00`, `OPEN`) và `S2` (`2026-12-01 21:00`, `CLOSED`).
* `bookings.csv`: Chỉ chứa dòng tiêu đề ban đầu để phục vụ chạy kịch bản demo từ `BK0001`.

---

## 🎬 Kịch Bản Demo Trực Tiếp (8–12 Phút)

Chạy `cinema_app.exe` và thao tác lần lượt theo đúng các bước sau:

| Bước | Chức năng | Thao tác nhập trên Console | Kết quả kỳ vọng (Chứng minh OOP) |
|---|---|---|---|
| **1** | **Xem lịch & Ghế** | Chọn `1` $\rightarrow$ Nhập `S1` | Hiện suất `S1 (OPEN)` và sơ đồ 21 ghế (`[S]`, `[V]`, `[C]`). |
| **2** | **Đặt vé `CASH` (Standard)** | Chọn `2` $\rightarrow$ `S1` $\rightarrow$ `A1 A2` $\rightarrow$ `An` $\rightarrow$ `CASH` $\rightarrow$ `Enter` | Thành công `BK0001`, tổng **`160,000 VND`**, in 2 vé `BK0001-A1`, `BK0001-A2`. |
| **3** | **Đa hình Ghế VIP + `CARD`** | Chọn `2` $\rightarrow$ `S1` $\rightarrow$ `C1 C2` $\rightarrow$ `Chi` $\rightarrow$ `CARD` $\rightarrow$ `Enter` | Thành công `BK0002`, giá VIP 150% (`240,000`) + phí 2% (`4,800`) = **`244,800 VND`**. |
| **4** | **Đa hình Ghế Couple** | Chọn `2` $\rightarrow$ `S1` $\rightarrow$ `D1` $\rightarrow$ `Dung` $\rightarrow$ `CASH` $\rightarrow$ `Enter` | Thành công `BK0003`, giá Couple 180% (`capacity = 2`) = **`144,000 VND`**. |
| **5** | **Chứng minh Exception** | Chọn `2` $\rightarrow$ `S1` $\rightarrow$ `A2 A3` $\rightarrow$ `Binh` $\rightarrow$ `CASH` $\rightarrow$ `Enter` | Báo `[Loi ghe da ban]` (`SeatTakenError` ở `A2`), xem lại `S1` thấy **`A3` vẫn trống** (*All-or-Nothing*). |
| **6** | **Chứng minh Rollback** | Chọn `2` $\rightarrow$ `S1` $\rightarrow$ `C3` $\rightarrow$ `Binh` $\rightarrow$ `WALLET` $\rightarrow$ `100000` | Báo `[Loi thanh toan]` (`PaymentFailedError`, thiếu tiền), xem lại `S1` thấy **`C3` được trả về trống** (không kẹt `Held`). |
| **7** | **Hủy vé & Hoàn tiền** | Chọn `3` $\rightarrow$ `BK0001` (rồi thử chọn `3` $\rightarrow$ `BK0001` lần nữa) | Lần 1: Hoàn **`160,000 VND`**, `A1 A2` trống lại. Lần 2: Báo `[Loi dat ve]` (`BookingError`). |
| **8** | **Báo cáo STL & Lambda** | Chọn `4` $\rightarrow$ `4` | Doanh thu phim `M1` (chỉ tính `Confirmed`) = **`388,800 VND`** (`244,800 + 144,000`). |
| **9** | **Lưu & Nạp lại CSV** | Chọn `5` $\rightarrow$ `1` (Lưu) $\rightarrow$ `0` (Thoát) $\rightarrow$ Mở lại app | Tự nạp lại trạng thái đã lưu: ghế `C1, C2, D1` vẫn `[X]`, đặt mới ra **`BK0004`**. |

---

## 🔗 Trạng Thái Tích Hợp (Mục C8)

| Thành viên / Gói | Nội dung đã đẩy lên `main` | Trạng thái Build & Kiểm thử |
|---|---|---|
| **Gói A (`seating/`)** | `SeatId`, `SeatErrors`, cây ghế đa hình (`Standard`/`Vip`/`Couple`), `SeatFactory`, `Room`, `SeatMap`, `data/rooms.csv`, `docs/uml/seating.puml`, `tests/test_seating.cpp` | 🟢 Build sạch (`0 warnings`) — `test_seating` **All checks passed!** |
| **Gói B (`booking/`)** | `Money`, `BookingErrors`, `IPayment` (`Cash`/`Card`/`Wallet`), `PaymentFactory`, `Ticket`, `Booking`, `BookingService` (`book` + rollback, `cancel`, `import/export`), `tests/test_book_flow.cpp` | 🟢 Build sạch (`0 warnings`) — `test_book_flow` **49/49 checks passed** |
| **Gói C (`catalog/` + `app/`)** | `CMakeLists.txt`, `tests/check.h`, `CinemaError`, `DateTime`, `Movie`, `Showtime` (Facade), `Cinema`, `DataStore` + CSV mẫu, `Reports`, `Menu`, `main.cpp`, `tests/test_e2e.cpp` (E1–E12) | 🟢 Build sạch (`0 warnings`) — `test_e2e` **All checks passed!** (E1–E12 pass 100%) |
