#include <QApplication>
#include <QColor>
#include <QFile>
#include <QLocale>
#include <QPalette>

#include "services/SessionManager.hpp"
#include "views/features/login/LoginView.hpp"
#include "views/features/main/MainWindow.hpp"

namespace {

void loadTheme(QApplication &app)
{
    QFile file(QStringLiteral(":/resources/theme.qss"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        app.setStyleSheet(QString::fromUtf8(file.readAll()));
    }
}

void applyLightPalette(QApplication &app)
{
    const QColor text(QStringLiteral("#1F2937"));
    const QColor hint(QStringLiteral("#6B7280"));
    const QColor disabled(QStringLiteral("#9CA3AF"));

    const QPalette::ColorGroup groups[] = {
        QPalette::Active, QPalette::Inactive, QPalette::Disabled};

    QPalette pal = app.palette();
    for (const QPalette::ColorGroup group : groups) {
        pal.setColor(group, QPalette::Window, QColor(QStringLiteral("#F4F5F7")));
        pal.setColor(group, QPalette::WindowText, text);
        pal.setColor(group, QPalette::Base, QColor(QStringLiteral("#FFFFFF")));
        pal.setColor(group, QPalette::Text, text);
        pal.setColor(group, QPalette::Button, QColor(QStringLiteral("#FFFFFF")));
        pal.setColor(group, QPalette::ButtonText, text);
        pal.setColor(group, QPalette::AlternateBase, QColor(QStringLiteral("#F9FAFB")));
        pal.setColor(group, QPalette::Highlight, QColor(QStringLiteral("#B91C1C")));
        pal.setColor(group, QPalette::HighlightedText, QColor(QStringLiteral("#FFFFFF")));
        pal.setColor(group, QPalette::ToolTipBase, QColor(QStringLiteral("#FFFFFF")));
        pal.setColor(group, QPalette::ToolTipText, text);
        pal.setColor(group, QPalette::PlaceholderText, hint);
        pal.setColor(group, QPalette::Link, QColor(QStringLiteral("#B91C1C")));
        pal.setColor(group, QPalette::LinkVisited, QColor(QStringLiteral("#7F1D1D")));
    }
    pal.setColor(QPalette::Disabled, QPalette::WindowText, disabled);
    pal.setColor(QPalette::Disabled, QPalette::Text, disabled);
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, disabled);
    app.setPalette(pal);
}

void showMainWindow();

void showLogin()
{
    auto *login = new LoginView;
    login->setAttribute(Qt::WA_DeleteOnClose);

    QObject::connect(login, &LoginView::loginSucceeded, login, []() {
        showMainWindow();
    });

    login->show();
}

void showMainWindow()
{
    auto *window = new MainWindow;
    window->setAttribute(Qt::WA_DeleteOnClose);

    QObject::connect(window, &MainWindow::logoutRequested, window, [window]() {
        SessionManager::instance().clear();
        showLogin();
        window->deleteLater();
    });

    QObject::connect(
        &SessionManager::instance(),
        &SessionManager::sessionExpired,
        window,
        [window]() {
            window->close();
            showLogin();
        }
    );

    window->show();
}

} // namespace

int main(int argc, char *argv[])
{
    QLocale::setDefault(QLocale(QLocale::Spanish, QLocale::Argentina));

    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("BancoDonante"));
    app.setOrganizationName(QStringLiteral("BancoDonante"));

    applyLightPalette(app);
    loadTheme(app);
    showLogin();

    return app.exec();
}
