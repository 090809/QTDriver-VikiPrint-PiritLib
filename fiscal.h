#ifndef FISCAL_H
#define FISCAL_H

#include <QString>
typedef unsigned char byte;

enum fiscal_types{
    FISCAL_VIKIPRINT = 1,
    FISCAL_ATOL,
    FISCAL_NONE = 0
};

enum document_types{
    DOC_PRINT = 0,
    DOC_PURCHASE,
};

class Fiscal
{
public:
    Fiscal() {}

    void OpenDocument(byte type);

    virtual void _OpenDocument(byte type);
    virtual void CloseDocument();

    virtual void SetTaxNumber(byte number);

    virtual void SetCashierName(QString name);

    virtual void SetNumDepart(byte number);

    virtual void SetDocNumber(int number);

    virtual void ScrollPaper();
    virtual void ScrollPaper(int count);

    virtual void PrintString(QString text);

    virtual void RegisterProduct(QString name, QString barcode, double quantity, double price, int numPos = 1);
    virtual void RegisterPayment(double sum, byte type = 0);

    virtual void RegisterSummDiscount(QString nameDiscount, double sum);
    virtual void RegisterPercentDiscount(QString nameDiscount, double value);

    virtual void PrintTotal();
    virtual void PrintServiceData();

    virtual bool Ready();
    virtual bool IsSessionOpen();

    virtual void OpenSession();
    virtual void CloseSession();

    virtual void AnnulateProduct(QString name, double quantity, double price);

    virtual QVector<int> GetStatuses();

    virtual void Finalize();

    virtual void PrintBarcode(QString barcode);
    virtual void PrintQRCode(QString text);
};

class FiscalFactory
{
private:
    FiscalFactory();
    static Fiscal* fiscal;
public:
    static Fiscal* createFiscal(bool force = false);
    static Fiscal* getFiscal() {
        return fiscal;
    }
};
#endif // FISCAL_H
