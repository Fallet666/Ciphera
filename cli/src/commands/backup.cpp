#include "backup.h"

#include "utils/shell.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace NCiphera::NCommands {

    int RunBackup() {
        std::cout << "=== Ciphera Backup ===" << std::endl;

        const std::string baseDir = "/opt/family-matrix";
        const std::string backupDir = baseDir + "/backups";

        NCiphera::NUtils::CreateDirectory(backupDir);

        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        std::ostringstream dateStream;
        dateStream << std::put_time(localTime, "%Y-%m-%d_%H-%M-%S");
        std::string timestamp = dateStream.str();

        std::string backupName = "matrix-backup-" + timestamp;
        std::string backupPath = backupDir + "/" + backupName;

        NCiphera::NUtils::CreateDirectory(backupPath);

        std::cout << "\n[1/3] Dumping PostgreSQL database..." << std::endl;
        NCiphera::NUtils::SudoRun(
            "docker exec matrix-postgres pg_dump -U synapse synapse > " + backupPath + "/synapse.sql"
        );

        std::cout << "\n[2/3] Archiving configs and media..." << std::endl;
        NCiphera::NUtils::SudoRun(
            "tar -czf " + backupPath + "/synapse-config-media.tar.gz "
            "-C " + baseDir + " "
            "synapse/homeserver.yaml "
            "synapse/media_store "
            "element/config.json "
            "coturn/turnserver.conf "
            "secrets "
            "2>/dev/null || true"
        );

        std::cout << "\n[3/3] Creating final archive..." << std::endl;
        NCiphera::NUtils::SudoRun(
            "tar -czf " + backupDir + "/" + backupName + ".tar.gz "
            "-C " + backupDir + " " + backupName
        );
        NCiphera::NUtils::SudoRun("rm -rf " + backupPath);

        std::cout << "\n[CLEANUP] Removing old backups (>14 days)..." << std::endl;
        NCiphera::NUtils::SudoRun(
            "find " + backupDir + " -type f -name 'matrix-backup-*.tar.gz' -mtime +14 -delete"
        );

        std::cout << "\n=== Backup complete ===" << std::endl;
        std::cout << "Archive: " << backupDir + "/" + backupName + ".tar.gz" << std::endl;

        NCiphera::NUtils::Run("ls -lh " + backupDir + "/" + backupName + ".tar.gz");

        return 0;
    }

} // namespace NCiphera::NCommands
