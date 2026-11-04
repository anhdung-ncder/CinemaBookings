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
