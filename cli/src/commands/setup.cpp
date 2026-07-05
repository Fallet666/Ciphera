#include "setup.h"

#include "utils/shell.h"

#include <iostream>

namespace NCiphera::NCommands {

    int RunSetup() {
        std::cout << "=== Ciphera Setup ===" << std::endl;

        std::cout << "\n[1/4] Updating packages..." << std::endl;
        NCiphera::NUtils::Run("apt update");

        std::cout << "\n[2/4] Installing required packages..." << std::endl;
        NCiphera::NUtils::Run(
            "apt install -y "
            "docker.io docker-compose-plugin nginx certbot python3-certbot-nginx "
            "ufw fail2ban jq curl openssl rsync tar cron"
        );

        std::cout << "\n[3/4] Enabling services..." << std::endl;
        NCiphera::NUtils::Run("systemctl enable --now docker");
        NCiphera::NUtils::Run("systemctl enable --now nginx");
        NCiphera::NUtils::Run("systemctl enable --now fail2ban");

        std::cout << "\n[4/4] Creating project directories..." << std::endl;
        NCiphera::NUtils::CreateDirectory("/opt/family-matrix/synapse");
        NCiphera::NUtils::CreateDirectory("/opt/family-matrix/postgres");
        NCiphera::NUtils::CreateDirectory("/opt/family-matrix/element");
        NCiphera::NUtils::CreateDirectory("/opt/family-matrix/coturn");
        NCiphera::NUtils::CreateDirectory("/opt/family-matrix/backups");
        NCiphera::NUtils::CreateDirectory("/opt/family-matrix/scripts");
        NCiphera::NUtils::CreateDirectory("/opt/family-matrix/secrets");
        NCiphera::NUtils::Run("chmod 700 /opt/family-matrix/secrets");

        std::cout << "\n[INFO] Checking for meterflow..." << std::endl;
        NCiphera::NUtils::Run("docker ps -a | grep -i meterflow || true");
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
