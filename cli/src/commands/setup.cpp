#include "setup.h"

#include "utils/shell.h"

#include <iostream>

namespace NCiphera::NCommands {

    int RunSetup() {
        std::cout << "=== Ciphera Setup ===" << std::endl;

        std::cout << "\n[1/4] Updating packages..." << std::endl;
        NCiphera::NUtils::SudoRun("apt update");

        std::cout << "\n[2/4] Installing required packages..." << std::endl;
        NCiphera::NUtils::SudoRun(
            "apt install -y "
            "docker.io docker-compose-plugin nginx certbot python3-certbot-nginx "
            "ufw fail2ban jq curl openssl rsync tar cron"
        );

        std::cout << "\n[3/4] Enabling services..." << std::endl;
        NCiphera::NUtils::SudoRun("systemctl enable --now docker");
        NCiphera::NUtils::SudoRun("systemctl enable --now nginx");
        NCiphera::NUtils::SudoRun("systemctl enable --now fail2ban");

        std::cout << "\n[4/4] Creating project directories..." << std::endl;
        NCiphera::NUtils::SudoRun("mkdir -p /opt/family-matrix/synapse");
        NCiphera::NUtils::SudoRun("mkdir -p /opt/family-matrix/postgres");
        NCiphera::NUtils::SudoRun("mkdir -p /opt/family-matrix/element");
        NCiphera::NUtils::SudoRun("mkdir -p /opt/family-matrix/coturn");
        NCiphera::NUtils::SudoRun("mkdir -p /opt/family-matrix/backups");
        NCiphera::NUtils::SudoRun("mkdir -p /opt/family-matrix/scripts");
        NCiphera::NUtils::SudoRun("mkdir -p /opt/family-matrix/secrets");
        NCiphera::NUtils::SudoRun("chmod 700 /opt/family-matrix/secrets");

        std::cout << "\n[INFO] Checking for meterflow..." << std::endl;
        NCiphera::NUtils::SudoRun("docker ps -a | grep -i meterflow || true");
        NCiphera::NUtils::Run("systemctl list-units --type=service | grep -i meterflow || true");
        NCiphera::NUtils::Run("ps aux | grep -i meterflow | grep -v grep || true");

        std::cout << "\n=== Setup complete ===" << std::endl;
        std::cout << "Next steps:" << std::endl;
        std::cout << "  1. Stop meterflow manually if found" << std::endl;
        std::cout << "  2. Run: ciphera secrets" << std::endl;
        std::cout << "  3. Run: ciphera audit" << std::endl;

        return 0;
    }

} // namespace NCiphera::NCommands
