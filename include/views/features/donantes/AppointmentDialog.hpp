#ifndef APPOINTMENTDIALOG_H
#define APPOINTMENTDIALOG_H

#include "models/Patientes.hpp"
#include "services/ApiError.hpp"
#include "services/TypeDonationService.hpp"

#include <QDialog>
#include <QDateTime>
#include <QJsonObject>
#include <QList>

class AppointmentService;
class DonorService;
class QComboBox;
class QDateEdit;
class QFormLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;

class AppointmentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AppointmentDialog(const Patientes &donante, QWidget *parent = nullptr);

signals:
    void created();

private:
    void buildUi();
    void onDonationTypes(const QList<TypeDonationItem> &items);
    void onTypeChanged(int index);
    void onCycles(const QJsonObject &page);
    void submit();
    void onFailed(const ApiError &error);

    AppointmentService *m_service = nullptr;
    TypeDonationService *m_typeService = nullptr;
    DonorService *m_donorService = nullptr;
    Patientes m_donante;
    QDateTime m_pendingDatetime;

    bool m_hasOpenCycle = false;

    QFormLayout *m_form = nullptr;
    QDateEdit *m_date = nullptr;
    QComboBox *m_time = nullptr;
    QComboBox *m_type = nullptr;
    QComboBox *m_donationType = nullptr;
    QLineEdit *m_observation = nullptr;
    QWidget *m_seriesRow = nullptr;
    QRadioButton *m_seriesContinue = nullptr;
    QRadioButton *m_seriesNew = nullptr;
    QLabel *m_formError = nullptr;
    QPushButton *m_acceptBtn = nullptr;
};

#endif
