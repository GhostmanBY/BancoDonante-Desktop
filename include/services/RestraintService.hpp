#ifndef RESTRAINTSERVICE_H
#define RESTRAINTSERVICE_H

#include "api/ApiClient.hpp"
#include "dtos/Restraints/CreateRestraintDTO.hpp"
#include "dtos/Restraints/CreateRestraintTemplateDTO.hpp"
#include "dtos/Restraints/UpdateRestraintTemplateDTO.hpp"
#include "services/ApiError.hpp"

#include <QDate>
#include <QJsonArray>
#include <QList>
#include <QObject>
#include <QString>

struct PatientObservationItem
{
    int id = 0;
    int patientId = 0;
    QString texto;
    QString createdAt;

    static PatientObservationItem fromJson(const QJsonObject &json);
};

struct RestraintTemplateItem
{
    int id = 0;
    QString motivo;
    int defaultMonths = 1;
    bool isActive = true;

    static RestraintTemplateItem fromJson(const QJsonObject &json);
};

class RestraintService : public QObject
{
    Q_OBJECT

public:
    explicit RestraintService(QObject *parent = nullptr);

    // POST /patientes/{id}/restraints — operación atómica en backend:
    // crea la restricción, cancela turnos incompatibles, documenta el
    // motivo y pone el estado del donante en 'no-apto'. templateId > 0
    // deja constancia de la plantilla de catálogo copiada.
    void assignRestraint(int patientId, const CreateRestraintDTO &dto);

    // GET /patientes/{id}/observations — historial documental.
    void fetchObservations(int patientId);

    // GET /restraint-templates — catálogo global completo
    // (activo e inactivo).
    void fetchTemplates();

    // POST /restraint-templates — agrega al catálogo global.
    void createTemplate(const CreateRestraintTemplateDTO &dto);

    // PUT /restraint-templates/{id} — edita los defaults de la
    // plantilla; no toca las asignaciones existentes.
    void updateTemplate(int id, const UpdateRestraintTemplateDTO &dto);

signals:
    void restraintAssigned(int cancelledAppointments);
    void observationsFetched(const QJsonArray &items);
    void templatesFetched(const QList<RestraintTemplateItem> &templates);
    void templateSaved(int id);
    void failed(const ApiError &error);

private:
    void prepare();
    void handle(QNetworkReply *reply, const std::function<void(int, const QByteArray &)> &onSuccess);

    ApiClient m_api;
};

#endif
