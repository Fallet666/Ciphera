#include "tokens.h"

#include "utils/config.h"
#include "utils/shell.h"

#include <iostream>

namespace NCiphera::NCommands {

    int RunTokens() {
        std::cout << "=== Ciphera Registration Tokens ===" << std::endl;

        const std::string baseDir = "/opt/family-matrix";
        const std::string envPath = baseDir + "/.env";

        if (!NCiphera::NUtils::FileExists(envPath)) {
            std::cerr << "Error: .env not found at " << envPath << std::endl;
            return 1;
        }

        NCiphera::NUtils::TConfig config(envPath);

        std::string adminToken;
        if (config.Has("ADMIN_TOKEN")) {
            adminToken = config.Get("ADMIN_TOKEN");
        } else {
            std::cout << "\nAdmin access token not found in .env" << std::endl;
            std::cout << "Get it from Element Web: Settings -> Help & About -> Advanced -> Access Token" << std::endl;
            std::cout << "\nEnter admin token: ";
            std::getline(std::cin, adminToken);

            if (adminToken.empty()) {
                std::cerr << "Error: empty token" << std::endl;
                return 1;
            }

            config.Set("ADMIN_TOKEN", adminToken);
            config.Save(envPath);
        }

        std::string matrixDomain = config.Get("SERVER_NAME");

        std::cout << "\n[INFO] Creating registration tokens..." << std::endl;

        const char* tokenNames[] = {
            "family-invite-001",
            "family-invite-mom",
            "family-invite-dad",
            "family-invite-sister"
        };

        for (const char* tokenName : tokenNames) {
            std::cout << "\nCreating token: " << tokenName << std::endl;
            std::string curlCmd =
                "curl -s -X POST 'https://" + matrixDomain + "/_synapse/admin/v1/registration_tokens/new' "
                "-H 'Authorization: Bearer " + adminToken + "' "
                "-H 'Content-Type: application/json' "
                "-d '{\"token\": \"" + tokenName + "\", \"uses_allowed\": 1}'";

            NCiphera::NUtils::Run(curlCmd);
        }

        std::cout << "\n=== Tokens created ===" << std::endl;
        std::cout << "\nList all tokens:" << std::endl;
        NCiphera::NUtils::Run(
            "curl -s 'https://" + matrixDomain + "/_synapse/admin/v1/registration_tokens' "
            "-H 'Authorization: Bearer " + adminToken + "' | jq ."
        );

        std::cout << "\nAfter all users registered, disable registration:" << std::endl;
        std::cout << "  1. Edit homeserver.yaml: enable_registration: false" << std::endl;
        std::cout << "  2. Run: docker compose restart synapse" << std::endl;

        return 0;
    }

} // namespace NCiphera::NCommands
