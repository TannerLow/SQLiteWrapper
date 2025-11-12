#pragma once
#include <string>
#include <filesystem>

#include <sqlite3/sqlite3.h>
#include <SQLiteWrapper/Statement.h>

namespace fs = std::filesystem;

namespace sql {

    struct OpenResult;

    class Database {
    public:
        sqlite3* db = nullptr;

        static OpenResult open(const fs::path& filepath);

        Database() = default;
        ~Database();

        Database(const Database&) = delete;
        Database& operator=(const Database&) = delete;

        Database(Database&& other) noexcept;
        Database& operator=(Database&& other) noexcept;

        Statement::CreateResult prepare(const std::string& sql);
    };

    struct OpenResult {
        bool successful;
        Database db;
        std::string error;
    };

} // namespace sql
