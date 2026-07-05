# Развёртывание Ciphera

Пошаговая инструкция развёртывания семейного Matrix-мессенджера на сервере 176.109.104.31.

## Требования

- Ubuntu/Debian сервер
- Root доступ
- DNS записи:
  - `matrix.ru.potapovotrap.online` → 176.109.104.31
  - `chat.ru.potapovotrap.online` → 176.109.104.31
  - `turn.ru.potapovotrap.online` → 176.109.104.31

## Шаги развёртывания

### 1. Клонирование репозитория

```bash
ssh root@176.109.104.31
git clone git@github.com:Fallet666/Ciphera.git /opt/family-matrix
cd /opt/family-matrix
```

### 2. Сборка CLI

```bash
cd cli
mkdir build && cd build
cmake ..
make
cp ciphera /usr/local/bin/
```

### 3. Первоначальная настройка

```bash
ciphera setup
```

Устанавливает пакеты, создаёт каталоги, проверяет meterflow.

### 4. Аудит сервера

```bash
ciphera audit
```

Сохраняет состояние сервера в `/root/pre-matrix-audit/`.

**Важно:** если найден meterflow — остановите его вручную перед продолжением.

### 5. Генерация секретов

```bash
ciphera secrets
```

Генерирует пароли и сохраняет в:
- `/opt/family-matrix/secrets/postgres_password`
- `/opt/family-matrix/secrets/registration_shared_secret`
- `/opt/family-matrix/secrets/turn_shared_secret`
- `/opt/family-matrix/.env`

### 6. Настройка firewall

```bash
ciphera firewall
```

Настраивает UFW: открывает SSH, HTTP, HTTPS, TURN порты.

### 7. Настройка fail2ban

```bash
ciphera fail2ban
```

Настраивает защиту от brute-force для SSH и nginx rate-limit.

### 8. Развёртывание

```bash
ciphera deploy
```

- Получает TLS сертификаты через certbot
- Настраивает nginx reverse proxy
- Генерирует Synapse config
- Запускает Docker контейнеры

### 9. Редактирование конфигов

После deploy нужно подставить секреты в конфиги:

```bash
# Прочитать секреты
cat /opt/family-matrix/secrets/postgres_password
cat /opt/family-matrix/secrets/registration_shared_secret
cat /opt/family-matrix/secrets/turn_shared_secret

# Отредактировать homeserver.yaml
nano /opt/family-matrix/synapse/homeserver.yaml
# Заменить: PASTE_POSTGRES_PASSWORD_HERE, PASTE_REGISTRATION_SHARED_SECRET_HERE, PASTE_TURN_SHARED_SECRET_HERE

# Отредактировать turnserver.conf
nano /opt/family-matrix/config/coturn/turnserver.conf
# Заменить: PASTE_TURN_SHARED_SECRET_HERE

# Скопировать конфиги
cp /opt/family-matrix/config/coturn/turnserver.conf /opt/family-matrix/coturn/turnserver.conf
cp /opt/family-matrix/config/element/config.json /opt/family-matrix/element/config.json

# Перезапустить
cd /opt/family-matrix
docker compose restart
```

### 10. Создание администратора

```bash
docker exec -it matrix-synapse register_new_matrix_user \
  -c /data/homeserver.yaml \
  http://localhost:8008 \
  --admin
```

Введите:
- username: `lesha` (или другой)
- password: длинный случайный пароль
- admin: `yes`

### 11. Создание токенов регистрации

1. Войдите в Element Web: https://chat.ru.potapovotrap.online
2. Получите access token: Settings → Help & About → Advanced → Access Token
3. Добавьте токен в .env:

```bash
nano /opt/family-matrix/.env
# ADMIN_TOKEN=ваш_токен
```

4. Создайте токены:

```bash
ciphera tokens
```

### 12. Проверка

```bash
ciphera health
```

Проверяет:
- Synapse API
- Element Web
- Docker контейнеры
- fail2ban
- firewall

## Эксплуатация

### Бэкапы

```bash
ciphera backup
```

Создаёт архив с PostgreSQL dump + media + configs.

Автоматический бэкап через cron:
```bash
crontab -e
# Добавить: 30 4 * * * /usr/local/bin/ciphera backup >/var/log/family-matrix-backup.log 2>&1
```

### Обновления

```bash
ciphera update
```

Автоматически создаёт бэкап, обновляет образы и перезапускает контейнеры.

### Health check

```bash
ciphera health
```

Автоматическая проверка через cron:
```bash
crontab -e
# Добавить: */10 * * * * /usr/local/bin/ciphera health >/tmp/family-matrix-health.log 2>&1
```

## Приёмочные критерии

- [ ] https://chat.ru.potapovotrap.online открывается по HTTPS
- [ ] https://matrix.ru.potapovotrap.online/_matrix/client/versions отдаёт JSON
- [ ] Регистрация без токена невозможна
- [ ] Регистрация с одноразовым токеном работает
- [ ] Создан admin-пользователь
- [ ] Созданы тестовые пользователи
- [ ] Между пользователями создан encrypted direct chat
- [ ] Отправляется текст, фото, видео, аудио
- [ ] fail2ban работает
- [ ] TURN проходит базовую проверку
- [ ] Бэкап создаётся
- [ ] После reboot все сервисы стартуют автоматически
