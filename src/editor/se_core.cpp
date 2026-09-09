#include "src/editor/se_core.hpp"

se::SeCore::SeCore()
    : currentScene_{
        leCore_.getDevice(),
        leCore_.getResources()
    }
{
}

se::SeCore::~SeCore()
{
}

void se::SeCore::run()
{
    while (!leCore_.getWindow().shouldClose())
    {
        leCore_.beginFrame();

        updateEditor();

        leCore_.render(currentScene_);

        renderEditor();

        leCore_.endFrame();
    }
}

void se::SeCore::updateEditor()
{
}

void se::SeCore::renderEditor()
{
}
