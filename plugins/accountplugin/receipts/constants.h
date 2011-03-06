#ifndef ACCOUNTRECEIPTS_CONTANTS_H
#define ACCOUNTRECEIPTS_CONTANTS_H

namespace ReceiptsConstants{
    enum TypeOfPayment{
        Cash = 0,
        Check,
        Visa,
        Banking,
        Other,
        Due,
        MaxOfTypeOfPayments        
    };

}

#endif
