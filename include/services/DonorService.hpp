#ifndef DONORSERVICE_H
#define DONORSERVICE_H

#include "api/ApiClient.hpp"
#include "dtos/Addresses/CreateAddressDTO.hpp"
#include "dtos/Patientes/CreatePatientesDTO.hpp"
#include "dtos/Patientes/UpdatePatientesDTO.hpp"
#include "models/Patientes.hpp"
#include "models/enums/PatientState.hpp"
#include "services/ApiError.hpp"

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>
#include <functional>
#include <optional>

struct DonantesPage
{
    QList<Patientes> data;
    int page = 1;
    int size = 20;
    int total = 0;
    int totalPages = 0;

    static DonantesPage fromJson(const QJsonObject &json);
};

class DonorService : public QObject
{
    Q_OBJECT

public:
    explicit DonorService(QObject *parent = nullptr);

    void fetchPage(
        int page,
        int size,
        const QString &search = QString(),
        const std::optional<PatientState> &state = std::nullopt,
        const std::optional<bool> &isActive = std::nullopt);
    void create(const CreatePatientesDTO &dto);
    void update(const UpdatePatientesDTO &dto);
    void activate(int id);
    void deactivate(int id);

    void fetchAddresses(int personId, int page = 1, int size = 50);
    void createAddress(const CreateAddressDTO &dto);
    void makeCurrentAddress(int addressId);
    void deleteAddress(int addressId);

    void fetchCycles(int patientId, int page = 1, int size = 50);
    void fetchCycleDetail(int cycleId);

signals:
    void pageFetched(const DonantesPage &page);
    void createSucceeded(const Patientes &donante);
    void updateSucceeded();
    void toggleSucceeded();
    void addressesFetched(const QJsonObject &page);
    void addressCreated();
    void addressCurrentSet();
    void addressDeleted();
    void cyclesFetched(const QJsonObject &page);
    void cycleDetailFetched(const QJsonObject &cycle);
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
