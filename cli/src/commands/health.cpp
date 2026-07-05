#include "health.h"

#include "utils/config.h"
#include "utils/shell.h"

#include <iostream>

namespace NCiphera::NCommands {

    int RunHealth() {
        std::cout << "=== Ciphera Health Check ===" << std::endl;

        const std::string baseDir = "/opt/family-matrix";
        const std::string envPath = baseDir + "/.env";

        std::string matrixDomain = "api.potapovotrap.online";
        std::string chatDomain = "ciphera.potapovotrap.online";

        if (NCiphera::NUtils::FileExists(envPath)) {
            NCiphera::NUtils::TConfig config(envPath);
            matrixDomain = config.Get("SERVER_NAME", matrixDomain);
            chatDomain = config.Get("CHAT_DOMAIN", chatDomain);
        }

        int failures = 0;

        std::cout << "\n[1/6] Checking Synapse API..." << std::endl;
        if (NCiphera::NUtils::Run("curl -fsS https://" + matrixDomain + "/_matrix/client/versions >/dev/null 2>&1") == 0) {
            std::cout << "  OK: Synapse API responding" << std::endl;
        } else {
            std::cout << "  FAIL: Synapse API not responding" << std::endl;
            failures++;
        }

        std::cout << "\n[2/6] Checking Element Web..." << std::endl;
        if (NCiphera::NUtils::Run("curl -fsSI https://" + chatDomain + " >/dev/null 2>&1") == 0) {
            std::cout << "  OK: Element Web responding" << std::endl;
        } else {
            std::cout << "  FAIL: Element Web not responding" << std::endl;
            failures++;
        }

        std::cout << "\n[3/6] Checking containers..." << std::endl;
        const char* containers[] = {"matrix-synapse", "matrix-postgres", "matrix-element-web", "matrix-coturn"};
        for (const char* container : containers) {
            std::string cmd = "docker ps --format '{{.Names}} {{.Status}}' | grep " + std::string(container);
            if (NCiphera::NUtils::SudoRun(cmd) == 0) {
                std::cout << "  OK: " << container << " running" << std::endl;
            } else {
                std::cout << "  FAIL: " << container << " not running" << std::endl;
                failures++;
            }
        }

        std::cout << "\n[4/6] Checking fail2ban..." << std::endl;
        NCiphera::NUtils::SudoRun("fail2ban-client status");

        std::cout << "\n[5/6] Checking firewall..." << std::endl;
        NCiphera::NUtils::SudoRun("ufw status");

        std::cout << "\n[6/6] Checking disk space..." << std::endl;
        NCiphera::NUtils::Run("df -h /opt/family-matrix");

        std::cout << "\n=== Health check complete ===" << std::endl;
        if (failures > 0) {
            std::cout << "FAILURES: " << failures << std::endl;
            return 1;
        }
        std::cout << "All checks passed!" << std::endl;

        return 0;
    }

} // namespace NCiphera::NCommands
