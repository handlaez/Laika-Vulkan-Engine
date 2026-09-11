#include "le_core.hpp"
#include "demo_app.cpp"

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
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}