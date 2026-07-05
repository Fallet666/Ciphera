#include "audit.h"

#include "utils/shell.h"

#include <iostream>

namespace NCiphera::NCommands {

    int RunAudit() {
        std::cout << "=== Ciphera Pre-Deployment Audit ===" << std::endl;

        const std::string auditDir = "/root/pre-matrix-audit";
        NCiphera::NUtils::SudoRun("mkdir -p " + auditDir);

        std::cout << "\n[1/7] Collecting system info..." << std::endl;
        NCiphera::NUtils::SudoRun("sh -c 'date > " + auditDir + "/date.txt'");
        NCiphera::NUtils::SudoRun("sh -c 'hostnamectl > " + auditDir + "/hostnamectl.txt'");
        NCiphera::NUtils::SudoRun("sh -c 'ip a > " + auditDir + "/ip-a.txt'");

        std::cout << "\n[2/7] Collecting network info..." << std::endl;
        NCiphera::NUtils::SudoRun("sh -c 'ss -tulpn > " + auditDir + "/ss-tulpn.txt'");

        std::cout << "\n[3/7] Collecting Docker info..." << std::endl;
        NCiphera::NUtils::SudoRun("sh -c 'docker ps -a > " + auditDir + "/docker-ps-a.txt 2>/dev/null || true'");

        std::cout << "\n[4/7] Collecting running services..." << std::endl;
        NCiphera::NUtils::SudoRun("sh -c 'systemctl list-units --type=service --state=running > " + auditDir + "/services-running.txt'");

        std::cout << "\n[5/7] Collecting nginx config..." << std::endl;
        NCiphera::NUtils::SudoRun("sh -c 'nginx -T > " + auditDir + "/nginx-before.conf 2>&1 || true'");

        std::cout << "\n[6/7] Searching for meterflow..." << std::endl;
        NCiphera::NUtils::SudoRun("sh -c 'find /opt /srv /var/www -maxdepth 3 -iname \"*meterflow*\" 2>/dev/null > " + auditDir + "/meterflow-search.txt'");

        std::cout << "\n[7/7] Audit complete ===" << std::endl;
        std::cout << "Audit files saved to: " << auditDir << std::endl;
        std::cout << "\nReview the files and stop meterflow if found before proceeding." << std::endl;

        return 0;
    }

} // namespace NCiphera::NCommands
