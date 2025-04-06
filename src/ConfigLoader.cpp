#include "include/ConfigLoader.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegExpValidator>

ConfigLoader::ConfigLoader(const QString& path)
{
    loadFromFile(path);
}

void ConfigLoader::loadFromFile(const QString& path)
{
    QFile file(path);
    qDebug() << "path: " << path;
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Can't open config:" << path;
        valid_ = false;
        return;
    }

    QByteArray raw = file.readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(raw, &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Parcing error:" << error.errorString();
        valid_ = false;
        return;
    }

    QJsonObject obj = doc.object();
    if (!obj.contains("server") || !obj["server"].isObject()) {
        valid_ = false;
        return;
    }

    QJsonObject server = obj.value("server").toObject();

    if (!server.contains("ip") || !server.contains("port")) {
        valid_ = false;
        return;
    }

    ip_ = server.value("ip").toString();
    port_ = server.value("port").toInt();

    if (!isValidIp(ip_)) {
        qWarning() << "Invalid IP-address:" << ip_;
        valid_ = false;
        return;
    }

    if (!isValidPort(port_)) {
        qWarning() << "Invalid port:" << port_;
        valid_ = false;
        return;
    }
    qDebug() << "Correct port:" << port_;
    valid_ = true;
}

bool ConfigLoader::isValidIp(const QString& ip) const
{
    QRegExp ipv4Rx(
        "(\\b25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(\\b25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(\\b25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(\\b25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b"
    );

    QRegExp ipv6Rx(
        "([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}|"
        "::([0-9a-fA-F]{1,4}:){1,7}[0-9a-fA-F]{1,4}|"
        "([0-9a-fA-F]{1,4}:){1,6}:|"
        "([0-9a-fA-F]{1,4}:){1,5}:([0-9a-fA-F]{1,4}:){1,3}[0-9a-fA-F]{1,4}|"
        "([0-9a-fA-F]{1,4}:){1,4}:([0-9a-fA-F]{1,4}:){1,2}[0-9a-fA-F]{1,4}|"
        "([0-9a-fA-F]{1,4}:){1,3}:([0-9a-fA-F]{1,4}:)[0-9a-fA-F]{1,4}|"
        "([0-9a-fA-F]{1,4}:){1,2}:([0-9a-fA-F]{1,4}:)[0-9a-fA-F]{1,4}|"
        "([0-9a-fA-F]{1,4}:){1}:([0-9a-fA-F]{1,4}:)[0-9a-fA-F]{1,4}|"
        "::([0-9a-fA-F]{1,4}:)[0-9a-fA-F]{1,4}"
    );

    if (ipv4Rx.exactMatch(ip)) {
        qDebug() << "ip4: " << ip;
        return true;
    }

    if (ipv6Rx.exactMatch(ip)) {
        qDebug() << "ip6: " << ip;
        return true;
    }
    qWarning() << "Invalid IP: " << ip;
    return false;
}

bool ConfigLoader::isValidPort(const int port) const
{
    return port >= 1 && port <= 65535;
}

bool ConfigLoader::isValid() const {
    return valid_;
}

std::optional<Config> ConfigLoader::loadConfig() const
{
    if (!isValid()) {
        return std::nullopt;
    }

    Config config;
    config.ip = serverIp();
    config.port = serverPort();
    return config;
}

QString ConfigLoader::serverIp() const {
    return ip_;
}
quint16 ConfigLoader::serverPort() const {
    return static_cast<quint16>(port_);
}

QString findConfigPath() {
#ifdef QT_DEBUG
    return QDir::cleanPath( QCoreApplication::applicationDirPath() + "/../SecureMessenger-R2F-Client/config.json" );
#else
    return QDir::cleanPath( QCoreApplication::applicationDirPath() + "/config.json" );
#endif
}
