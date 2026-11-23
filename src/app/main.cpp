
#include <iostream>

#include "cinema/app/DataStore.h"
#include "cinema/app/Menu.h"
#include "cinema/catalog/Cinema.h"
#include "cinema/common/CinemaError.h"

#ifdef _WIN32
#include <Windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    cinema::Cinema cinema;
    cinema::BookingService service;
    cinema::DataStore store("data");
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
