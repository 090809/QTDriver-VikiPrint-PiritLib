#include "vikiprint.h"
#include <QTextCodec>
#include <QDebug>

VikiPrint::VikiPrint(QString setPortName, int setPortSpeed)
    :Fiscal()
{
    if (setPortName.startsWith("COM"))
        portName = setPortName;
    else
        portName = "COM" + setPortName;

    portSpeed = setPortSpeed;
    taxNumber = static_cast<unsigned char>(1);
    qDebug() << "pName -" << portName << ", pSpeed -" << portSpeed;
    int result = openPort(portName, portSpeed);
        
    qDebug() << "Result: " << result;
    if (!result)
        qWarning() << "Порт не открыт!";

    commandStart();
    statuses = QVector<int>();
}

bool VikiPrint::Ready()
{

    int fatalStatus = 0, currentFlagsStatus = 0, documentStatus = 0;
    int flagsStatus = getStatusFlags(&fatalStatus, &currentFlagsStatus, &documentStatus);

    return flagsStatus == 0;
}

void VikiPrint::_OpenDocument(byte type)
{
    libOpenDocument(type, numDepart, constConvertTo866(cashierName), docNumber);
}

void VikiPrint::CloseDocument()
{
    //MData test = MData();
    libCloseDocument(1);
    //libCloseDocument(test, 1);
    ScrollPaper(4);
}

QVector<int> VikiPrint::GetStatuses()
{
    return statuses;
}

void VikiPrint::SetTaxNumber(byte number)
{
    taxNumber = number;
}

void VikiPrint::SetCashierName(QString name)
{
    cashierName = name;
}

void VikiPrint::SetNumDepart(byte number)
{
    numDepart = number;
}

void VikiPrint::SetDocNumber(int number)
{
    docNumber = number;
}

void VikiPrint::ScrollPaper()
{
    scrollPaper();
}

void VikiPrint::ScrollPaper(int count)
{
    for (int i = 0; i < count; i++)
        scrollPaper();
}

void VikiPrint::PrintString(QString text)
{
    libPrintString(constConvertTo866(text), 0);
    libCloseDocument(1);

    //MData test = MData();
    libCloseDocument(1);
}

void VikiPrint::RegisterProduct(QString name, QString barcode, double quantity, double price, int numPos)
{
    libAddPosition(constConvertTo866(name), constConvertTo866(barcode), quantity, price, taxNumber, numPos, numDepart, 0, "", 0);
}

void VikiPrint::AnnulateProduct(QString name, double quantity, double price)
{
    int numPos = 1;
    libAddPosition(constConvertTo866(name), constConvertTo866(name), quantity, price, taxNumber, numPos, numDepart, 0, "", 0);
}

void VikiPrint::RegisterPayment(double sum, byte type)
{
    int result = libAddPaymentD(type, sum, "");
}

void VikiPrint::PrintTotal()
{
    libSubTotal();
}

void VikiPrint::RegisterSummDiscount(QString nameDiscount, double sum)
{
    libAddDiscount(DISCOUNT_PERCENT, constConvertTo866(nameDiscount), (int)(sum*1000));
}

void VikiPrint::RegisterPercentDiscount(QString nameDiscount, double sum)
{
    libAddDiscount(DISCOUNT_SUM, constConvertTo866(nameDiscount), (int)(sum*100));
}

void VikiPrint::PrintServiceData()
{
    libPrintServiceData();
}

void VikiPrint::OpenSession()
{
    libOpenShift(constConvertTo866(cashierName));
}

void VikiPrint::CloseSession()
{
    libPrintZReport(constConvertTo866(cashierName), 0);
}

bool VikiPrint::IsSessionOpen()
{
    int fatalStatus = 0, currentFlagsStatus = 0, documentStatus = 0;
    int flagsStatus = getStatusFlags(&fatalStatus, &currentFlagsStatus, &documentStatus);

    if (flagsStatus == 0)
    {
        if (currentFlagsStatus & 8)
        {
            qDebug() << "Смена длится более 24 часов. Смена перезапустится самостоятельно.";
            CloseSession();
            OpenSession();
            return true;
        }

        if (currentFlagsStatus & 4)
        {
            return true;
        }
    }

    return false;
}

QByteArray VikiPrint::constConvertTo866(QString str)
{
    return QTextCodec::codecForName("CP866")->fromUnicode(str);
}

char* VikiPrint::ConvertTo866(QString str)
{

    QTextEncoder* codec866 = QTextCodec::codecForName("CP866")->makeEncoder();

    return const_cast<char*>(codec866->fromUnicode(str).operator const char *());
}

void VikiPrint::PrintBarcode(QString barcode, int type)
{
    libPrintBarCode(0, 8, 50, type, constConvertTo866(barcode));
}
