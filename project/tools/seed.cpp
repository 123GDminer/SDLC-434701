// Электронный путеводитель. Создание демонстрационной базы при сборке.
#include <winsqlite/winsqlite3.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>

int wmain(int argc, wchar_t** argv)
{
    if (argc != 3)
        return 1;

    const std::filesystem::path output(argv[2]);
    auto temporary = output;
    temporary += L".tmp";

    try {
        const std::filesystem::path source(argv[1]);
        std::ifstream input(source, std::ios::binary);
        if (!input)
            throw std::runtime_error("Cannot read catalog.sql");
        const std::string sql{std::istreambuf_iterator<char>(input), {}};

        std::filesystem::remove(temporary);
        sqlite3* rawDatabase = nullptr;
        const auto filename = temporary.u8string();
        const int result = sqlite3_open(filename.c_str(), &rawDatabase);
        std::unique_ptr<sqlite3, decltype(&sqlite3_close)> database(rawDatabase, sqlite3_close);
        if (result != SQLITE_OK)
            throw std::runtime_error("Cannot create catalog database");
        if (sqlite3_exec(database.get(), sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(database.get()));
        database.reset();

        std::filesystem::rename(temporary, output);
    } catch (const std::exception& error) {
        std::error_code unused;
        std::filesystem::remove(temporary, unused);
        std::cerr << "Catalog: " << error.what() << '\n';
        return 1;
    }
}
