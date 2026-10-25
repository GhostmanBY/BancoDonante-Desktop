#ifndef ASSIGNTURNODIALOG_H
#define ASSIGNTURNODIALOG_H

#include "services/ApiError.hpp"
#include "services/UsersService.hpp"

#include <QDialog>

class UserXAppointmentService;
class QComboBox;
class QLabel;
class QPushButton;

class AssignTurnoDialog : public QDialog
{
    Q_OBJECT

public:
    AssignTurnoDialog(int appointmentId, QWidget *parent = nullptr);

signals:
    void assigned();

private:
    void onUsersFetched(const UsersPage &page);
    void onFailed(const ApiError &error);
    void submit();

    int m_appointmentId = 0;
    UsersService *m_usersService = nullptr;
    UserXAppointmentService *m_assignmentService = nullptr;
    QComboBox *m_user = nullptr;
    QLabel *m_error = nullptr;
    QPushButton *m_accept = nullptr;
    bool m_canChooseUser = false;
};

#endif
