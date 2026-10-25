#ifndef USERDIALOG_H
#define USERDIALOG_H

#include "services/ApiError.hpp"
#include "services/UsersService.hpp"

#include <QDialog>
#include <optional>

class QAction;
class QComboBox;
class QDateEdit;
class QLabel;
class QLineEdit;
class QPushButton;

class UserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserDialog(
        const std::optional<UserItem> &user,
        QWidget *parent = nullptr);

signals:
    void saved();

private:
    void buildUi();
    void prefill();
    bool validate();
    void submit();
    void onFailed(const ApiError &error);
    bool isCreate() const;

    UsersService *m_service = nullptr;
    std::optional<UserItem> m_user;

    QLineEdit *m_username = nullptr;
    QLineEdit *m_password = nullptr;
    QAction *m_revealAction = nullptr;
    QLineEdit *m_name = nullptr;
    QLineEdit *m_lastname = nullptr;
    QLineEdit *m_mail = nullptr;
    QLineEdit *m_phone = nullptr;
    QComboBox *m_documentType = nullptr;
    QLineEdit *m_documentNumber = nullptr;
    QDateEdit *m_birthDate = nullptr;
    QLineEdit *m_birthPlace = nullptr;
    QComboBox *m_role = nullptr;
    QLabel *m_formError = nullptr;
    QPushButton *m_acceptBtn = nullptr;
};

#endif
