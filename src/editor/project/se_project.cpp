#include "se_project.hpp"

namespace se {
	Project::Project(std::filesystem::path root, std::string name)
		: root_(std::move(root)), name_(std::move(name)), startupScene_("Scenes/Main.scene")
	{
	}

	const std::string& Project::getName() const
	{
		return name_;
	}

	const std::filesystem::path& Project::getRoot() const
	{
		return root_;
	}

	std::filesystem::path Project::getProjectFile() const
	{
		return root_ / (name_ + ".laika");
	}

	std::filesystem::path Project::getAssetDirectory() const
	{
		return root_ / "Assets";
	}

	std::filesystem::path Project::getSceneDirectory() const
	{
		return root_ / "Scenes";
	}

	std::filesystem::path Project::getScriptDirectory() const
	{
		return root_ / "Scripts";
	}

	const std::filesystem::path& Project::getStartupScene() const
	{
		return startupScene_;
	}

	void Project::setStartupScene(std::filesystem::path path)
	{
		startupScene_ = std::move(path);
	}
}