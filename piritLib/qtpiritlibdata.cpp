#include "qtpiritlibdata.h"

QByteArray intToByteArray(int value, bool toString, int base /* = 16 */, bool needJustified /* = false */)
{
    QByteArray s = QByteArray();
    if (toString)
    {
        QString str = QString::number(value, base).toUpper();
        if (needJustified)
            str = str.rightJustified(2, '0');
        s = str.toStdString().c_str();
    } else {
        s.append(value);
    }

    return s;
}

QByteArray doubleToByteArray(double value, bool toString)
{
    QByteArray s = QByteArray();
    if (toString)
    {
        QString str = QString::number(value).toUpper().leftJustified(2, '0');;
        s = str.toStdString().c_str();
    } else {
        s.append(value);
    }

    return s;
}

QByteArray* intToLinkByteArray(int value, bool toString, int base /* = 16 */)
{
    QByteArray* s = new QByteArray();
    if (toString)
    {
        QString str = QString::number(value, base).toUpper().leftJustified(2, '0');;
        for  (int i = 0; i < str.length(); i++)
            s->append(str.at(i));
    } else {
        s->append(value);
    }

    return s;
}

rData::rData(QByteArray r, bool isSimple)
    :simple(isSimple), _r(r)
{
    checkAndParseData(isSimple);
}

bool rData::checkSum()
{
    byte chs = 0;
    if (_r.size() < 9)
    {
        qDebug() << "Битый пакет" << _r.size();
        return false;
    }

    for (int i = 1; i < _r.size() - 2; i++)
        chs ^= _r.at(i);

    QByteArray readedchs = intToByteArray(QString(_r.at(_r.size() - 2)).append(QString(_r.at(_r.size() - 1))).toInt(nullptr, 16));

    return intToByteArray(chs) == readedchs;
}
