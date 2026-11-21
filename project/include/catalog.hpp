// Электронный путеводитель. Записи каталога и загрузка локальной базы.
#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

using RecordId = std::int64_t;

struct Attraction {
    RecordId id;
    std::wstring name;
    std::wstring city;
    std::wstring description;
};

struct Route {
    std::wstring name;
    std::wstring description;
    std::vector<RecordId> stops;
};

struct Country {
    std::wstring name;
    std::wstring description;
    std::vector<Attraction> attractions;
    std::vector<Route> routes;
};

std::vector<Country> loadCatalog(const std::filesystem::path& databasePath);
