#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QHash>
#include <QMainWindow>
#include <QString>

class Sidebar;
class TopBar;
class DashboardView;
class DonantesView;
class TurnosView;
class SettingsView;
class UsersView;
class QStackedWidget;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:
    void logoutRequested();

private:
    void buildUi();
    void switchToPage(const QString &id);

    Sidebar *m_sidebar = nullptr;
    TopBar *m_topBar = nullptr;
    QStackedWidget *m_stack = nullptr;
    QHash<QString, int> m_pageIndexes;
    DashboardView *m_dashboard = nullptr;
    DonantesView *m_donantes = nullptr;
    TurnosView *m_turnos = nullptr;
    SettingsView *m_settingsPage = nullptr;
    UsersView *m_usersPage = nullptr;
};

#endif
