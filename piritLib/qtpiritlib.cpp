#include "qtpiritlib.h"
#include "qtpiritlibdata.h"
#include <QObject>
#include <qobjectdefs.h>
#include <QByteArray>
#include <QDateTime>
#include <QtSerialPort>
#include <QDebug>

qConnector* qConnector::sefl = nullptr;

template <typename T>
T qConnector::sendRequest(byte commandId, sData* data, bool isSimple)
{
    if (packetId < 0x40 || packetId > 0x80)
        packetId = 0x40;
    else packetId++;

    QByteArray* spack;

    if (!isSimple)
        spack = (new sPacket(commandId, data, packetId))->toByteArray();
    else
        spack = intToLinkByteArray(commandId, false);

    const quint64 bytesWritten = sp->write(*spack);

    qDebug() << "Written bytes:" << QString(*spack) <<"; bytesCount:" << bytesWritten;

    if (bytesWritten == (quint64)-1) {
        qDebug() << QObject::tr("Failed to write the data, error: %1")
                          .arg(sp->errorString());
    } else if (bytesWritten != (quint64)spack->size()) {
        qDebug() << QObject::tr("Failed to write all the data, error: %1")
                          .arg(sp->errorString());
    }

    //scroll never returns the pong
    if (commandId != 0x0A)
    {
        QEventLoop* loop = new QEventLoop();
        connect(this, &qConnector::readDone, loop, &QEventLoop::quit);
        loop->exec();
        delete loop;
    }

    qDebug() << "ReadedData " << readedData;
    return T(readedData);
}

QByteArray sPacket::calculateChkSum()
{
    byte crc = 0;
    for (int i = 0; i < pass.size(); i++)
        crc ^= pass.at(i);
    QByteArray s = intToByteArray(32, false);
    for (int i = 0; i < s.size(); i++)
        crc ^= s.at(i);
    for (int i = 0; i < command.size(); i++)
        crc ^= command.at(i);
    QByteArray sdata = data->toByteArray();
    for (int i = 0; i < sdata.size(); i++)
        crc ^= sdata.at(i);
    crc ^= endByte;

    return intToByteArray(crc, true, 16, true);
}

void sPacket::setCommand(int commandId)
{
    command = intToByteArray(commandId);
}

QByteArray* sPacket::toByteArray()
{
    QByteArray* ret = new QByteArray();
    
    ret->append(intToByteArray(startByte, false));
    ret->append(pass);
    ret->append(intToByteArray(32, false));
    ret->append(command);
    if (data->toByteArray().length())
        ret->append(data->toByteArray());
    ret->append(intToByteArray(endByte, false));
    ret->append(calculateChkSum());

    return ret;
}

int qConnector::openPort(QString port, long baudrate)
{
    qDebug() << "openPort" << port << baudrate;

    sp->setPortName(port);
    sp->setBaudRate(baudrate, QSerialPort::AllDirections);
    sp->setParity(QSerialPort::NoParity);
    sp->setStopBits(QSerialPort::OneStop);
    sp->setFlowControl(QSerialPort::HardwareControl);

    return sp->open(QSerialPort::ReadWrite);
}

void qConnector::commandStart()
{
    qDebug() << "commandStart Execute";

    //s_checkStatus* scs = new s_checkStatus();
    //r_status rs = sendRequest<r_status>(0x05, scs, true);
    //if(rs._r !)
    //{
    //    qDebug() << "rs.ACK: " << rs.ACK;
    //    qDebug() << "need ACK: " << 0x6;
    //    qDebug() << "Аппарат не на связи";
    //    return;
    //}

    s_commandStart* scm = new s_commandStart(QDate::currentDate().toString("ddMMyy").toStdString().c_str(), QTime::currentTime().toString("hhmmss").toStdString().c_str());
    sendRequest<void>(0x10, scm);
}

void qConnector::scrollPaper()
{
    qDebug() << "scrollPaper Execute";

    s_scrollPaper* scp = new s_scrollPaper();
    sendRequest<void>(0x0A, scp, true);
}

int qConnector::getStatusFlags(int *fatalStatus, int *currentFlagsStatus, int *documentStatus)
{
    qDebug() << "GetStatusFlags Execute";

    s_statusFlags* ssf = new s_statusFlags();
    r_statusFlags rsf = sendRequest<r_statusFlags>(0x00, ssf);

    if (rsf.getError() != 0) {
        int err = rsf.getError();
        if (err == 1)
        {
            r_getAdvError s = sConnector->sendRequest<r_getAdvError>(0x6, new s_getAdvErr());
            qDebug() << "1h Ошибка:" << s.error;
        }
        return err;
    }

    *fatalStatus = rsf.fatalStatus;
    *currentFlagsStatus = rsf.currentFlagsStatus;
    *documentStatus = rsf.documentStatus;

    return rsf.getError();
}

int qConnector::printZReport(QByteArray nameCashier, int options)
{
    qDebug() << "printZReport Execute";

    s_printZReport* spzr = new s_printZReport(nameCashier, options);
    r_printZReport rpzr= sendRequest<r_printZReport>(0x21, spzr);
    int err = rpzr.getError();
    if (err == 1)
    {
        r_getAdvError s = sConnector->sendRequest<r_getAdvError>(0x6, new s_getAdvErr());
        qDebug() << "1h Ошибка:" << s.error;
        switch (s.error)
        {
        case 9: closeDocument(0); printZReport(nameCashier, options);
        default:
            break;
        }
    }
    return err;
}

void qConnector::closeDocument(byte cutPaper)
{
    qDebug() << "closeDocument Execute";
    sendRequest<r_closeDocument>(0x31,  new s_closeDoc(cutPaper));
}

int qConnector::openDocument(byte type, byte numDepart, QByteArray nameCashier, long docNumber)
{
    qDebug() << "openDocument Execute";

    s_openDocument* sod = new s_openDocument(type, numDepart, nameCashier, docNumber);
    r_openDocument r = sendRequest<r_openDocument>(0x30, sod);
    int err = r.getError();
    if (err == 1)
    {
        r_getAdvError s = sConnector->sendRequest<r_getAdvError>(0x6, new s_getAdvErr(), false);
        qDebug() << "1h Ошибка:" << s.error;
    }
    return err;
}

int qConnector::printString(QByteArray textStr, byte attribute)
{
    qDebug() << "printString Execute";

    s_printString* sps = new s_printString(textStr, attribute);
    r_printString rps = sendRequest<r_printString>(0x40, sps);
    int err = rps.getError();
    if (err == 1)
    {
        r_getAdvError s = sConnector->sendRequest<r_getAdvError>(0x6, new s_getAdvErr(), false);
        qDebug() << "1h Ошибка:" << s.error;
    }
    return err;
}

int qConnector::printBarCode(byte posText, byte widthBarCode, byte heightBarCode, byte typeBarCode, QByteArray barCode)
{
    qDebug() << "printBarCode Execute";

    s_printBarCode* spbc = new s_printBarCode(posText, widthBarCode, heightBarCode, typeBarCode, barCode);
    r_printBarCode rpbc = sendRequest<r_printBarCode>(0x41, spbc);
    int err = rpbc.getError();
    if (err == 1)
    {
        r_getAdvError s = sConnector->sendRequest<r_getAdvError>(0x6, new s_getAdvErr(), false);
        qDebug() << "1h Ошибка:" << s.error;
    }
    return err;
}

int qConnector::addPosition(QByteArray goodsName, QByteArray barcode, double quantity, double price, byte taxNumber, int numGoodsPos, byte numDepart, byte coefType, QByteArray coefName, double coefValue)
{
    qDebug() << "addPosition Execute" << goodsName << barcode;

    s_addPosition* sap = new s_addPosition();
    sap->goodsName = goodsName;
    sap->barcode = barcode;
    sap->quantity = quantity;
    sap->price = price;
    sap->taxNumber = taxNumber;
    sap->numGoodsPos = numGoodsPos;
    sap->numDepart = numDepart;
    sap->coefType = coefType;
    sap->coefName = coefName;
    sap->coefValue = coefValue;
    r_addPosition rap = sendRequest<r_addPosition>(0x42, sap);
    int err = rap.getError();
    if (err == 1)
    {
        r_getAdvError s = sConnector->sendRequest<r_getAdvError>(0x6, new s_getAdvErr(), false);
        qDebug() << "1h Ошибка:" << s.error;
    }
    return err;
}

int qConnector::printSubTotal()
{
    qDebug() << "printSubTotal Execute";

    s_printSubTotal* spst = new s_printSubTotal();
    r_printSubTotal rpst = sendRequest<r_printSubTotal>(0x44, spst);
    int err = rpst.getError();
    if (err == 1)
    {
        r_getAdvError s = sConnector->sendRequest<r_getAdvError>(0x6, new s_getAdvErr(), false);
        qDebug() << "1h Ошибка:" << s.error;
    }
    return err;
}

int qConnector::addDiscount(byte typeDiscount, QByteArray nameDiscount, long sum)
{
    qDebug() << "addDiscount Execute";

    s_addDiscount* sad = new s_addDiscount(typeDiscount, nameDiscount, sum);
    r_addDiscount rad = sendRequest<r_addDiscount>(0x45, sad);
    int err = rad.getError();
    if (err == 1)
    {
        r_getAdvError s = sConnector->sendRequest<r_getAdvError>(0x6, new s_getAdvErr(), false);
        qDebug() << "1h Ошибка:" << s.error;
    }
    return err;
}

void qConnector::printServiceData()
{
    qDebug() << "printServiceData Execute";
    sendRequest<void>(0x94, new s_printServiceData(), false);
}

int qConnector::openShift(QByteArray nameCashier)
{
    qDebug() << "openShift Execute";
    r_openShift ros = sendRequest<r_openShift>(0x20, new s_openShift(nameCashier));
    int err = ros.getError();
    if (err == 1)
    {
        r_getAdvError s = sConnector->sendRequest<r_getAdvError>(0x6, new s_getAdvErr(), false);
        qDebug() << "1h Ошибка:" << s.error;
    }
    return err;
}

int qConnector::addPaymentD(byte typePayment, double sum, QByteArray infoStr)
{
    qDebug() << "addPaymentD Execute";

    s_addPaymentD* sapd = new s_addPaymentD(typePayment, sum, infoStr);
    r_addPaymentD rapd = sendRequest<r_addPaymentD>(0x47, sapd);
    int err = rapd.getError();
    if (err == 1)
    {
        r_getAdvError s = sConnector->sendRequest<r_getAdvError>(0x6, new s_getAdvErr(), false);
        qDebug() << "1h Ошибка:" << s.error;
    }
    return err;
}

extern int  openPort(QString fileName, long speed) { return sConnector->openPort(fileName, speed); }
extern int  commandStart()  { sConnector->commandStart(); return true; }
extern int  scrollPaper() { sConnector->scrollPaper(); return true; }
extern int  getStatusFlags(int *fatalStatus, int *currentFlagsStatus, int *documentStatus)
    { return sConnector->getStatusFlags(fatalStatus, currentFlagsStatus, documentStatus);}
extern int  libPrintZReport(QByteArray nameCashier, int options) { return sConnector->printZReport(nameCashier, options); }
extern int  libOpenDocument(byte type, byte numDepart, QByteArray nameCashier, long docNumber) { return sConnector->openDocument(type, numDepart, nameCashier, docNumber); }
extern void libCloseDocument(byte cutPaper) { sConnector->closeDocument(cutPaper); }
extern int  libPrintString(QByteArray textStr, byte attribute) { return sConnector->printString(textStr, attribute); }
extern int  libPrintBarCode(byte posText, byte widthBarCode, byte heightBarCode, byte typeBarCode, QByteArray barCode) { return sConnector->printBarCode(posText, widthBarCode, heightBarCode, typeBarCode, barCode); }
extern int  libAddPosition(QByteArray goodsName, QByteArray barcode, double quantity, double price, byte taxNumber, int numGoodsPos, byte numDepart, byte coefType, QByteArray coefName, double coefValue)
    { return sConnector->addPosition(goodsName, barcode, quantity, price, taxNumber, numGoodsPos, numDepart, coefType, coefName, coefValue); }
extern int  libSubTotal() { return sConnector->printSubTotal(); }
extern int  libAddDiscount(byte typeDiscount, QByteArray nameDiscount, long sum) { return sConnector->addDiscount(typeDiscount, nameDiscount, sum); }
extern void libPrintServiceData() { sConnector->printServiceData(); }
extern int  libOpenShift(QByteArray nameCashier) { return sConnector->openShift(nameCashier); }
extern int  libAddPaymentD(byte typePayment, double sum, QByteArray infoStr) { return sConnector->addPaymentD(typePayment, sum, infoStr); }
