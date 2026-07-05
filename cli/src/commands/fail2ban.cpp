#include "fail2ban.h"

#include "utils/shell.h"

#include <iostream>

namespace NCiphera::NCommands {

    int RunFail2ban() {
        std::cout << "=== Ciphera Fail2ban Setup ===" << std::endl;

        const std::string sshdConfig =
            "[sshd]\n"
            "enabled = true\n"
            "port = ssh\n"
            "filter = sshd\n"
            "logpath = %(sshd_log)s\n"
            "maxretry = 4\n"
            "findtime = 10m\n"
            "bantime = 12h\n";

        const std::string nginxConfig =
            "[nginx-limit-req]\n"
            "enabled = true\n"
            "filter = nginx-limit-req\n"
            "logpath = /var/log/nginx/error.log\n"
            "maxretry = 20\n"
            "findtime = 5m\n"
            "bantime = 6h\n";

        std::cout << "\n[1/3] Configuring SSH jail..." << std::endl;
        NCiphera::NUtils::SudoWriteFile("/etc/fail2ban/jail.d/sshd.local", sshdConfig);

        std::cout << "\n[2/3] Configuring nginx rate-limit jail..." << std::endl;
        NCiphera::NUtils::SudoWriteFile("/etc/fail2ban/jail.d/nginx-limit-req.local", nginxConfig);

        std::cout << "\n[3/3] Restarting fail2ban..." << std::endl;
        NCiphera::NUtils::SudoRun("systemctl restart fail2ban");
        NCiphera::NUtils::SudoRun("fail2ban-client status");

        std::cout << "\n=== Fail2ban configured ===" << std::endl;

        return 0;
    }

} // namespace NCiphera::NCommands
