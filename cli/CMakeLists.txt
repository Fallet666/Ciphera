cmake_minimum_required(VERSION 3.16)
project(ciphera LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

set(SOURCES
    src/main.cpp
    src/utils/shell.cpp
    src/utils/config.cpp
    src/commands/setup.cpp
    src/commands/secrets.cpp
    src/commands/deploy.cpp
    src/commands/backup.cpp
    src/commands/health.cpp
    src/commands/update.cpp
    src/commands/tokens.cpp
    src/commands/audit.cpp
    src/commands/firewall.cpp
    src/commands/fail2ban.cpp
)

add_executable(ciphera ${SOURCES})

target_include_directories(ciphera PRIVATE src)
