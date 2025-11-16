#include <SQLiteWrapper/Database.h>
#include <O2Profiler/Profiler.h>

#include <iostream>

int main() {
	PROFILE_BEGIN("Run All Test Code", _all_tests);

	sql::Database::OpenResult opened = sql::Database::open("test.db");
	if (not opened.successful) {
		std::cout << opened.error << std::endl;
		return EXIT_FAILURE;
	}

	sql::Database db = std::move(opened.db);

	sql::Statement::CreateResult createdCreate = db.prepare("CREATE TABLE IF NOT EXISTS test(id INTEGER PRIMARY KEY, name TEXT);");
	if (not createdCreate.successful) {
		std::cout << createdCreate.error << std::endl;
		return EXIT_FAILURE;
	}

	sql::Statement::StepResult createStep;
	do {
		createStep = createdCreate.statement.step();

		if (not createStep.successful) {
			std::cout << createStep.error << std::endl;
			return EXIT_FAILURE;
		}
	} while (not createStep.done);

	sql::Statement::CreateResult createdInsert = db.prepare("INSERT OR IGNORE INTO test(id, name) VALUES (?, ?);");
	if (not createdInsert.successful) {
		std::cout << createdInsert.error << std::endl;
		return EXIT_FAILURE;
	}

	std::optional<std::string> err = createdInsert.statement.bind(1, 1);
	if (err.has_value()) {
		std::cout << err.value() << std::endl;
	}
	err = createdInsert.statement.bind(2, "Tanner");
	if (err.has_value()) {
		std::cout << err.value() << std::endl;
	}

	sql::Statement::StepResult insertStep;
	do {
		insertStep = createdInsert.statement.step();

		if (not insertStep.successful) {
			std::cout << insertStep.error << std::endl;
			return EXIT_FAILURE;
		}
	} while (not insertStep.done);

	createdInsert = db.prepare("INSERT OR IGNORE INTO test(id, name) VALUES (?, ?);");
	if (not createdInsert.successful) {
		std::cout << createdInsert.error << std::endl;
		return EXIT_FAILURE;
	}

	err = createdInsert.statement.bind(1, 2);
	if (err.has_value()) {
		std::cout << err.value() << std::endl;
	}
	err = createdInsert.statement.bind(2, "Tony");
	if (err.has_value()) {
		std::cout << err.value() << std::endl;
	}

	do {
		insertStep = createdInsert.statement.step();

		if (not insertStep.successful) {
			std::cout << insertStep.error << std::endl;
			return EXIT_FAILURE;
		}
	} while (not insertStep.done);

	sql::Statement::CreateResult createdSelect = db.prepare("SELECT * FROM test;");
	if (not createdSelect.successful) {
		std::cout << createdSelect.error << std::endl;
		return EXIT_FAILURE;
	}

	sql::Statement::StepResult selectStep;
	do {
		selectStep = createdSelect.statement.step();

		if (not selectStep.successful) {
			std::cout << selectStep.error << std::endl;
			return EXIT_FAILURE;
		}

		if (not selectStep.done) {
			std::cout << createdSelect.statement.column_int(0) << ' ';
			std::optional<std::string> name = createdSelect.statement.column_text(1);
			if (name.has_value()) {
				std::cout << name.value();
			}
			std::cout << std::endl;
		}
	} while (not selectStep.done);

	PROFILE_END(_all_tests);
	PROFILE_REPORT();

	return 0;
}