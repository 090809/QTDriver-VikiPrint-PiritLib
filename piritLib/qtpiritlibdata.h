#ifndef QTPIRITLIBDATA_H
#define QTPIRITLIBDATA_H
#include <QByteArray>
#include <QString>
#include <QDebug>
//sendData
QByteArray  intToByteArray(int value, bool toString = true, int base = 16, bool needJustified = false);
QByteArray  doubleToByteArray(double value, bool toString = true);
QByteArray* intToLinkByteArray(int value, bool toString = true, int base = 16);

#define DATA_START 6
#define DATA_END _r.size() - 3

typedef unsigned char byte;

const QByteArray FS = QByteArray(1, 0x1C);

struct sData{
    virtual QByteArray toByteArray();
    operator QByteArray() { return this->toByteArray(); }
};

//Для запросов БЕЗ отправных данных.
struct s_none : sData
{
    QByteArray toByteArray() { return QByteArray(); }
} typedef s_checkStatus, s_scrollPaper, s_statusFlags, s_printSubTotal, s_printServiceData;

struct s_getAdvErr : sData{

    virtual QByteArray toByteArray() { return QByteArray(intToByteArray(1)); }
};

struct s_openShift : sData
{
public:
    s_openShift(QByteArray name)
        :name(name) { }
    QByteArray name;
    QByteArray toByteArray() { return name; }
};

struct s_commandStart : sData
{
    s_commandStart(QByteArray date, QByteArray time)
        :date(date), time(time) { }

    QByteArray date;
    QByteArray time;
    
    QByteArray toByteArray() { return date + FS + time; }
};

struct s_openDocument : sData
{
    s_openDocument(byte docType, byte numDepart, QByteArray cashierName, int docNumber)
        :docType(docType), numDepart(numDepart), cashierName(cashierName), docNumber(docNumber) { }

    unsigned char docType;
    unsigned char numDepart;
    QByteArray cashierName;
    int docNumber;
    
    QByteArray toByteArray() { return intToByteArray(docType, true, 10) + FS +
                                      intToByteArray(numDepart, true, 10) + FS +
                                      cashierName + FS +
                                      intToByteArray(docNumber, true, 10); }
};

struct s_printString : sData
{
    s_printString(QByteArray text, byte attr)
        :text(text), attr(attr) { }

    QByteArray text;
    byte attr;

    QByteArray toByteArray() { return text + FS + intToByteArray(attr, true, 10); }
};

struct s_printBarCode : sData
{
    s_printBarCode(byte posText, byte width, byte height, byte type, QByteArray barCode)
        :posText(posText), width(width), height(height), type(type), barCode(barCode) { }

    byte posText;
    byte width;
    byte height;
    byte type;
    QByteArray barCode;

    QByteArray toByteArray() { return intToByteArray(posText, true, 10) + FS +
                                      intToByteArray(width, true, 10) + FS +
                                      intToByteArray(height, true, 10) + FS +
                                      intToByteArray(type, true, 10) + FS +
                                      barCode; }
};

struct s_addPosition : sData
{
    QByteArray goodsName;
    QByteArray barcode;
    double quantity = 1;
    double price = 1;
    byte taxNumber = 0;
    int numGoodsPos = 0;
    byte numDepart = 0;
    byte coefType = 0;
    QByteArray coefName;
    double coefValue = 0;

    QByteArray toByteArray() { return goodsName + FS +
                                      barcode + FS +
                                      doubleToByteArray(quantity) + FS +
                                      doubleToByteArray(price) + FS +
                                      intToByteArray(taxNumber, true, 16) + FS +
                                      intToByteArray(numDepart, true, 16) + FS +
                                      intToByteArray(coefType, true, 16) + FS +
                                      coefName + FS +
                                      doubleToByteArray(coefValue) + FS;}
};

struct s_printZReport : sData
{
    s_printZReport(QByteArray name, int options)
        :name(name), options(options) { }

    QByteArray name;
    int options;

    QByteArray toByteArray() { return name + FS + intToByteArray(options, true, 10); }
};

struct s_closeDoc : sData
{
    s_closeDoc(byte cut)
        :cut(cut) { }

    byte cut;
    QByteArray toByteArray() { return intToByteArray(cut, true, 10); }
};

struct s_addDiscount : sData
{
    s_addDiscount(byte type, QByteArray name, long sum)
        :type(type), name(name), sum(sum) { }

    byte type;
    QByteArray name;
    long sum;

    QByteArray toByteArray() { return intToByteArray(type, true, 10) + FS + name + FS + intToByteArray(sum, true, 10); }
};

struct s_addPaymentD : sData
{
    s_addPaymentD (byte type, double sum, QByteArray infoStr)
        :type(type), sum(sum), infoStr(infoStr) { }

    byte type;
    double sum;
    QByteArray infoStr;

    QByteArray toByteArray() { return intToByteArray(type, true, 10) + FS + doubleToByteArray(sum) + FS + infoStr; }
};


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
////////////////    reciveData     ///////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


//recieveData

struct rData {
    QByteArray _r, data;
    bool simple = false, errFind = false;
    rData(QByteArray r, bool isSimple = false);
    byte customErr = 0;

    bool checkSum();
    int getError() {
        if (errFind)
            return customErr;

        int err = simple ?: (QString(_r.at(4)) + QString(_r.at(5))).toInt(nullptr, 16);

        return err;
    }

    void checkAndParseData(bool isSimple) {
        if (!isSimple) {
            data = _r.mid(6, _r.size() - 3);
                if (checkSum())
                    if (int k = getError())
                        qDebug() << "getError" << k;
                    else
                        parseData();
                else
                {
                    qDebug() << "checkSum err";
                    errFind = true;
                    customErr = -1;
                }
        } else
            parseData();
    }
    virtual void parseData() {}
};


struct r_getAdvError : rData
{
    int error = 0;
    r_getAdvError(QByteArray r, bool simple = false) : rData(r, simple) { checkAndParseData(simple); }

    void parseData() override {
        QList<QByteArray> parsed = data.split(0x1C);
        error = parsed[1].toInt();
    }
};

struct r_statusFlags : rData {
    r_statusFlags(QByteArray r, bool simple = false) : rData(r, simple) { checkAndParseData(simple); }

    int fatalStatus = 0;
    int currentFlagsStatus = 0;
    int documentStatus = 0;

    void parseData() override {
        QList<QByteArray> parsed = data.split(0x1C);
        fatalStatus = parsed[0].toInt();
        currentFlagsStatus = parsed[1].toInt();
        documentStatus = parsed[2].toInt();
    }
};

struct r_status : rData {
    r_status(QByteArray r, bool simple = true) : rData(r, simple) { }
    byte ACK;

    void parseData() override {
        //ACK = _r;
    }
};

struct r_scroll : rData {
    r_scroll(QByteArray r, bool simple = true) : rData(r, simple) { }
};

struct r_printZReport : rData {
    r_printZReport(QByteArray r, bool simple = false) : rData(r, simple) { }
};

struct r_closeDocument : rData
{
    r_closeDocument(QByteArray r, bool simple = false) : rData(r, simple) { }
};

struct r_openDocument : rData
{
    r_openDocument(QByteArray r, bool simple = false) : rData(r, simple) { }
};

struct r_printString : rData
{
    r_printString(QByteArray r, bool simple = false) : rData(r, simple) { }
};

struct r_printBarCode : rData
{
    r_printBarCode(QByteArray r, bool simple = false) : rData(r, simple) { }
};

struct r_addPosition : rData
{
    r_addPosition(QByteArray r, bool simple = false) : rData(r, simple) { }
};

struct r_printSubTotal : rData
{
    r_printSubTotal(QByteArray r, bool simple = false) : rData(r, simple) { }
};

struct r_addDiscount : rData
{
    r_addDiscount(QByteArray r, bool simple = false) : rData(r, simple) { }
};

struct r_printServiceData : rData
{
    r_printServiceData(QByteArray r, bool simple = false) : rData(r, simple) { }
};

struct r_openShift : rData
{
    r_openShift(QByteArray r, bool simple = false) : rData(r, simple) { }
};

struct r_addPaymentD : rData
{
    r_addPaymentD(QByteArray r, bool simple = false) : rData(r, simple) { }
};

#endif // QTPIRITLIBDATA_H
