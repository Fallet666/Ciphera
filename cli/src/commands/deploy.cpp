#include "deploy.h"

#include "utils/config.h"
#include "utils/shell.h"

#include <iostream>

namespace NCiphera::NCommands {

    int RunDeploy() {
        std::cout << "=== Ciphera Deployment ===" << std::endl;

        const std::string baseDir = "/opt/family-matrix";
        const std::string envPath = baseDir + "/.env";

        if (!NCiphera::NUtils::FileExists(envPath)) {
            std::cerr << "Error: .env not found. Run 'ciphera secrets' first." << std::endl;
            return 1;
        }

        NCiphera::NUtils::TConfig config(envPath);
        std::string matrixDomain = config.Get("SERVER_NAME");
        std::string chatDomain = config.Get("CHAT_DOMAIN");
        std::string turnDomain = config.Get("TURN_DOMAIN");

        std::cout << "\n[1/5] Obtaining TLS certificates..." << std::endl;
        NCiphera::NUtils::SudoRun("nginx -t");
        NCiphera::NUtils::SudoRun("systemctl reload nginx");
        NCiphera::NUtils::SudoRun(
            "certbot certonly --nginx "
            "-d " + matrixDomain + " "
            "-d " + chatDomain + " "
            "-d " + turnDomain + " "
            "--non-interactive --agree-tos --email admin@" + matrixDomain
        );
        NCiphera::NUtils::SudoRun("certbot renew --dry-run");

        std::cout << "\n[2/5] Configuring nginx..." << std::endl;
        std::string nginxConf =
            "limit_req_zone $binary_remote_addr zone=matrix_login:10m rate=5r/m;\n"
            "limit_req_zone $binary_remote_addr zone=matrix_api:20m rate=20r/s;\n"
            "limit_conn_zone $binary_remote_addr zone=addr:10m;\n\n"
            "server {\n"
            "    listen 80;\n"
            "    server_name " + matrixDomain + " " + chatDomain + " " + turnDomain + ";\n"
            "    return 301 https://$host$request_uri;\n"
            "}\n\n"
            "server {\n"
            "    listen 443 ssl http2;\n"
            "    server_name " + matrixDomain + ";\n\n"
            "    ssl_certificate /etc/letsencrypt/live/" + matrixDomain + "/fullchain.pem;\n"
            "    ssl_certificate_key /etc/letsencrypt/live/" + matrixDomain + "/privkey.pem;\n\n"
            "    client_max_body_size " + config.Get("MAX_UPLOAD_SIZE", "200M") + ";\n\n"
            "    add_header X-Frame-Options DENY always;\n"
            "    add_header X-Content-Type-Options nosniff always;\n"
            "    add_header Referrer-Policy no-referrer always;\n"
            "    server_tokens off;\n\n"
            "    location ~ ^/_matrix/client/(r0|v3|unstable)/(login|register) {\n"
            "        limit_req zone=matrix_login burst=10 nodelay;\n"
            "        proxy_pass http://127.0.0.1:8008;\n"
            "        proxy_set_header X-Forwarded-For $remote_addr;\n"
            "        proxy_set_header X-Forwarded-Proto $scheme;\n"
            "        proxy_set_header Host $host;\n"
            "    }\n\n"
            "    location ~ ^(/_matrix|/_synapse/client) {\n"
            "        limit_req zone=matrix_api burst=100 nodelay;\n"
            "        limit_conn addr 30;\n"
            "        proxy_pass http://127.0.0.1:8008;\n"
            "        proxy_set_header X-Forwarded-For $remote_addr;\n"
            "        proxy_set_header X-Forwarded-Proto $scheme;\n"
            "        proxy_set_header Host $host;\n"
            "        proxy_read_timeout 600s;\n"
            "        proxy_send_timeout 600s;\n"
            "    }\n"
            "}\n\n"
            "server {\n"
            "    listen 443 ssl http2;\n"
            "    server_name " + chatDomain + ";\n\n"
            "    ssl_certificate /etc/letsencrypt/live/" + matrixDomain + "/fullchain.pem;\n"
            "    ssl_certificate_key /etc/letsencrypt/live/" + matrixDomain + "/privkey.pem;\n\n"
            "    add_header X-Frame-Options DENY always;\n"
            "    add_header X-Content-Type-Options nosniff always;\n"
            "    add_header Referrer-Policy no-referrer always;\n"
            "    server_tokens off;\n\n"
            "    location / {\n"
            "        proxy_pass http://127.0.0.1:8081;\n"
            "        proxy_set_header Host $host;\n"
            "        proxy_set_header X-Forwarded-Proto $scheme;\n"
            "        proxy_set_header X-Forwarded-For $remote_addr;\n"
            "    }\n"
            "}\n";

        NCiphera::NUtils::SudoWriteFile("/etc/nginx/sites-available/family-matrix.conf", nginxConf);
        NCiphera::NUtils::SudoRun("ln -sf /etc/nginx/sites-available/family-matrix.conf /etc/nginx/sites-enabled/family-matrix.conf");
        NCiphera::NUtils::SudoRun("nginx -t");
        NCiphera::NUtils::SudoRun("systemctl reload nginx");

        std::cout << "\n[3/5] Generating Synapse config..." << std::endl;
        NCiphera::NUtils::SudoRun(
            "docker run --rm "
            "-v " + baseDir + "/synapse:/data "
            "-e SYNAPSE_SERVER_NAME=" + matrixDomain + " "
            "-e SYNAPSE_REPORT_STATS=no "
            "matrixdotorg/synapse:latest generate"
        );

        std::cout << "\n[4/5] Starting services..." << std::endl;
        NCiphera::NUtils::SudoRun("cd " + baseDir + " && docker compose up -d");
        NCiphera::NUtils::SudoRun("cd " + baseDir + " && docker compose ps");

        std::cout << "\n[5/5] Verifying deployment..." << std::endl;
        NCiphera::NUtils::Run("curl -s https://" + matrixDomain + "/_matrix/client/versions | jq .");
        NCiphera::NUtils::Run("curl -I https://" + chatDomain);

        std::cout << "\n=== Deployment complete ===" << std::endl;
        std::cout << "\nNext steps:" << std::endl;
        std::cout << "  1. Edit homeserver.yaml with secrets from .env" << std::endl;
        std::cout << "  2. Edit turnserver.conf with TURN secret" << std::endl;
        std::cout << "  3. Restart: docker compose restart" << std::endl;
        std::cout << "  4. Create admin: docker exec -it matrix-synapse register_new_matrix_user -c /data/homeserver.yaml http://localhost:8008 --admin" << std::endl;
        std::cout << "  5. Run: ciphera tokens" << std::endl;

        return 0;
    }

} // namespace NCiphera::NCommands
