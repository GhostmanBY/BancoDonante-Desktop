#include "views/features/login/LoginView.hpp"

#include "config/AppConfig.hpp"
#include "services/AuthService.hpp"
#include "views/components/PasswordReveal.hpp"
#include "views/components/Spinner.hpp"

#include <QAction>
#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

LoginView::LoginView(QWidget *parent)
    : QWidget(parent),
      m_authService(new AuthService(this))
{
    setWindowTitle(QStringLiteral("Banco de Sangre — Iniciar sesión"));
    setMinimumSize(420, 480);
    resize(420, 480);

    buildUi();

    const QString remembered = AppConfig::instance().rememberedUsername();
    if (!remembered.isEmpty()) {
        m_usernameEdit->setText(remembered);
        m_rememberCheck->setChecked(true);
        m_passwordEdit->setFocus();
    }

    connect(m_authService, &AuthService::loginSucceeded, this, [this]() {
        if (m_rememberCheck->isChecked()) {
            AppConfig::instance().setRememberedUsername(m_usernameEdit->text().trimmed());
        } else {
            AppConfig::instance().setRememberedUsername(QString());
        }

        setLoading(false);
        emit loginSucceeded();
        LoginView::close();
    });

    connect(m_authService, &AuthService::loginFailed, this, [this](const ApiError &error) {
        setLoading(false);
        showError(error.detail);
    });
}

void LoginView::buildUi()
{
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(24, 24, 24, 24);

    outerLayout->addStretch();

    auto *card = new QFrame(this);
    card->setObjectName(QStringLiteral("loginCard"));
    card->setStyleSheet(QStringLiteral(
        "QFrame#loginCard {"
        "  background: #FFFFFF;"
        "  border: 1px solid #E5E7EB;"
        "  border-radius: 12px;"
        "  padding: 32px;"
        "}"
    ));
    card->setMinimumWidth(360);

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(16);

    auto *title = new QLabel(QStringLiteral("Banco de Sangre"), card);
    title->setProperty("role", "login-title");

    auto *subtitle = new QLabel(
        QStringLiteral("Iniciá sesión para continuar"),
        card
    );
    subtitle->setProperty("role", "login-subtitle");

    cardLayout->addWidget(title);
    cardLayout->addWidget(subtitle);
    cardLayout->addSpacing(8);

    m_usernameEdit = new QLineEdit(card);
    m_usernameEdit->setPlaceholderText(QStringLiteral("Usuario"));
    m_usernameEdit->setObjectName(QStringLiteral("usernameEdit"));

    m_passwordEdit = new QLineEdit(card);
    m_passwordEdit->setPlaceholderText(QStringLiteral("Contraseña"));
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setObjectName(QStringLiteral("passwordEdit"));

    m_revealAction = PasswordReveal::attach(m_passwordEdit);

    m_rememberCheck = new QCheckBox(QStringLiteral("Recordar usuario"), card);

    m_errorLabel = new QLabel(card);
    m_errorLabel->setProperty("role", "login-error");
    m_errorLabel->setWordWrap(true);
    m_errorLabel->setVisible(false);

    m_loginButton = new QPushButton(QStringLiteral("Iniciar sesión"), card);
    m_loginButton->setProperty("variant", "primary");
    m_loginButton->setObjectName(QStringLiteral("loginButton"));
    m_loginButton->setCursor(Qt::PointingHandCursor);
    m_loginButton->setDefault(true);

    m_spinner = new Spinner(card);
    m_spinner->setFixedSize(20, 20);
    m_spinner->stop();

    auto *buttonRow = new QHBoxLayout();
    buttonRow->addWidget(m_spinner);
    buttonRow->addWidget(m_loginButton, 1);

    cardLayout->addWidget(m_usernameEdit);
    cardLayout->addWidget(m_passwordEdit);
    cardLayout->addWidget(m_rememberCheck);
    cardLayout->addWidget(m_errorLabel);
    cardLayout->addSpacing(4);
    cardLayout->addLayout(buttonRow);

    outerLayout->addWidget(card, 0, Qt::AlignCenter);
    outerLayout->addStretch();

    connect(m_usernameEdit, &QLineEdit::returnPressed, this, &LoginView::attemptLogin);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginView::attemptLogin);
    connect(m_loginButton, &QPushButton::clicked, this, &LoginView::attemptLogin);
}

void LoginView::setLoading(bool loading)
{
    m_usernameEdit->setEnabled(!loading);
    m_passwordEdit->setEnabled(!loading);
    m_rememberCheck->setEnabled(!loading);
    m_revealAction->setEnabled(!loading);
    m_loginButton->setEnabled(!loading);
    m_loginButton->setText(
        loading ? QStringLiteral("Ingresando…") : QStringLiteral("Iniciar sesión")
    );

    if (loading) {
        m_spinner->start();
    } else {
        m_spinner->stop();
    }
}

void LoginView::attemptLogin()
{
    const QString username = m_usernameEdit->text().trimmed();
    const QString password = m_passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        showError(QStringLiteral("Ingresá usuario y contraseña."));
        return;
    }

    clearError();
    setLoading(true);
    m_authService->login(username, password);
}

void LoginView::showError(const QString &message)
{
    m_errorLabel->setText(message);
    m_errorLabel->setVisible(true);
}

void LoginView::clearError()
{
    m_errorLabel->clear();
    m_errorLabel->setVisible(false);
}
