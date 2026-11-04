cmake_minimum_required(VERSION 3.20)

project(CinemaBookings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable Debug build type by default if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Cờ cảnh báo theo Hợp đồng: MSVC (/W4 /utf-8), g++/clang (-Wall -Wextra)
if (MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra)
endif()

# 1. Tự động gom mọi file .cpp trong src/ (bao gồm seating, booking, catalog, common)
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# 2. Loại trừ main.cpp ra khỏi thư viện core (để không bị trùng hàm main)
list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/src/app/main\\.cpp$")

# 3. Tạo thư viện tĩnh cinema_core
add_library(cinema_core STATIC ${CORE_SOURCES})

target_include_directories(cinema_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 4. Chương trình chính
add_executable(cinema_app src/app/main.cpp)
target_link_libraries(cinema_app PRIVATE cinema_core)

# Tạo thêm alias CinemaBookings để các bạn gõ tên nào cũng chạy được
add_executable(CinemaBookings ALIAS cinema_app)

# 5. Cấu hình kiểm thử (tự động nhận khi các bạn tạo file test trong tests/)
enable_testing()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_seating.cpp")
    add_executable(test_seating tests/test_seating.cpp)
    target_link_libraries(test_seating PRIVATE cinema_core)
    add_test(NAME test_seating COMMAND test_seating)
endif()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_booking.cpp")
    add_executable(test_booking tests/test_booking.cpp)
    target_link_libraries(test_booking PRIVATE cinema_core)
    add_test(NAME test_booking COMMAND test_booking)
endif()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_e2e.cpp")
    add_executable(test_e2e tests/test_e2e.cpp)
    target_link_libraries(test_e2e PRIVATE cinema_core)
    add_test(NAME test_e2e COMMAND test_e2e)
endif()
