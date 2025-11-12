#include <SQLiteWrapper/Database.h>


namespace sql {

    Database::~Database() {
        if (db) {
            sqlite3_close(db);
        }
    }

    Database::OpenResult Database::open(const fs::path& filepath) {
        OpenResult result;

        if (sqlite3_open(filepath.u8string().c_str(), &result.db.db) != SQLITE_OK) {
            result.error = sqlite3_errmsg(result.db.db);
            sqlite3_close(result.db.db);
            result.db.db = nullptr;
            result.successful = false;
        }
        else {
            result.successful = true;
        }

        return result;
    }

    Database::Database(Database&& other) noexcept 
        : db(other.db) { 

        other.db = nullptr; 
    }

    Database& Database::operator=(Database&& other) noexcept {
        if (this != &other) {
            if (db) sqlite3_close(db);
            db = other.db;
            other.db = nullptr;
        }

        return *this;
    }

    Statement::CreateResult Database::prepare(const std::string& sql) {
        return Statement::create(db, sql);
    }

} // namespace sql