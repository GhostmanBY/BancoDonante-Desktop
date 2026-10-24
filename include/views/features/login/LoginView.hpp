#ifndef LOGINVIEW_H
#define LOGINVIEW_H

#include <QWidget>

class AuthService;
class QLabel;
class QLineEdit;
class QPushButton;
class QCheckBox;
class Spinner;

class LoginView : public QWidget
{
    Q_OBJECT

public:
    explicit LoginView(QWidget *parent = nullptr);

signals:
    void loginSucceeded();

private:
    void buildUi();
    void setLoading(bool loading);
    void attemptLogin();
    void showError(const QString &message);
    void clearError();

    AuthService *m_authService = nullptr;
    QLineEdit *m_usernameEdit = nullptr;
    QLineEdit *m_passwordEdit = nullptr;
    QCheckBox *m_rememberCheck = nullptr;
    QPushButton *m_loginButton = nullptr;
    QLabel *m_errorLabel = nullptr;
    Spinner *m_spinner = nullptr;
};

#endif
