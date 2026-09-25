#ifndef SE_FILE_SYSTEM_HPP
#define SE_FILE_SYSTEM_HPP

#include <filesystem>
#include <string>
#include <vector>

namespace se {
    class FileSystem {
    public:
        static bool exists(const std::filesystem::path& path);
        static bool isDirectory(const std::filesystem::path& path);
        static bool isFile(const std::filesystem::path& path);

        static bool createDirectories(const std::filesystem::path& path);
        static std::vector<std::filesystem::path> listDirectory(const std::filesystem::path& path);

        static bool writeTextFile(const std::filesystem::path& path, const std::string& contents);
        static bool readTextFile(const std::filesystem::path& path, std::string& contents);
    };
}

#endif