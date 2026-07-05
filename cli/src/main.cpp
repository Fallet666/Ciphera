#include "commands/audit.h"
#include "commands/backup.h"
#include "commands/deploy.h"
#include "commands/fail2ban.h"
#include "commands/firewall.h"
#include "commands/health.h"
#include "commands/secrets.h"
#include "commands/setup.h"
#include "commands/tokens.h"
#include "commands/update.h"

#include <iostream>
#include <string>

void PrintUsage() {
    std::cout << "Ciphera CLI - Matrix messenger deployment tool\n\n";
    std::cout << "Usage: ciphera <command>\n\n";
    std::cout << "Commands:\n";
    std::cout << "  setup      Install packages, create directories, stop meterflow\n";
    std::cout << "  secrets    Generate secrets from .env.example\n";
    std::cout << "  audit      Pre-deployment server audit\n";
    std::cout << "  firewall   Configure UFW firewall\n";
    std::cout << "  fail2ban   Configure fail2ban\n";
    std::cout << "  tls        Obtain TLS certificates via certbot\n";
    std::cout << "  deploy     Full deployment (TLS + nginx + docker compose up)\n";
    std::cout << "  tokens     Create registration tokens\n";
    std::cout << "  backup     Create backup of PostgreSQL + media + configs\n";
    std::cout << "  health     Check services health\n";
    std::cout << "  update     Update containers\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        PrintUsage();
        return 1;
    }

    std::string command = argv[1];

    if (command == "setup") {
        return NCiphera::NCommands::RunSetup();
    } else if (command == "secrets") {
        return NCiphera::NCommands::RunSecrets();
    } else if (command == "audit") {
        return NCiphera::NCommands::RunAudit();
    } else if (command == "firewall") {
        return NCiphera::NCommands::RunFirewall();
    } else if (command == "fail2ban") {
        return NCiphera::NCommands::RunFail2ban();
    } else if (command == "deploy") {
        return NCiphera::NCommands::RunDeploy();
    } else if (command == "tokens") {
        return NCiphera::NCommands::RunTokens();
    } else if (command == "backup") {
        return NCiphera::NCommands::RunBackup();
    } else if (command == "health") {
        return NCiphera::NCommands::RunHealth();
    } else if (command == "update") {
        return NCiphera::NCommands::RunUpdate();
    } else if (command == "help" || command == "--help" || command == "-h") {
        PrintUsage();
        return 0;
    } else {
        std::cerr << "Unknown command: " << command << "\n\n";
        PrintUsage();
        return 1;
    }
}
