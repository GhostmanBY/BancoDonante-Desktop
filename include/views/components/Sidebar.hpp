#ifndef SIDEBAR_H
#define SIDEBAR_H

#include "services/SessionManager.hpp"

#include <QFrame>
#include <QList>
#include <QString>

class QPushButton;
class QVBoxLayout;

class Sidebar : public QFrame
{
    Q_OBJECT

public:
    explicit Sidebar(QWidget *parent = nullptr);

    void addItem(
        const QString &id,
        const QString &label,
        const QString &glyph
    );
    void addItem(
        const QString &id,
        const QString &label,
        const QString &glyph,
        UserRole requiredRole
    );
    void addSeparator();

    void rebuildForRole(UserRole role);
    void setActiveItem(const QString &id);

signals:
    void itemClicked(const QString &id);

private:
    struct Item
    {
        QString id;
        QString label;
        QString glyph;
        UserRole requiredRole;
        bool isSeparator = false;
    };

    void buildUi();
    QPushButton *createButton(const Item &item);
    bool isItemAllowed(const Item &item, UserRole role) const;

    QList<Item> m_items;
    QList<QPushButton *> m_buttons;
    QVBoxLayout *m_navLayout = nullptr;
    UserRole m_currentRole = UserRole::NURSE;
};

#endif
