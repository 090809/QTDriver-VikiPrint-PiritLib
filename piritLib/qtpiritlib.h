#ifndef QTPIRITLIB_H
#define QTPIRITLIB_H
#include <QObject>
#include <QByteArray>
#include <QSerialPort>
#include <QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QSerialPortInfo>
#include <QMap>
#include <qtpiritlibdata.h>
#include "fiscal.h"

enum CoefficientTypes
{
    NONE = 0, DISCOUNT_PERCENT, DISCOUNT_SUM, MARGIN_PERCENT, MARGIN_SUM
};

class qConnector : public QObject
{
    Q_OBJECT
private:
    static qConnector* sefl;
    qConnector(QObject* p = nullptr) : QObject(p) {
        sp = new QSerialPort();
        qDebug() << "Serial port created";
        packetId = 0x20;
        connect(sp, &QSerialPort::readyRead, this, &qConnector::readMessage);
    }

    byte packetId;
    QByteArray readedData;
    //QMap<int, Packet> packetsMap;
    QSerialPort* sp;
public slots:
    void readMessage() {
        readedData = 0;
        while (sp->bytesAvailable() != 0)
        {
            readedData.append(sp->readAll());
        }
        emit readDone();
    }

signals:
    void readDone();// {}

public:
    static qConnector* getInstance()
    {
        if (!sefl)
            sefl = new qConnector();
        return sefl;
    }
//########################  Port     #######################//

    int openPort(QString port, long baudrate);
    void closePort();

//########################  Easy-Commands  #################//

    void commandStart();
    void scrollPaper();
    void closeDocument(byte cutPapare);
    void printServiceData();

//########################  Commands  ######################//

    int getStatusFlags(int *fatalStatus, int *currentFlagsStatus, int *documentStatus);
    int printZReport(QByteArray nameCashier, int options);
    int openDocument(byte type, byte numDepart, QByteArray nameCashier, long docNumber);
    int addPaymentD(byte typePayment, double sum, QByteArray infoStr);
    int openShift(QByteArray nameCashier);
    int addDiscount(byte typeDiscount, QByteArray nameDiscount, long sum);
    int printSubTotal();
    int addPosition(QByteArray goodsName, QByteArray barcode, double quantity, double price, byte taxNumber, int numGoodsPos, byte numDepart, byte coefType, QByteArray coefName, double coefValue);
    int printBarCode(byte posText, byte widthBarCode, byte heightBarCode, byte typeBarCode, QByteArray barCode);
    int printString(QByteArray textStr, byte attribute);

//########################  Packets  #######################//
    template <typename T>
    T sendRequest(byte commandId, sData* data, bool isSimple = false);
    rData getAnswer(byte requestId);
};

struct sPacket {

    sPacket(byte commandId, sData* d, byte pid) {
        command = intToByteArray(commandId, true, 16, true);
        data = d;
        packetId = pid;
    }

    byte startByte = 0x02;
    QByteArray pass = QByteArray(QString("PIRI").toStdString().c_str());
    byte packetId;
    QByteArray command;
    sData* data;
    byte endByte = 0x03;
    QByteArray chkSum;

    QByteArray calculateChkSum();
    void setCommand(int commandId);
    QByteArray* toByteArray();
    static byte getPacketId();
};

enum commands {
    GET_STATUS_FLAG = 0x00,
};

extern int  openPort(QString fileName, long speed);
extern int  commandStart();
extern int  scrollPaper();
extern int  getStatusFlags(int *fatalStatus, int *currentFlagsStatus, int *documentStatus);
extern int  libPrintZReport(QByteArray nameCashier, int options);
extern int  libOpenDocument(byte type, byte numDepart, QByteArray nameCashier, long docNumber);
extern void libCloseDocument(byte cutPaper); // ????
extern int  libPrintString(QByteArray textStr, byte attribute);
extern int  libPrintBarCode(byte posText, byte widthBarCode, byte heightBarCode, byte typeBarCode, QByteArray barCode);
extern int  libAddPosition(QByteArray goodsName, QByteArray barcode, double quantity, double price, byte taxNumber, int numGoodsPos, byte numDepart, byte coefType, QByteArray coefName, double coefValue);
extern int  libSubTotal();
extern int  libAddDiscount(byte typeDiscount, QByteArray nameDiscount, long sum);
extern void libPrintServiceData();
extern int  libOpenShift(QByteArray nameCashier);
extern int  libAddPaymentD(byte typePayment, double sum, QByteArray infoStr);

//IMPORTDLL MData PIRITLIB_CALL libGetCountersAndRegisters(unsigned char numRequest);
//IMPORTDLL MData PIRITLIB_CALL libGetKKTInfo(unsigned char numRequest);
//IMPORTDLL int PIRITLIB_CALL getKKTInfo(unsigned char numRequest, char *data);
//IMPORTDLL MData PIRITLIB_CALL libGetReceiptData(unsigned char numRequest);
//IMPORTDLL MData PIRITLIB_CALL libGetPrinterStatus();
//IMPORTDLL MData PIRITLIB_CALL libGetServiceInfo(unsigned char numRequest);
//IMPORTDLL MData PIRITLIB_CALL libGetExErrorInfo(unsigned char numRequest);
//IMPORTDLL int PIRITLIB_CALL scrollPaper();
//IMPORTDLL int PIRITLIB_CALL libCommandStart(MPiritDate mpDate, MPiritTime mpTime);
//IMPORTDLL MData PIRITLIB_CALL libReadSettingsTable(unsigned char number, int index);
//IMPORTDLL int PIRITLIB_CALL libWriteSettingsTable(unsigned char number, int index, const char* data);
//IMPORTDLL MData PIRITLIB_CALL libGetPiritDateTime();
//IMPORTDLL int PIRITLIB_CALL libSetPiritDateTime(MPiritDate mpDate, MPiritTime mpTime);
//IMPORTDLL int PIRITLIB_CALL libLoadLogo(int size, unsigned char* data);
//IMPORTDLL int PIRITLIB_CALL libDeleteLogo();
//IMPORTDLL int PIRITLIB_CALL libLoadReceiptDesign(int size, unsigned char* data);
//IMPORTDLL int PIRITLIB_CALL libLoadPicture(int width, int height, int sm, const char* name, int number, unsigned char* data);
//IMPORTDLL int PIRITLIB_CALL libPrintXReport(const char* nameCashier);
//IMPORTDLL unsigned long PIRITLIB_CALL libSetBuyerAddress(const char *buyerAddress);
//IMPORTDLL unsigned long PIRITLIB_CALL libGetBuyerAddress(char *buyerAddress, unsigned long baLength);
//IMPORTDLL void PIRITLIB_CALL libCleanBuyerAddress();
//IMPORTDLL int PIRITLIB_CALL libCancelDocument();
//IMPORTDLL int PIRITLIB_CALL libPostponeDocument(const char* info);
//IMPORTDLL int PIRITLIB_CALL libCutDocument();
//IMPORTDLL int PIRITLIB_CALL libDelPosition(const char* goodsName, const char* barcode, double quantity, double price, unsigned char taxNumber, int numGoodsPos, unsigned char numDepart);
//IMPORTDLL int PIRITLIB_CALL libAddMargin(unsigned char typeMargin, const char* nameMargin, long sum);
//IMPORTDLL int PIRITLIB_CALL libAddPayment(unsigned char typePayment, long long sum, const char* infoStr);
//IMPORTDLL int PIRITLIB_CALL libCashInOut(const char* infoStr, long long sum);
//IMPORTDLL int PIRITLIB_CALL libPrintRequsit(unsigned char codeReq, unsigned char attributeText, const char* str1, const char* str2, const char* str3, const char* str4);
//IMPORTDLL int PIRITLIB_CALL libRegisterSumToDepart(unsigned char typeOperation, unsigned char numberDepart, long sum);
//IMPORTDLL int PIRITLIB_CALL libRegisterTaxSum(unsigned char numberTax, long sum);
//IMPORTDLL int PIRITLIB_CALL libCompareSum(long sum);
//IMPORTDLL int PIRITLIB_CALL libOpenCopyReceipt(unsigned char type, unsigned char numDepart, const char* nameCashier, int numCheck, int numCash, MPiritDate mpDate, MPiritTime mpTime);
//IMPORTDLL int PIRITLIB_CALL libSetToZeroCashInCashDrawer();
//IMPORTDLL int PIRITLIB_CALL libPrintPictureInDocument(int width, int height, int sm, unsigned char* data);
//IMPORTDLL int PIRITLIB_CALL libPrintPreloadedPicture(int sm, int number);
//IMPORTDLL int PIRITLIB_CALL libTechnologicalReset(const DateTime *dateTime);
//IMPORTDLL int PIRITLIB_CALL libFiscalization(const char *oldPassword, const char *regNumber, const char *INN, const char *newPassword);
//IMPORTDLL int PIRITLIB_CALL libPrintFiscalReportByShifts(unsigned char typeReport, int startShiftNumber, int endShiftNumber, const char *password);
//IMPORTDLL int PIRITLIB_CALL libPrintFiscalReportByDate(unsigned char typeReport, MPiritDate startDate, MPiritDate endDate, const char *password);
//IMPORTDLL int PIRITLIB_CALL libActivizationECT();
//IMPORTDLL int PIRITLIB_CALL libCloseArchiveECT();
//IMPORTDLL int PIRITLIB_CALL libCloseFN(const char *cashierName);
//IMPORTDLL int PIRITLIB_CALL libPrintControlTapeFromECT(int shiftNumber);
//IMPORTDLL int PIRITLIB_CALL libPrintDocumentFromECT(int KPKNumber);
//IMPORTDLL int PIRITLIB_CALL libPrintReportFromECTByShifts(unsigned char typeReport, int startShiftNumber, int endShiftNumber);
//IMPORTDLL int PIRITLIB_CALL libPrintReportFromECTByDate(unsigned char typeReport, MPiritDate startDate, MPiritDate endDate);
//IMPORTDLL int PIRITLIB_CALL libPrintReportActivizationECT();
//IMPORTDLL int PIRITLIB_CALL libPrintReportFromECTByShift(int shiftNumber);
//IMPORTDLL MData PIRITLIB_CALL libGetInfoFromECT(unsigned char number, long dataL1, long dataL2);
//IMPORTDLL int PIRITLIB_CALL libOpenCashDrawer(int pulseDuration);
//IMPORTDLL MData PIRITLIB_CALL libGetCashDrawerStatus();
//IMPORTDLL int PIRITLIB_CALL getCashDrawerStatus(int *drawerStatus);
//IMPORTDLL int PIRITLIB_CALL libBeep(int duration);
//IMPORTDLL int PIRITLIB_CALL libAuthorization(MPiritDate mpDate, MPiritTime mpTime, const char *numKKT);
//IMPORTDLL MData PIRITLIB_CALL libReadMemoryBlock(unsigned char type, long startAdress, long numBytes);
//IMPORTDLL int PIRITLIB_CALL libSetSpeed(unsigned char numSpeed);
//IMPORTDLL int PIRITLIB_CALL command0x9A();
//IMPORTDLL int PIRITLIB_CALL command0x9B();
//IMPORTDLL int PIRITLIB_CALL libEmergencyCloseShift();
//IMPORTDLL int PIRITLIB_CALL libPrintCopyLastZReport();
//IMPORTDLL int PIRITLIB_CALL libEnableServiceChannelToECT();
//IMPORTDLL int PIRITLIB_CALL libPrintCopyReportFiscalization();
//IMPORTDLL int PIRITLIB_CALL getCurMPTime(MPiritDate *mpDate, MPiritTime *mpTime);
//IMPORTDLL int PIRITLIB_CALL saveLogoToFile(wchar_t *fileName);
//IMPORTDLL void PIRITLIB_CALL setDebugLevel(int level);
//IMPORTDLL long long PIRITLIB_CALL getDriverVersion();
//IMPORTDLL int PIRITLIB_CALL libPrintDocsFromECTSDByNumberDoc(int startNumber, int endNumber);
//IMPORTDLL int PIRITLIB_CALL libPrintDocsFromECTSDByNumberShift(int startNumber, int endNumber);
//IMPORTDLL int PIRITLIB_CALL libPrintDocsFromECTSDByDate(MPiritDate mpDateStart, MPiritDate mpDateEnd);
//IMPORTDLL int PIRITLIB_CALL libGetInfoFromECT_NumberDoc(int *numDoc);
//IMPORTDLL int PIRITLIB_CALL libBLRPrintControlTapeFromECT();
//IMPORTDLL int PIRITLIB_CALL getCountersAndRegisters(int requestNumber, int *data);
//IMPORTDLL void PIRITLIB_CALL setAmountDecimalPlaces(int decimalPlaces);
//IMPORTDLL void PIRITLIB_CALL setQuantityDecimalPlaces(int decimalPlaces);
//IMPORTDLL void PIRITLIB_CALL setPercentageDecimalPlaces(int decimalPlaces);
//IMPORTDLL int PIRITLIB_CALL getAmountDecimalPlaces();
//IMPORTDLL int PIRITLIB_CALL getQuantityDecimalPlaces();
//IMPORTDLL int PIRITLIB_CALL getPercentageDecimalPlaces();
//IMPORTDLL int PIRITLIB_CALL libGetCountersAndRegisters_12(int data[], int maxElement, char *str);
//IMPORTDLL int PIRITLIB_CALL getPrinterStatus(int* result);
//IMPORTDLL int PIRITLIB_CALL getPiritDateTime(int* cDate, int* cTime);
//IMPORTDLL int PIRITLIB_CALL libDoCheckCorrection(char* nameCashier, double cash, double cashless, unsigned char correctionFlags);
//IMPORTDLL int PIRITLIB_CALL libDoCheckCorrectionEx(char* nameCashier, double cash, double cashless, double sum1, double sum2, double sum3, unsigned char correctionFlags, MPiritDate docDate, char *docNumber, char *correctionName, double sum_18, double sum_10, double sum_0, double sum_WT, double sum_18_118, double sum_10_110);
//IMPORTDLL int PIRITLIB_CALL libCurrentStatusReport(char* nameCashier);
//IMPORTDLL int PIRITLIB_CALL libRegistration(unsigned char type, const char *regNumber, const char *INN, int systemTax, int rej, const char *cashierName);
//IMPORTDLL int PIRITLIB_CALL libGetInfoFromECT_NumberFP(char *data);
//IMPORTDLL int PIRITLIB_CALL libGetInfoFromECT_FP(int numDoc, char *data);
//IMPORTDLL unsigned long PIRITLIB_CALL libFormatMessage(int errorCode, char *msgBuffer, unsigned long cbBuffer);

#define sConnector qConnector::getInstance()
#endif // QTPIRITLIB_H
