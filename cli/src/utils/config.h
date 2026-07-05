#pragma once

#include <map>
#include <string>

namespace NCiphera::NUtils {

    class TConfig {
    public:
        explicit TConfig(const std::string& envPath);

        const std::string& Get(const std::string& key) const;
        std::string Get(const std::string& key, const std::string& defaultValue) const;
        bool Has(const std::string& key) const;
        void Set(const std::string& key, const std::string& value);
        void Save(const std::string& path) const;

        const std::string& GetBaseDir() const;

    private:
        void Load(const std::string& envPath);
        static std::string Trim(const std::string& str);

        std::map<std::string, std::string> Values_;
        std::string BaseDir_;
    };

} // namespace NCiphera::NUtils
