#include "src/editor/se_core.hpp"
#include "src/demo_app.hpp"

namespace se {

    se::SeCore::SeCore() : leCore_{}, currentScene_ { leCore_.getDevice(), leCore_.getResources() }
    {
    }

    se::SeCore::~SeCore()
    {
    }

    void se::SeCore::run()
    {
        DemoApp demoApp;
        demoApp.onStart(currentScene_);

        while (!leCore_.getWindow().shouldClose())
        {
            leCore_.beginFrame();

            demoApp.onUpdate(currentScene_, leCore_.getFrameInfo());
            updateEditor();

            leCore_.render(currentScene_);
            renderEditor();

            leCore_.endFrame();
        }

        demoApp.onShutdown();
    }

    void se::SeCore::updateEditor()
    {
    }

    void se::SeCore::renderEditor()
    {
    }
} // se