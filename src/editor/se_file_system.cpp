#include "se_file_system.hpp"

#include <fstream>

namespace se {

    bool FileSystem::exists(const std::filesystem::path& path) {
        return std::filesystem::exists(path);
    }

    bool FileSystem::isDirectory(const std::filesystem::path& path) {
        return std::filesystem::is_directory(path);
    }

    bool FileSystem::isFile(const std::filesystem::path& path) {
        return std::filesystem::is_regular_file(path);
    }

    bool FileSystem::createDirectories(const std::filesystem::path& path) {
        try {
            return std::filesystem::create_directories(path) || std::filesystem::is_directory(path);
        }
        catch (const std::filesystem::filesystem_error&) {
            return false;
        }
    }

    std::vector<std::filesystem::path> FileSystem::listDirectory(const std::filesystem::path& path)
    {
        std::vector<std::filesystem::path> result;

        if (!isDirectory(path)) {
            return result;
        }

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            result.push_back(entry.path());
        }

        return result;
    }

    bool FileSystem::writeTextFile(const std::filesystem::path& path, const std::string& contents)
    {
        std::ofstream file(path);

        if (!file) {
            return false;
        }

        file << contents;
        return file.good();
    }

    bool FileSystem::readTextFile(const std::filesystem::path& path, std::string& contents)
    {
        std::ifstream file(path);

        if (!file) {
            return false;
        }

        contents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

        return true;
    }
}