#ifndef USERSSERVICE_H
#define USERSSERVICE_H

#include "api/ApiClient.hpp"
#include "services/ApiError.hpp"

#include <QList>
#include <QObject>
#include <QString>

struct UserItem
{
    int id = 0;
    QString username;
    QString name;
    QString lastname;
    QString mail;
    QString phone;
    QString documentType;
    QString documentNumber;
    QString birthDate;
    QString birthPlace;
    bool isActive = true;
    QString role;
};

struct UsersPage
{
    QList<UserItem> data;
    int page = 1;
    int size = 20;
    int total = 0;
    int totalPages = 0;

    static UsersPage fromJson(const QJsonObject &json);
};

class UsersService : public QObject
{
    Q_OBJECT

public:
    explicit UsersService(QObject *parent = nullptr);

    void fetchPage(int page, int size);
    void create(const UserItem &item, const QString &password);
    void update(int id, const UserItem &item, const QString &newPassword);
    void setActive(int id, bool active);

signals:
    void pageFetched(const UsersPage &page);
    void createSucceeded();
    void updateSucceeded();
    void activeChanged();
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
