#include "se_project.hpp"

namespace se {
	Project::Project(std::filesystem::path root, std::string name)
		: root_(std::move(root)), name_(std::move(name))
	{
	}

	const std::filesystem::path Project::getRoot() const
	{
		return root_ / (name_ + ".laika");
	}

	const std::filesystem::path Project::getAssetDirectory() const
	{
		return root_ / "Assets";
	}

	const std::filesystem::path Project::getSceneDirectory() const
	{
		return root_ / "Scenes";
	}

	const std::filesystem::path Project::getScriptDirectory() const
	{
		return root_ / "Scripts";
	}
}