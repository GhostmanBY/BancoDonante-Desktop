#ifndef TURNODETAILDIALOG_H
#define TURNODETAILDIALOG_H

#include "services/AppointmentService.hpp"

#include <QDialog>
#include <QJsonObject>
#include <QJsonValue>

class QFormLayout;
class QLabel;

class TurnoDetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TurnoDetailDialog(int appointmentId, QWidget *parent = nullptr);

private:
    void buildUi();
    void onAppointmentFetched(const QJsonObject &appointment);
    void onFailed(const ApiError &error);
    void addRow(const QString &label, const QString &value);
    void addClinicalData(const QJsonValue &value);

    AppointmentService *m_service = nullptr;
    QFormLayout *m_form = nullptr;
    QLabel *m_loading = nullptr;
};

#endif
