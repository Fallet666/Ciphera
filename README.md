# Ciphera

Закрытый семейный мессенджер на базе Matrix (Synapse + Element Web + coturn).

## Стек

- **Synapse** — Matrix homeserver
- **PostgreSQL 16** — база данных
- **Element Web** — веб-клиент (PWA)
- **coturn** — TURN/STUN сервер для звонков
- **nginx** — reverse proxy + TLS
- **fail2ban** — защита от brute-force
- **UFW** — firewall

## Домены

| Домен | Назначение |
|---|---|
| `chat.ru.potapovotrap.online` | Element Web (PWA) |
| `matrix.ru.potapovotrap.online` | Synapse API |
| `turn.ru.potapovotrap.online` | TURN сервер |

## Сервер

- IP: `176.109.104.31`
- ОС: Ubuntu/Debian

## Быстрый старт

```bash
git clone git@github.com:Fallet666/Ciphera.git /opt/family-matrix
cd /opt/family-matrix

# Сборка CLI
cd cli
mkdir build && cd build
cmake ..
make
cp ciphera /usr/local/bin/
cd /opt/family-matrix

# Развёртывание
ciphera setup
ciphera secrets
ciphera audit
ciphera firewall
ciphera fail2ban
ciphera deploy
ciphera tokens
```

## CLI команды

```bash
ciphera setup      # Установка пакетов, создание каталогов
ciphera secrets    # Генерация секретов
ciphera audit      # Аудит сервера
ciphera firewall   # Настройка UFW
ciphera fail2ban   # Настройка fail2ban
ciphera deploy     # TLS + nginx + docker compose up
ciphera tokens     # Создание токенов регистрации
ciphera backup     # Бэкап PostgreSQL + media + configs
ciphera health     # Проверка сервисов
ciphera update     # Обновление контейнеров
```

## Документация

- [Пошаговое развёртывание](docs/DEPLOY.md)
- [Инструкция для семьи](docs/FAMILY-GUIDE.md)
- [Границы безопасности](docs/SECURITY.md)
