#ifndef DASHBOARDVIEW_H
#define DASHBOARDVIEW_H

#include "services/ApiError.hpp"
#include "services/DashboardService.hpp"

#include <QWidget>

class QLabel;
class Spinner;

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

    QLabel *m_activos = nullptr;
    QLabel *m_apto = nullptr;
    QLabel *m_revision = nullptr;
    QLabel *m_noApto = nullptr;
    QLabel *m_suspended = nullptr;
    QLabel *m_sinContacto = nullptr;
    QLabel *m_turnosPendientes = nullptr;
    QLabel *m_turnosConfirmados = nullptr;
    QLabel *m_donacionesMes = nullptr;
    QLabel *m_error = nullptr;

    Spinner *m_spinner = nullptr;
    QWidget *m_cardsHost = nullptr;
};

#endif
