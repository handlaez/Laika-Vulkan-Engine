#include "laika_app.hpp"

#include "src/objects/le_model.hpp"
#include "src/objects/le_camera.hpp"

#include <iostream>

LaikaApp::LaikaApp()
{
    if (!scriptSystem_.initialize(
        "managed/Laika.GameTest.runtimeconfig.json",
        "managed/Laika.Managed.dll",
        "managed/Laika.GameTest.dll"))
    {
        std::cerr << "Failed to initialize Laika C# scripting\n";
    }
}

void LaikaApp::onStart(le::LeScene& scene) 
{
    scriptSystem_.start(scene);
}

void LaikaApp::onUpdate(le::LeScene& scene, le::FrameInfo fi, bool viewportActive) 
{
    // movement
    controller_.moveInPlaneXZ(fi.window, fi.deltaTime, scene.getCameraObject(), viewportActive);

    // camera update
    scene.getCamera().setPerspectiveProjection(glm::radians(50.f), fi.aspect, 0.1f, 100.f);
    scene.getCamera().setView(scene.getCameraObject().transform.translation, scene.getCameraObject().transform.rotation);

    // scripts
    scriptSystem_.update(scene, fi.deltaTime);
}

void LaikaApp::onShutdown(le::LeScene&)
{
    scriptSystem_.shutdown();
}
