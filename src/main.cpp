#include "src/editor/se_core.hpp"

#include <cstdlib>
#include <iostream>
#include <exception>

int main()
{
    try
    {
        se::SeCore editor{};
        editor.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}