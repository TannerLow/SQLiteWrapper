#include <SQLiteWrapper/Statement.h>

namespace sql {

    Statement::CreateResult Statement::create(sqlite3* db, const std::string& sql) {
        CreateResult result;
        result.statement.db = db;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &result.statement.stmt, nullptr) != SQLITE_OK) {
            result.error = sqlite3_errmsg(db);
            result.statement.db = nullptr;
            result.statement.stmt = nullptr;
            result.successful = false;
        }
        else {
            result.successful = true;
        }

        return result;
    }

    Statement::~Statement() {
        if (stmt) {
            sqlite3_finalize(stmt);
        }
    }

    Statement::Statement(Statement&& other) noexcept 
        : stmt(other.stmt), db(other.db) {
        
        other.stmt = nullptr;
        other.db = nullptr;
    }

    Statement& Statement::operator=(Statement&& other) noexcept {
        if (this != &other) {
            if (stmt) sqlite3_finalize(stmt);
            stmt = other.stmt;
            db = other.db;
            other.stmt = nullptr;
            other.db = nullptr;
        }
        return *this;
    }

    std::optional<std::string> Statement::bind(int idx, int value) {
        std::optional<std::string> result;

        if (sqlite3_bind_int(stmt, idx, value) != SQLITE_OK) {
            result = sqlite3_errmsg(db);
        }

        return result;
    }

    std::optional<std::string> Statement::bind(int idx, double value) {
        std::optional<std::string> result;

        if (sqlite3_bind_double(stmt, idx, value) != SQLITE_OK) {
            result = sqlite3_errmsg(db);
        }

        return result;
    }

    std::optional<std::string> Statement::bind(int idx, const std::string& value) {
        std::optional<std::string> result;

        if (sqlite3_bind_text(stmt, idx, value.c_str(), value.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
            result = sqlite3_errmsg(db);
        }

        return result;
    }

    std::optional<std::string> Statement::bind(int idx, std::nullptr_t) {
        std::optional<std::string> result;

        if (sqlite3_bind_null(stmt, idx) != SQLITE_OK) {
            result = sqlite3_errmsg(db);
        }

        return result;
    }

    Statement::StepResult Statement::step() {
        StepResult result;
        result.successful = true;

        int rc = sqlite3_step(stmt);

        if (rc == SQLITE_ROW) {
            result.done = false;
        }
        else if (rc == SQLITE_DONE) {
            result.done = true;
        }
        else {
            result.error = sqlite3_errmsg(db);
            result.successful = false;
        }

        return result;
    }

    int Statement::column_int(int idx) const { 
        return sqlite3_column_int(stmt, idx);
    }

    double Statement::column_double(int idx) const {
        return sqlite3_column_double(stmt, idx);
    }

    std::optional<std::string> Statement::column_text(int idx) const {
        std::optional<std::string> result;

        const unsigned char* raw = sqlite3_column_text(stmt, idx);
        int len = sqlite3_column_bytes(stmt, idx);

        if (!raw) {
            return std::nullopt;
        }

        result = std::string(reinterpret_cast<const char*>(raw), static_cast<size_t>(len));
        
        return result;
    }

    void Statement::reset() {
        sqlite3_reset(stmt);
    }

} // namespace sql