#ifndef RESCHEDULETURNODIALOG_H
#define RESCHEDULETURNODIALOG_H

#include "services/AppointmentService.hpp"

#include <QDateTime>
#include <QDialog>

class QComboBox;
class QDateEdit;
class QLabel;
class QPushButton;

class RescheduleTurnoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RescheduleTurnoDialog(const TurnoItem &item, QWidget *parent = nullptr);

signals:
    void rescheduled();

private:
    void buildUi();
    void submit();
    void onFailed(const ApiError &error);

    TurnoItem m_item;
    AppointmentService *m_service = nullptr;
    QDateTime m_pendingDatetime;

    QDateEdit *m_date = nullptr;
    QComboBox *m_time = nullptr;
    QLabel *m_formError = nullptr;
    QPushButton *m_acceptBtn = nullptr;
};

#endif
