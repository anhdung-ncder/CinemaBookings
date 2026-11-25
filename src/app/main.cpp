
#include <fstream>
#include <iostream>
#include <string>

#include "cinema/app/DataStore.h"
#include "cinema/app/Menu.h"
#include "cinema/catalog/Cinema.h"
#include "cinema/common/CinemaError.h"

#ifdef _WIN32
#include <Windows.h>
#endif

// Tu dong tim thu muc data/ ke ca khi chay tu out/build/x64-Debug/ cua Visual Studio
static std::string findDataDir() {
    const char* candidates[] = { "data", "../data", "../../data", "../../../data" };
    for (const char* dir : candidates) {
        std::ifstream test(std::string(dir) + "/movies.csv");
        if (test.is_open()) {
            return dir;
        }
    }
    return "data";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    cinema::Cinema cinema;
    cinema::BookingService service;
    cinema::DataStore store(findDataDir());
    try {
        store.load(cinema, service);
        std::cout << "Da nap du lieu tu thu muc 'data/' thanh cong.\n";
    }
    catch (const cinema::CinemaError& e) {
        std::cout << "[Canh bao] Chua nap duoc du lieu ban dau: " << e.what() << "\n";
    }

    cinema::Menu menu(cinema, service, store);
    menu.run();

    return 0;
}
