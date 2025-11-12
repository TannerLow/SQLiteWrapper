#include <SQLiteWrapper/Database.h>

#include <iostream>

int main() {

	sql::OpenResult opened = sql::Database::open("test.db");
	if (not opened.successful) {
		std::cout << opened.error << std::endl;
		return EXIT_FAILURE;
	}

	sql::Database db = std::move(opened.db);

	sql::Statement::CreateResult created = db.prepare("CREATE TABLE IF NOT EXISTS test(id INTEGER PRIMARY KEY, name TEXT);");
	if (not created.successful) {
		std::cout << created.error << std::endl;
		return EXIT_FAILURE;
	}

	sql::Statement::StepResult stepped = created.statement.step();
	do {
		if (not stepped.successful) {
			std::cout << stepped.error << std::endl;
			return EXIT_FAILURE;
		}
	} while (not stepped.done);

	return 0;
}