#include "firewall.h"

#include "utils/shell.h"

#include <iostream>

namespace NCiphera::NCommands {

    int RunFirewall() {
        std::cout << "=== Ciphera Firewall Setup ===" << std::endl;

        std::cout << "\n[1/3] Setting default policies..." << std::endl;
        NCiphera::NUtils::SudoRun("ufw default deny incoming");
        NCiphera::NUtils::SudoRun("ufw default allow outgoing");

        std::cout << "\n[2/3] Allowing required ports..." << std::endl;
        NCiphera::NUtils::SudoRun("ufw allow 22/tcp");
        NCiphera::NUtils::SudoRun("ufw allow 80/tcp");
        NCiphera::NUtils::SudoRun("ufw allow 443/tcp");

        NCiphera::NUtils::SudoRun("ufw allow 3478/tcp");
        NCiphera::NUtils::SudoRun("ufw allow 3478/udp");
        NCiphera::NUtils::SudoRun("ufw allow 5349/tcp");
        NCiphera::NUtils::SudoRun("ufw allow 5349/udp");
        NCiphera::NUtils::SudoRun("ufw allow 49160:49200/udp");

        std::cout << "\n[3/3] Enabling firewall..." << std::endl;
        NCiphera::NUtils::SudoRun("sh -c 'echo y | ufw enable'");
        NCiphera::NUtils::SudoRun("ufw status verbose");

        std::cout << "\n=== Firewall configured ===" << std::endl;
        std::cout << "Note: Consider limiting SSH access to your IP:" << std::endl;
        std::cout << "  ufw delete allow 22/tcp" << std::endl;
        std::cout << "  ufw allow from <YOUR_IP> to any port 22 proto tcp" << std::endl;

        return 0;
    }

} // namespace NCiphera::NCommands
