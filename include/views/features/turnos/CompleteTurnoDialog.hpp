#ifndef COMPLETETURNODIALOG_H
#define COMPLETETURNODIALOG_H

#include "services/AppointmentService.hpp"

#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;

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
    QLabel *m_formError = nullptr;
    QPushButton *m_acceptBtn = nullptr;
};

#endif
