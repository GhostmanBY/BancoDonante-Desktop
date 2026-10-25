#ifndef DASHBOARDVIEW_H
#define DASHBOARDVIEW_H

#include "services/ApiError.hpp"
#include "services/DashboardService.hpp"
#include "services/AppointmentService.hpp"
#include "services/DonorService.hpp"

#include <QWidget>

class QLabel;
class Spinner;
class AppointmentStatusChart;
class QListWidget;
class QGridLayout;

class DashboardView : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardView(QWidget *parent = nullptr);

    void refresh();

private:
    void buildUi();
    void applyStats(const DashboardStats &stats);
    void setLoading(bool loading);

    DashboardService *m_service = nullptr;
    AppointmentService *m_appointmentService = nullptr;
    DonorService *m_donorService = nullptr;

    QLabel *m_error = nullptr;
    QLabel *m_monthlyDonations = nullptr;
    QLabel *m_activeDonors = nullptr;
    QLabel *m_noContact = nullptr;

    Spinner *m_spinner = nullptr;
    QWidget *m_dashboardContent = nullptr;
    AppointmentStatusChart *m_appointmentChart = nullptr;
    QListWidget *m_upcomingAppointments = nullptr;
    QGridLayout *m_donorGrid = nullptr;
};

#endif
