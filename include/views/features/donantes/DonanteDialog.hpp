#ifndef DONANTEDIALOG_H
#define DONANTEDIALOG_H

#include "models/Patientes.hpp"
#include "services/ApiError.hpp"

#include <QDialog>
#include <optional>

class AddressFields;
class DonorService;
class QComboBox;
class QDateEdit;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;

class DonanteDialog : public QDialog
{
    Q_OBJECT

public:
    DonanteDialog(
        const std::optional<Patientes> &donante,
        QWidget *parent = nullptr);

signals:
    void saved();

private:
    void buildUi();
    void prefill();
    bool validate();
    void submit();
    void onFailed(const ApiError &error);
    QString birthDateValue() const;
    bool wantsAddress() const;
    void finishSave(const QString &message);

    DonorService *m_service = nullptr;
    std::optional<Patientes> m_donante;
    int m_createdId = 0;

    QLineEdit *m_name = nullptr;
    QLineEdit *m_lastname = nullptr;
    QLineEdit *m_mail = nullptr;
    QLineEdit *m_phone = nullptr;
    QComboBox *m_documentType = nullptr;
    QLineEdit *m_documentNumber = nullptr;
    QDateEdit *m_birthDate = nullptr;
    QLineEdit *m_birthPlace = nullptr;
    QComboBox *m_state = nullptr;
    QComboBox *m_sex = nullptr;
    QDoubleSpinBox *m_peso = nullptr;
    QDoubleSpinBox *m_altura = nullptr;
    QComboBox *m_bloodGroup = nullptr;
    AddressFields *m_addressFields = nullptr;
    QLabel *m_formError = nullptr;
    QPushButton *m_acceptBtn = nullptr;
};

#endif
