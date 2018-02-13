#include "fiscal.h"
#include "registry.h"
#include "vikiprint.h"
#include "atol.h"
#include "fiscalconfig.h"

Fiscal* FiscalFactory::fiscal = nullptr;

Fiscal* FiscalFactory::createFiscal(bool force)
{
    if (FiscalFactory::fiscal != nullptr && !force)
        return FiscalFactory::fiscal;

    if (FiscalFactory::fiscal != nullptr)
        FiscalFactory::fiscal->Finalize();

    FiscalConfig* fc = Registry::getInstance()->getFiscalConfig();

    switch (fc->getType()) {
    case FISCAL_VIKIPRINT:
        FiscalFactory::fiscal = (Fiscal*) new VikiPrint(fc->getPort(), fc->getSpeed()); break;
    case FISCAL_ATOL:
        FiscalFactory::fiscal = (Fiscal*) new Atol(fc->getPort(), fc->getSpeed()); break;
    default:
        break;
    }

    return FiscalFactory::fiscal;
}

void Fiscal::OpenDocument(byte type)
{
    switch (Registry::getInstance()->getFiscalConfig()->getType()) {
    case FISCAL_VIKIPRINT:
        switch (type) {
        case DOC_PRINT:     _OpenDocument(1); break;
        case DOC_PURCHASE:  _OpenDocument(2); break;
        }
        break;
    case FISCAL_ATOL:
        switch (type) {
        case DOC_PRINT:     _OpenDocument(0); break;
        case DOC_PURCHASE:  _OpenDocument(1); break;
        }
        break;
    }
}
