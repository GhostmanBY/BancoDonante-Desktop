#ifndef WHATSAPPSERVICE_H
#define WHATSAPPSERVICE_H

#include "api/ApiClient.hpp"
#include "services/ApiError.hpp"

#include <QList>
#include <QMetaType>
#include <QObject>
#include <functional>

struct WSPTemplateOption
{
    QString motive;
    QString name;
};

struct WSPTemplateItem
{
    int id = 0;
    QString motive;
    QString name;
    QString text;
    bool isActive = false;
};

struct WSPInboundReview
{
    int id = 0;
    int patientId = 0;
    int appointmentId = 0;
    QString patientName;
    QString senderPhone;
    QString body;
    QString receivedAt;
};
Q_DECLARE_METATYPE(WSPInboundReview)

class WhatsAppService : public QObject
{
    Q_OBJECT

public:
    explicit WhatsAppService(QObject *parent = nullptr);

    void fetchConnection();
    void fetchActiveTemplates();
    void fetchTemplates();
    void updateTemplate(int id, const QString &motive, const QString &name, const QString &text);
    void setTemplateActive(int id, bool active);
    void enqueue(int patientId, const QString &motive);
    void fetchPendingInboundReviews();
    void markInboundReviewReviewed(int id);
    void resolveInboundReview(int id, const QString &decision);

signals:
    void connectionFetched(const QString &state, const QString &qr);
    void templatesFetched(const QList<WSPTemplateOption> &templates);
    void managedTemplatesFetched(const QList<WSPTemplateItem> &templates);
    void templateSaved(int id);
    void templateActivationChanged(int id, bool active);
    void enqueueSucceeded();
    void inboundReviewsFetched(const QList<WSPInboundReview> &reviews, int total);
    void inboundReviewMarked(int id);
    void inboundReviewResolved(int id);
    void failed(const ApiError &error);

private:
    void prepare();
    void handle(QNetworkReply *reply, const std::function<void(const QByteArray &)> &onSuccess);

    ApiClient m_api;
};

#endif
