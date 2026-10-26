#ifndef COMPLETETURNODIALOG_H
#define COMPLETETURNODIALOG_H

#include "services/AppointmentService.hpp"

#include <QDialog>

class QLabel;
class QComboBox;
class QDoubleSpinBox;
class QGroupBox;
class QLineEdit;
class QPushButton;
class QSpinBox;

class CompleteTurnoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CompleteTurnoDialog(const TurnoItem &item, QWidget *parent = nullptr);

signals:
    void completed();

private:
    void buildUi();
    void submit();
    void onFailed(const ApiError &error);

    TurnoItem m_item;
    AppointmentService *m_service = nullptr;

    QLineEdit *m_observation = nullptr;
    QGroupBox *m_clinicalDataGroup = nullptr;
    QSpinBox *m_wholeBloodComponent = nullptr;
    QSpinBox *m_plateletsComponent = nullptr;
    QSpinBox *m_plasmaComponent = nullptr;
    QSpinBox *m_plateletsPerUl = nullptr;
    QDoubleSpinBox *m_hemoglobin = nullptr;
    QDoubleSpinBox *m_redBloodCells = nullptr;
    QDoubleSpinBox *m_hematocrit = nullptr;
    QLineEdit *m_bagType = nullptr;
    QComboBox *m_arm = nullptr;
    QLabel *m_formError = nullptr;
    QPushButton *m_acceptBtn = nullptr;
};

#endif
