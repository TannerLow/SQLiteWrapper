#pragma once
#include <string>
#include <optional>

#include <sqlite3/sqlite3.h>

namespace sql {

	class Statement {
    public:
        struct CreateResult;
        struct StepResult;

        sqlite3_stmt* stmt = nullptr;
        sqlite3* db = nullptr;
        
        static CreateResult create(sqlite3* db, const std::string& sql);

        Statement() = default;
        ~Statement();

        Statement(const Statement&) = delete;
        Statement& operator=(const Statement&) = delete;

        Statement(Statement&& other) noexcept;
        Statement& operator=(Statement&& other) noexcept;

        std::optional<std::string> bind(int idx, int value);
        std::optional<std::string> bind(int idx, double value);
        std::optional<std::string> bind(int idx, const std::string& value);
        std::optional<std::string> bind(int idx, std::nullptr_t);

        StepResult step();

        int column_int(int idx) const;
        double column_double(int idx) const;
        std::optional<std::string> column_text(int idx) const;

        void reset();
	};

    struct Statement::CreateResult {
        bool successful;
        Statement statement;
        std::string error;
    };

    struct Statement::StepResult {
        bool successful;
        bool done;
        std::string error;
    };

} // namespace sql