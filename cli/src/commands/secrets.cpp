#include "secrets.h"

#include "utils/config.h"
#include "utils/shell.h"

#include <filesystem>
#include <iostream>

namespace NCiphera::NCommands {

    int RunSecrets() {
        std::cout << "=== Ciphera Secrets Generator ===" << std::endl;

        const std::string baseDir = "/opt/family-matrix";
        const std::string envPath = baseDir + "/.env";
        const std::string envExamplePath = baseDir + "/.env.example";
        const std::string secretsDir = baseDir + "/secrets";

        if (!NCiphera::NUtils::FileExists(envExamplePath)) {
            std::cerr << "Error: .env.example not found at " << envExamplePath << std::endl;
            return 1;
        }

        NCiphera::NUtils::TConfig config(envExamplePath);

        std::cout << "\n[1/3] Generating secrets..." << std::endl;

        std::string postgresPassword = NCiphera::NUtils::Capture("openssl rand -hex 32");
        std::string registrationSecret = NCiphera::NUtils::Capture("openssl rand -hex 64");
        std::string turnSecret = NCiphera::NUtils::Capture("openssl rand -hex 32");

        config.Set("POSTGRES_PASSWORD", postgresPassword);
        config.Set("REGISTRATION_SHARED_SECRET", registrationSecret);
        config.Set("TURN_SHARED_SECRET", turnSecret);

        std::cout << "\n[2/3] Saving secrets to files..." << std::endl;

        NCiphera::NUtils::WriteFile(secretsDir + "/postgres_password", postgresPassword);
        NCiphera::NUtils::WriteFile(secretsDir + "/registration_shared_secret", registrationSecret);
        NCiphera::NUtils::WriteFile(secretsDir + "/turn_shared_secret", turnSecret);

        NCiphera::NUtils::Run("chmod 600 " + secretsDir + "/*");

        std::cout << "\n[3/3] Saving .env file..." << std::endl;
        config.Save(envPath);

        std::cout << "\n=== Secrets generated ===" << std::endl;
        std::cout << "Files created:" << std::endl;
        std::cout << "  " << secretsDir << "/postgres_password" << std::endl;
        std::cout << "  " << secretsDir << "/registration_shared_secret" << std::endl;
        std::cout << "  " << secretsDir << "/turn_shared_secret" << std::endl;
        std::cout << "  " << envPath << std::endl;
        std::cout << "\nNext step: edit homeserver.yaml and turnserver.conf with these secrets" << std::endl;

        return 0;
    }

} // namespace NCiphera::NCommands
