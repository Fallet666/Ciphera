#include "config.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace NCiphera::NUtils {

    TConfig::TConfig(const std::string& envPath) {
        BaseDir_ = std::filesystem::path(envPath).parent_path().string();
        Load(envPath);
    }

    const std::string& TConfig::Get(const std::string& key) const {
        auto it = Values_.find(key);
        if (it == Values_.end()) {
            throw std::runtime_error("Config key not found: " + key);
        }
        return it->second;
    }

    std::string TConfig::Get(const std::string& key, const std::string& defaultValue) const {
        auto it = Values_.find(key);
        if (it == Values_.end()) {
            return defaultValue;
        }
        return it->second;
    }

    bool TConfig::Has(const std::string& key) const {
        return Values_.find(key) != Values_.end() && !Values_.at(key).empty();
    }

    void TConfig::Set(const std::string& key, const std::string& value) {
        Values_[key] = value;
    }

    void TConfig::Save(const std::string& path) const {
        std::ofstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + path);
        }
        for (const auto& [key, value] : Values_) {
            file << key << "=" << value << "\n";
        }
    }

    const std::string& TConfig::GetBaseDir() const {
        return BaseDir_;
    }

    void TConfig::Load(const std::string& envPath) {
        std::ifstream file(envPath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open config file: " + envPath);
        }

        std::string line;
        while (std::getline(file, line)) {
            line = Trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }

            auto pos = line.find('=');
            if (pos == std::string::npos) {
                continue;
            }

            std::string key = Trim(line.substr(0, pos));
            std::string value = Trim(line.substr(pos + 1));

            if (!key.empty()) {
                Values_[key] = value;
            }
        }
    }

    std::string TConfig::Trim(const std::string& str) {
        auto start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            return "";
        }
        auto end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }

} // namespace NCiphera::NUtils
