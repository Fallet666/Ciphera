#include "update.h"

#include "backup.h"
#include "utils/shell.h"

#include <iostream>

namespace NCiphera::NCommands {

    int RunUpdate() {
        std::cout << "=== Ciphera Update ===" << std::endl;

        const std::string baseDir = "/opt/family-matrix";

        std::cout << "\n[1/4] Creating backup before update..." << std::endl;
        int backupResult = RunBackup();
        if (backupResult != 0) {
            std::cerr << "Warning: backup failed, but continuing with update" << std::endl;
        }

        std::cout << "\n[2/4] Pulling latest images..." << std::endl;
        NCiphera::NUtils::SudoRun("cd " + baseDir + " && docker compose pull");

        std::cout << "\n[3/4] Recreating containers..." << std::endl;
        NCiphera::NUtils::SudoRun("cd " + baseDir + " && docker compose up -d");

        std::cout << "\n[4/4] Cleaning up old images..." << std::endl;
        NCiphera::NUtils::SudoRun("docker image prune -f");

        std::cout << "\n[VERIFY] Checking services..." << std::endl;
        NCiphera::NUtils::SudoRun("cd " + baseDir + " && docker compose ps");

        std::cout << "\n=== Update complete ===" << std::endl;

        return 0;
    }

} // namespace NCiphera::NCommands
