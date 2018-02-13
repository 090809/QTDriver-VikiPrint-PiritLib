#ifndef FISCALCONFIG_H
#define FISCALCONFIG_H

#include <QString>

class FiscalConfig
{
private:
    QString port;
    int speed, type;
public:
    FiscalConfig();
    QString getPort() { return port; }
    void setPort(QString _port) { port = _port; }

    int getSpeed() { return speed; }
    void setSpeed(int _speed) { speed = _speed; }

    int getType() { return type; }
    void setType(int _type) { type = _type; }


};

#endif // FISCALCONFIG_H
