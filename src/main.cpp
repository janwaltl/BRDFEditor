#include "Core/Program.hpp"

#include "GL/ShaderProgram.hpp"
#include <iostream>

int
main() {
	try {
		brdfEditor::Program p;
		p.Run();
	} catch (const std::exception &e) {
		std::string asteriks(80, '*');
		asteriks += '\n';
		std::cout << asteriks << "An exception occured:\n"
				  << asteriks << e.what() << '\n'
				  << asteriks << '\n';
	} catch (...) {
		std::cout << "UNCAUGHT EXCEPTION!";
	}
	system("PAUSE");

	return 0;
}