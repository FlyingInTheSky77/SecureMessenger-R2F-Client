#pragma once

#include <QCoreApplication>
#include <QString>

#include <optional>

struct Config {
public:
    QString ip;
    quint16 port;
};

class ConfigLoader
{
public:
    ConfigLoader(const QString& path = "./config.json");

    QString serverIp() const;
    quint16 serverPort() const;
    bool isValid() const;
    std::optional<Config> loadConfig() const;

private:
    QString ip_;
    int port_;
    bool valid_ = false;

    void loadFromFile(const QString& path);
    bool isValidIp(const QString& ip) const;
    bool isValidPort(const int port) const;
};

QString findConfigPath();
