#ifndef WHATSAPPSERVICE_H
#define WHATSAPPSERVICE_H

#include "api/ApiClient.hpp"
#include "services/ApiError.hpp"

#include <QList>
#include <QObject>
#include <functional>

struct WSPTemplateOption
{
    QString motive;
    QString name;
};

class WhatsAppService : public QObject
{
    Q_OBJECT

public:
    explicit WhatsAppService(QObject *parent = nullptr);

    void fetchConnection();
    void fetchActiveTemplates();
    void enqueue(int patientId, const QString &motive);

signals:
    void connectionFetched(const QString &state, const QString &qr);
    void templatesFetched(const QList<WSPTemplateOption> &templates);
    void enqueueSucceeded();
    void failed(const ApiError &error);

private:
    void prepare();
    void handle(QNetworkReply *reply, const std::function<void(const QByteArray &)> &onSuccess);

    ApiClient m_api;
};

#endif
