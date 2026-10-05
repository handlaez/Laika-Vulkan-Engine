#include "laika_app.hpp"

#include "src/objects/le_model.hpp"
#include "src/objects/le_camera.hpp"

#include <iostream>

LaikaApp::LaikaApp(le::log::Logger& logger) : scriptSystem_(logger)
{
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

void LaikaApp::configureScriptProject(
    const std::filesystem::path& scriptProject, const std::filesystem::path& managedAssembly,
    const std::filesystem::path& gameAssembly, const std::filesystem::path& runtimeConfig)
{
    scriptSystem_.configureProject(scriptProject, managedAssembly, gameAssembly, runtimeConfig);
}

void LaikaApp::clearScriptProject()
{
    scriptSystem_.clearProject();
}
