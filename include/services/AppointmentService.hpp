#ifndef APPOINTMENTSERVICE_H
#define APPOINTMENTSERVICE_H

#include "api/ApiClient.hpp"
#include "dtos/Appointments/CreateAppointmentDTO.hpp"
#include "dtos/Appointments/UpdateAppointmentDTO.hpp"
#include "models/enums/AppointmentState.hpp"
#include "models/enums/AppointmentType.hpp"
#include "services/ApiError.hpp"

#include <QDateTime>
#include <QList>
#include <QObject>
#include <QString>
#include <optional>

struct TurnoItem
{
    int id = 0;
    QDateTime datetime;
    AppointmentState state = AppointmentState::PENDING;
    AppointmentType type = AppointmentType::DONATION;
    int patientId = 0;
    QString patientName;
    QString patientLastname;
    QString assignedUsers;
    QString observation;
    std::optional<int> typeDonationId;
};

struct TurnosPage
{
    QList<TurnoItem> data;
    int page = 1;
    int size = 20;
    int total = 0;
    int totalPages = 0;

    static TurnosPage fromJson(const QJsonObject &json);
};

class AppointmentService : public QObject
{
    Q_OBJECT

public:
    explicit AppointmentService(QObject *parent = nullptr);

    void fetchPage(
        int page,
        int size,
        const std::optional<AppointmentState> &state = std::nullopt);
    void create(const CreateAppointmentDTO &dto);
    void update(const UpdateAppointmentDTO &dto);
    void changeState(int id, AppointmentState state);
    void autoNoShowExpired(const QList<int> &ids);
    void remove(int id);

signals:
    void pageFetched(const TurnosPage &page);
    void createSucceeded();
    void updateSucceeded();
    void changeStateSucceeded();
    void expiredNoShowDone();
    void removeSucceeded();
    void failed(const ApiError &error);

private:
    void prepare();
    void handle(
        QNetworkReply *reply,
        quint64 requestSeq,
        const std::function<void(int, const QByteArray &)> &onSuccess);

    ApiClient m_api;
    quint64 m_pageSeq = 0;
};

#endif
