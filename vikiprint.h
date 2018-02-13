#ifndef VIKIPRINT_H
#define VIKIPRINT_H
#include "fiscal.h"
#include "piritLib/qtpiritlib.h"
#include <QVector>

class VikiPrint : public Fiscal
{
private:
    QString cashierName     = "НЕИЗВЕСТНО";
    int docNumber           = 1;
    unsigned char numDepart = 1;
    QVector<int> statuses;
    unsigned char taxNumber = 1;
    QString portName;
    int portSpeed           = 0;

public:
    static constexpr int DOC_TYPE_SERVICE   = 1; //For print texts
    static constexpr int DOC_TYPE_REGISTER  = 2; //For fiscal registration
    static constexpr int DOC_TYPE_RETURN    = 3;
    static constexpr int DOC_TYPE_INCOME    = 4;
    static constexpr int DOC_TYPE_OUTCOME   = 5;
    static constexpr int DOC_TYPE_BUY       = 6;
    static constexpr int DOC_TYPE_ANNULATE  = 7;

    VikiPrint(QString setPortName, int setPortSpeed);

    bool Ready();

    void _OpenDocument(byte type);

    void CloseDocument();

    QVector<int> GetStatuses();

    void SetTaxNumber(byte number);

    void SetCashierName(QString name);

    void SetNumDepart(byte number);

    void SetDocNumber(int number);

    void ScrollPaper();

    void ScrollPaper(int count);

    void PrintString(QString text);

    void RegisterProduct(QString name, QString barcode, double quantity, double price, int numPos = 1);

    void AnnulateProduct(QString name, double quantity, double price);

    void RegisterPayment(double sum, byte type = 0);

    void PrintTotal();

    void RegisterSummDiscount(QString nameDiscount, double sum);

    void RegisterPercentDiscount(QString nameDiscount, double sum);

    void PrintServiceData();

    void OpenSession();

    void CloseSession();

    bool IsSessionOpen();

    void PrintBarcode(QString barcode) { PrintBarcode(barcode, 3); }
    void PrintBarcode(QString barcode, int type = 0);

    void PrintQRCode(QString text) { PrintBarcode(text, 8); }

private:
    char* ConvertTo866(QString str);
    QByteArray constConvertTo866(QString str);
};

#endif // VIKIPRINT_H
