// Электронный путеводитель. Проверка и чтение каталога из SQLite.
#include "catalog.hpp"

#include <winsqlite/winsqlite3.h>

#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace {
using Database = std::unique_ptr<sqlite3, decltype(&sqlite3_close)>;

class Query {
public:
    Query(sqlite3* database, const char* sql) : database_(database)
    {
        sqlite3_stmt* statement = nullptr;
        const int result = sqlite3_prepare_v2(database, sql, -1, &statement, nullptr);
        statement_.reset(statement);
        if (result != SQLITE_OK)
            fail();
    }

    bool next()
    {
        const int result = sqlite3_step(statement_.get());
        if (result != SQLITE_ROW && result != SQLITE_DONE)
            fail();
        return result == SQLITE_ROW;
    }

    RecordId id(int column) const { return sqlite3_column_int64(statement_.get(), column); }

    std::wstring text(int column) const
    {
        const auto* value = static_cast<const wchar_t*>(sqlite3_column_text16(statement_.get(), column));
        return value ? std::wstring(value) : L"";
    }

private:
    [[noreturn]] void fail() const { throw std::runtime_error(sqlite3_errmsg(database_)); }

    sqlite3* database_;
    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> statement_{nullptr, sqlite3_finalize};
};
}

std::vector<Country> loadCatalog(const std::filesystem::path& databasePath)
{
    sqlite3* rawDatabase = nullptr;
    const auto utf8Path = databasePath.u8string();
    const int result = sqlite3_open_v2(utf8Path.c_str(), &rawDatabase, SQLITE_OPEN_READONLY, nullptr);
    Database database(rawDatabase, sqlite3_close);
    if (result != SQLITE_OK)
        throw std::runtime_error("Cannot open catalog database");

    auto* db = database.get();
    Query(db, "BEGIN").next();
    Query integrity(db, "PRAGMA quick_check");
    if (!integrity.next() || integrity.text(0) != L"ok" || integrity.next()
        || Query(db, "PRAGMA foreign_key_check").next())
        throw std::runtime_error("Invalid catalog database");

    std::vector<Country> catalog;
    std::unordered_map<RecordId, std::size_t> countryIndex;
    Query countries(db, "SELECT id, name, description FROM countries ORDER BY name, id");
    while (countries.next()) {
        countryIndex.emplace(countries.id(0), catalog.size());
        catalog.push_back({countries.text(1), countries.text(2), {}, {}});
    }

    std::unordered_map<RecordId, RecordId> attractionCountry;
    Query attractions(db, "SELECT country_id, id, name, city, description "
                          "FROM attractions ORDER BY city, name, id");
    while (attractions.next()) {
        const auto countryId = attractions.id(0);
        const auto attractionId = attractions.id(1);
        attractionCountry.emplace(attractionId, countryId);
        catalog.at(countryIndex.at(countryId)).attractions.push_back({
            attractionId, attractions.text(2), attractions.text(3), attractions.text(4)
        });
    }

    Query routes(db, "SELECT country_id, id, name, description FROM routes ORDER BY name, id");
    while (routes.next()) {
        const auto countryId = routes.id(0);
        const auto routeId = routes.id(1);
        Route route{routes.text(2), routes.text(3), {}};

        // Идентификатор уже прочитан как целое число из базы.
        const auto sql = "SELECT attraction_id FROM route_stops WHERE route_id = "
                       + std::to_string(routeId) + " ORDER BY position";
        Query stops(db, sql.c_str());
        while (stops.next()) {
            const auto attractionId = stops.id(0);
            if (attractionCountry.at(attractionId) != countryId)
                throw std::runtime_error("Route stop belongs to another country");
            route.stops.push_back(attractionId);
        }
        if (route.stops.empty())
            throw std::runtime_error("Route has no stops");
        catalog.at(countryIndex.at(countryId)).routes.push_back(std::move(route));
    }
    return catalog;
}
