#include "shell.h"

#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace NCiphera::NUtils {

    int Run(const std::string& command) {
        std::cout << "[RUN] " << command << std::endl;
        return std::system(command.c_str());
    }

    int SudoRun(const std::string& command) {
        return Run("sudo " + command);
    }

    std::string Capture(const std::string& command) {
        std::array<char, 4096> buffer;
        std::string result;

        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            throw std::runtime_error("popen() failed for: " + command);
        }

        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
            result += buffer.data();
        }

        int exitCode = pclose(pipe);
        if (exitCode != 0) {
            throw std::runtime_error("Command failed with code " + std::to_string(exitCode) + ": " + command);
        }

        if (!result.empty() && result.back() == '\n') {
            result.pop_back();
        }

        return result;
    }

    std::string SudoCapture(const std::string& command) {
        return Capture("sudo " + command);
    }

    bool FileExists(const std::string& path) {
        return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
    }

    bool DirectoryExists(const std::string& path) {
        return std::filesystem::exists(path) && std::filesystem::is_directory(path);
    }

    void CreateDirectory(const std::string& path) {
        if (!DirectoryExists(path)) {
            std::filesystem::create_directories(path);
        }
    }

    void WriteFile(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + path);
        }
        file << content;
    }

    std::string ReadFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + path);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    void SudoWriteFile(const std::string& path, const std::string& content) {
        std::string escaped;
        for (char ch : content) {
            if (ch == '\'') {
                escaped += "'\\''";
            } else {
                escaped += ch;
            }
        }
        SudoRun("sh -c 'printf \"%s\" '\"'\"'" + escaped + "'\"'\"' > " + path + "'");
    }

} // namespace NCiphera::NUtils
