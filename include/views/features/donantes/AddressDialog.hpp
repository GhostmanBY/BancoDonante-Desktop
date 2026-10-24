#ifndef AddressDIALOG_H
#define AddressDIALOG_H

#include "services/ApiError.hpp"

#include <QDialog>

class AddressFields;
class DonorService;

class AddressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddressDialog(int personId, QWidget *parent = nullptr);

signals:
    void saved();

private:
    void submit();
    void onFailed(const ApiError &error);

    DonorService *m_service = nullptr;
    int m_personId = 0;
    AddressFields *m_fields = nullptr;
    class QPushButton *m_acceptBtn = nullptr;
};

#endif
