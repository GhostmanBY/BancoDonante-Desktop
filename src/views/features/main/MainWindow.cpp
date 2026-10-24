#include "views/features/main/MainWindow.hpp"

#include "services/SessionManager.hpp"
#include "views/components/Sidebar.hpp"
#include "views/components/TopBar.hpp"
#include "views/features/dashboard/DashboardView.hpp"
#include "views/features/donantes/DonantesView.hpp"
#include "views/features/settings/SettingsView.hpp"
#include "views/features/turnos/TurnosView.hpp"
#include "views/features/usuarios/UsersView.hpp"

#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Banco de Sangre"));
    resize(1180, 760);
    setMinimumSize(960, 640);

    buildUi();

    SessionUser user = SessionManager::instance().user();
    m_topBar->setUser(user);
    m_sidebar->rebuildForRole(user.role);
    switchToPage(QStringLiteral("dashboard"));
}

void MainWindow::buildUi()
{
    auto *central = new QWidget(this);
    auto *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    m_sidebar = new Sidebar(central);

    auto *rightBox = new QVBoxLayout();
    rightBox->setContentsMargins(0, 0, 0, 0);
    rightBox->setSpacing(0);

    m_topBar = new TopBar(central);

    m_stack = new QStackedWidget(central);

    m_dashboard = new DashboardView(m_stack);
    m_stack->addWidget(m_dashboard);
    m_pageIndexes.insert(QStringLiteral("dashboard"), m_stack->count() - 1);

    m_donantes = new DonantesView(m_stack);
    m_stack->addWidget(m_donantes);
    m_pageIndexes.insert(QStringLiteral("donantes"), m_stack->count() - 1);

    m_turnos = new TurnosView(m_stack);
    m_stack->addWidget(m_turnos);
    m_pageIndexes.insert(QStringLiteral("turnos"), m_stack->count() - 1);

    m_settingsPage = new SettingsView(m_stack);
    m_stack->addWidget(m_settingsPage);
    m_pageIndexes.insert(QStringLiteral("settings"), m_stack->count() - 1);

    m_usersPage = new UsersView(m_stack);
    m_stack->addWidget(m_usersPage);
    m_pageIndexes.insert(QStringLiteral("users"), m_stack->count() - 1);

    rightBox->addWidget(m_topBar);
    rightBox->addWidget(m_stack, 1);

    rootLayout->addWidget(m_sidebar);
    rootLayout->addLayout(rightBox, 1);

    setCentralWidget(central);

    m_sidebar->addItem(
        QStringLiteral("dashboard"),
        QStringLiteral("Dashboard"),
        QStringLiteral("⌂")
    );
    m_sidebar->addItem(
        QStringLiteral("donantes"),
        QStringLiteral("Donantes"),
        QStringLiteral("♥")
    );
    m_sidebar->addItem(
        QStringLiteral("turnos"),
        QStringLiteral("Turnos"),
        QStringLiteral("◷")
    );
    m_sidebar->addItem(
        QStringLiteral("settings"),
        QStringLiteral("Ajustes"),
        QStringLiteral("⚙"),
        UserRole::AREA_MANAGER
    );
    m_sidebar->addSeparator();
    m_sidebar->addItem(
        QStringLiteral("users"),
        QStringLiteral("Usuarios"),
        QStringLiteral("⌂"),
        UserRole::AREA_MANAGER
    );

    connect(m_sidebar, &Sidebar::itemClicked, this, &MainWindow::switchToPage);
    connect(m_topBar, &TopBar::logoutRequested, this, &MainWindow::logoutRequested);
}

void MainWindow::switchToPage(const QString &id)
{
    if (!m_pageIndexes.contains(id)) {
        return;
    }

    m_stack->setCurrentIndex(m_pageIndexes.value(id));
    m_sidebar->setActiveItem(id);

    if (id == QLatin1String("dashboard")) {
        m_dashboard->refresh();
    } else if (id == QLatin1String("donantes")) {
        m_donantes->refresh();
    } else if (id == QLatin1String("turnos")) {
        m_turnos->refresh();
    } else if (id == QLatin1String("settings")) {
        m_settingsPage->refresh();
    } else if (id == QLatin1String("users")) {
        m_usersPage->refresh();
    }
}
