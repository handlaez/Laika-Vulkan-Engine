#include "le_core.hpp"
#include "demo_app.hpp"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
	le::LeCore engine{};
	DemoApp demoApp;

	try 
	{
		engine.run(demoApp);
	}
	catch (const std::exception &e) 
	{
		std::cerr << e.what() << "\n";
		std::cin;
		return EXIT_FAILURE;
	}
	std::cin;
	return EXIT_SUCCESS;
}