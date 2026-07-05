#pragma once

#include <string>

namespace NCiphera::NUtils {

    int Run(const std::string& command);
    std::string Capture(const std::string& command);
    bool FileExists(const std::string& path);
    bool DirectoryExists(const std::string& path);
    void CreateDirectory(const std::string& path);
    void WriteFile(const std::string& path, const std::string& content);
    std::string ReadFile(const std::string& path);

} // namespace NCiphera::NUtils
