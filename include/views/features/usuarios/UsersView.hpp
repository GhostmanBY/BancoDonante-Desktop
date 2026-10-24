#ifndef USERSVIEW_H
#define USERSVIEW_H

#include "services/ApiError.hpp"
#include "services/UsersService.hpp"

#include <QWidget>
#include <QString>

class EmptyState;
class PaginationBar;
class QPushButton;
class QStackedWidget;
class QTableView;
class UsersModel;


class UsersView : public QWidget
{
    Q_OBJECT

public:
    explicit UsersView(QWidget *parent = nullptr);

    void refresh();

private:
    void buildUi();
    void loadPage(int page);
    void onPageChanged(int page);
    void onSizeChanged(int size);
    void updateEditAction();
    const UserItem *selectedUser() const;
    bool canEdit(const UserItem &item) const;
    bool canToggle(const UserItem &item) const;
    void openEditDialog();
    void openCreateDialog();
    void openContextMenu(const QPoint &pos);
    void setActiveForSelected(bool active);

    UsersService *m_service = nullptr;
    UsersModel *m_model = nullptr;

    QPushButton *m_newBtn = nullptr;
    QPushButton *m_editBtn = nullptr;
    QPushButton *m_refreshBtn = nullptr;

    QStackedWidget *m_bodyStack = nullptr;
    QTableView *m_table = nullptr;
    PaginationBar *m_pagination = nullptr;
    EmptyState *m_empty = nullptr;
    QWidget *m_loading = nullptr;

    int m_page = 1;
    int m_size = 20;
    bool m_hasLoadedOnce = false;
};

#endif
