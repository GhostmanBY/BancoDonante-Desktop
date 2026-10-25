#include "views/features/dashboard/DashboardView.hpp"

#include "views/components/Spinner.hpp"
#include "views/components/Toast.hpp"

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPainter>
#include <QDate>
#include <QSize>
#include <QVBoxLayout>

#include <algorithm>

class AppointmentStatusChart : public QWidget
{
public:
    explicit AppointmentStatusChart(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setMinimumHeight(250);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }

    void setData(int completed, int pending, int confirmed)
    {
        m_values = {completed, pending, confirmed};
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const QStringList labels = {
            QStringLiteral("Realizados"), QStringLiteral("Pendientes"), QStringLiteral("Confirmados")};
        const QList<QColor> colors = {
            QColor(QStringLiteral("#15803D")), QColor(QStringLiteral("#D97706")),
            QColor(QStringLiteral("#2563EB"))};
        const int total = m_values.at(0) + m_values.at(1) + m_values.at(2);
        const int diameter = qMin(184, qMin(width() - 24, height() - 74));
        const QRectF pieRect((width() - diameter) / 2.0, 4, diameter, diameter);

        painter.setPen(Qt::NoPen);
        if (total == 0) {
            painter.setBrush(QColor(QStringLiteral("#E5E7EB")));
            painter.drawEllipse(pieRect);
            painter.setPen(QColor(QStringLiteral("#6B7280")));
            QFont emptyFont = painter.font();
            emptyFont.setPixelSize(12);
            painter.setFont(emptyFont);
            painter.drawText(pieRect, Qt::AlignCenter, QStringLiteral("Sin turnos hoy"));
        } else {
            int startAngle = 90 * 16;
            for (int i = 0; i < m_values.size(); ++i) {
                if (m_values.at(i) <= 0) {
                    continue;
                }
                const int span = qRound(360.0 * 16.0 * m_values.at(i) / total);
                painter.setBrush(colors.at(i));
                painter.drawPie(pieRect, startAngle, -span);
                startAngle -= span;
            }
        }

        const int legendY = diameter + 14;
        const int columnWidth = width() / labels.size();
        QFont legendFont = painter.font();
        legendFont.setPixelSize(11);
        painter.setFont(legendFont);
        for (int i = 0; i < labels.size(); ++i) {
            const int x = i * columnWidth;
            painter.setPen(Qt::NoPen);
            painter.setBrush(colors.at(i));
            painter.drawEllipse(QRectF(x + 5, legendY + 4, 9, 9));

            painter.setPen(QColor(QStringLiteral("#374151")));
            const QString countLabel = total > 0
                ? QStringLiteral("%1 (%2%)").arg(m_values.at(i)).arg(qRound(100.0 * m_values.at(i) / total))
                : QString::number(m_values.at(i));
            painter.drawText(QRect(x + 18, legendY, columnWidth - 20, 20),
                Qt::AlignLeft | Qt::AlignVCenter,
                QStringLiteral("%1 · %2").arg(labels.at(i), countLabel));
        }
    }

private:
    QList<int> m_values{0, 0, 0};
};

namespace {

QFrame *makeStatPanel(
    const QString &title,
    const QString &hintText,
    QLabel **valueOut,
    const QString &accent,
    QWidget *parent)
{
    auto *panel = new QFrame(parent);
    panel->setObjectName(QStringLiteral("primaryDashboardStat"));
    panel->setStyleSheet(QStringLiteral(
        "QFrame#primaryDashboardStat { background: #FFFFFF; border: 1px solid #E5E7EB; border-radius: 10px; }"));

    auto *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(5);

    auto *heading = new QLabel(title, panel);
    heading->setStyleSheet(QStringLiteral(
        "QLabel { color: #4B5563; font-size: 13px; font-weight: 600; background: transparent; }"));
    *valueOut = new QLabel(QStringLiteral("—"), panel);
    (*valueOut)->setStyleSheet(QStringLiteral(
        "QLabel { color: %1; font-size: 34px; font-weight: 700; background: transparent; }").arg(accent));
    auto *hint = new QLabel(hintText, panel);
    hint->setProperty("role", "hint");
    hint->setStyleSheet(QStringLiteral("QLabel { background: transparent; }"));

    layout->addWidget(heading);
    layout->addWidget(*valueOut);
    layout->addWidget(hint);
    return panel;
}

QFrame *makeTodayPanel(QWidget *parent, AppointmentStatusChart **chartOut)
{
    auto *panel = new QFrame(parent);
    panel->setObjectName(QStringLiteral("appointmentsDashboardPanel"));
    panel->setStyleSheet(QStringLiteral(
        "QFrame#appointmentsDashboardPanel { background: #FFFFFF; border: 1px solid #E5E7EB; border-radius: 10px; }"));

    auto *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(20, 18, 20, 16);
    layout->setSpacing(4);

    auto *heading = new QLabel(QStringLiteral("Turnos de hoy"), panel);
    heading->setStyleSheet(QStringLiteral(
        "QLabel { color: #1F2937; font-size: 16px; font-weight: 700; background: transparent; }"));
    auto *hint = new QLabel(QStringLiteral("Realizados, pendientes y confirmados"), panel);
    hint->setProperty("role", "hint");
    hint->setStyleSheet(QStringLiteral("QLabel { background: transparent; }"));

    *chartOut = new AppointmentStatusChart(panel);
    layout->addWidget(heading);
    layout->addWidget(hint);
    layout->addWidget(*chartOut, 1);
    return panel;
}

QFrame *makeUpcomingPanel(QWidget *parent, QListWidget **upcomingOut)
{
    auto *panel = new QFrame(parent);
    panel->setObjectName(QStringLiteral("appointmentsDashboardPanel"));
    panel->setStyleSheet(QStringLiteral(
        "QFrame#appointmentsDashboardPanel { background: #FFFFFF; border: 1px solid #E5E7EB; border-radius: 10px; }"));

    auto *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(20, 18, 20, 16);
    layout->setSpacing(8);
    auto *heading = new QLabel(QStringLiteral("Próximos turnos"), panel);
    heading->setStyleSheet(QStringLiteral(
        "QLabel { color: #1F2937; font-size: 16px; font-weight: 700; background: transparent; }"));
    *upcomingOut = new QListWidget(panel);
    (*upcomingOut)->setFrameShape(QFrame::NoFrame);
    (*upcomingOut)->setSpacing(4);
    (*upcomingOut)->setStyleSheet(QStringLiteral(
        "QListWidget { background: transparent; border: none; }"
        "QListWidget::item { background: #F9FAFB; border: 1px solid #F3F4F6; border-radius: 6px; padding: 9px 10px; color: #374151; }"));
    (*upcomingOut)->setUniformItemSizes(true);
    layout->addWidget(heading);
    layout->addWidget(*upcomingOut, 1);
    return panel;
}

QFrame *makeOldestDonorsPanel(QWidget *parent, QGridLayout **gridOut)
{
    auto *panel = new QFrame(parent);
    panel->setObjectName(QStringLiteral("oldestDonorsDashboardPanel"));
    panel->setStyleSheet(QStringLiteral(
        "QFrame#oldestDonorsDashboardPanel { background: #FFFFFF; border: 1px solid #E5E7EB; border-radius: 10px; }"));

    auto *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(20, 18, 20, 18);
    layout->setSpacing(5);

    auto *heading = new QLabel(QStringLiteral("Donantes con más tiempo sin donar"), panel);
    heading->setStyleSheet(QStringLiteral(
        "QLabel { color: #1F2937; font-size: 16px; font-weight: 700; background: transparent; }"));
    auto *hint = new QLabel(QStringLiteral("Donantes activos ordenados por su última donación"), panel);
    hint->setProperty("role", "hint");

    auto *gridWidget = new QWidget(panel);
    *gridOut = new QGridLayout(gridWidget);
    (*gridOut)->setContentsMargins(0, 8, 0, 0);
    (*gridOut)->setHorizontalSpacing(12);
    (*gridOut)->setVerticalSpacing(10);

    layout->addWidget(heading);
    layout->addWidget(hint);
    layout->addWidget(gridWidget);
    return panel;
}

QFrame *makeDonorTile(const Patientes &donor, int position, QWidget *parent)
{
    auto *tile = new QFrame(parent);
    tile->setStyleSheet(QStringLiteral(
        "QFrame { background: #F9FAFB; border: 1px solid #E5E7EB; border-radius: 8px; }"));
    auto *layout = new QHBoxLayout(tile);
    layout->setContentsMargins(12, 10, 12, 10);
    layout->setSpacing(10);

    auto *index = new QLabel(QString::number(position), tile);
    index->setAlignment(Qt::AlignCenter);
    index->setFixedSize(28, 28);
    index->setStyleSheet(QStringLiteral(
        "QLabel { color: #991B1B; background: #FEE2E2; border: none; border-radius: 14px; font-weight: 700; }"));

    auto *details = new QVBoxLayout();
    details->setSpacing(2);
    auto *name = new QLabel((donor.name() + QLatin1Char(' ') + donor.lastname()).trimmed(), tile);
    name->setStyleSheet(QStringLiteral(
        "QLabel { color: #1F2937; font-size: 13px; font-weight: 600; background: transparent; border: none; }"));
    QString lastDonation = QStringLiteral("Sin donaciones registradas");
    if (donor.lastDonationDate().has_value() && donor.lastDonationDate()->isValid()) {
        const QDate date = donor.lastDonationDate()->date();
        const int days = date.daysTo(QDate::currentDate());
        lastDonation = QStringLiteral("Última: %1 · hace %2 días")
            .arg(date.toString(QStringLiteral("dd/MM/yyyy")))
            .arg(qMax(0, days));
    }
    auto *date = new QLabel(lastDonation, tile);
    date->setStyleSheet(QStringLiteral(
        "QLabel { color: #6B7280; font-size: 11px; background: transparent; border: none; }"));
    details->addWidget(name);
    details->addWidget(date);
    layout->addWidget(index);
    layout->addLayout(details, 1);
    return tile;
}

} // namespace

DashboardView::DashboardView(QWidget *parent)
    : QWidget(parent),
      m_service(new DashboardService(this)),
      m_appointmentService(new AppointmentService(this)),
      m_donorService(new DonorService(this))
{
    buildUi();

    connect(m_service, &DashboardService::statsFetched, this, [this](const DashboardStats &stats) {
        applyStats(stats);
        setLoading(false);
        m_error->setVisible(false);
    });
    connect(m_service, &DashboardService::failed, this, [this](const ApiError &error) {
        setLoading(false);
        m_error->setText(error.detail.isEmpty()
            ? QStringLiteral("No se pudieron cargar las estadísticas")
            : error.detail);
        m_error->setVisible(true);
        Toast::show(this, m_error->text(), Toast::Type::Error);
    });
    connect(m_appointmentService, &AppointmentService::pageFetched, this, [this](const TurnosPage &page) {
        m_upcomingAppointments->clear();
        if (page.data.isEmpty()) {
            m_upcomingAppointments->addItem(QStringLiteral("No hay turnos próximos"));
            return;
        }

        for (const TurnoItem &appointment : page.data) {
            const QString name = (appointment.patientName + QLatin1Char(' ') + appointment.patientLastname).trimmed();
            const QString date = appointment.datetime.toLocalTime().toString(QStringLiteral("dd/MM · HH:mm"));
            const QString state = appointmentStateLabel(appointment.state);
            auto *item = new QListWidgetItem(QStringLiteral("%1\n%2 · %3")
                .arg(name.isEmpty() ? QStringLiteral("Donante") : name, date, state), m_upcomingAppointments);
            item->setSizeHint(QSize(item->sizeHint().width(), 56));
        }
    });
    connect(m_donorService, &DonorService::pageFetched, this, [this](const DonantesPage &page) {
        while (QLayoutItem *item = m_donorGrid->takeAt(0)) {
            if (QWidget *widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }
        if (page.data.isEmpty()) {
            auto *empty = new QLabel(QStringLiteral("No hay donantes activos para mostrar"), m_donorGrid->parentWidget());
            empty->setProperty("role", "hint");
            m_donorGrid->addWidget(empty, 0, 0, 1, 2);
            return;
        }
        for (int i = 0; i < page.data.size(); ++i) {
            m_donorGrid->addWidget(makeDonorTile(page.data.at(i), i + 1, m_donorGrid->parentWidget()), i / 2, i % 2);
        }
    });

    refresh();
}

void DashboardView::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 20, 24, 16);
    layout->setSpacing(14);

    auto *title = new QLabel(QStringLiteral("Dashboard"), this);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 20px; font-weight: 700; color: #111827; background: transparent; }"));
    auto *subtitle = new QLabel(QStringLiteral("Indicadores principales del banco de sangre"), this);
    subtitle->setProperty("role", "hint");

    m_error = new QLabel(this);
    m_error->setProperty("role", "error");
    m_error->setWordWrap(true);
    m_error->setVisible(false);

    m_dashboardContent = new QWidget(this);
    auto *contentLayout = new QVBoxLayout(m_dashboardContent);
    contentLayout->setContentsMargins(0, 2, 0, 0);
    contentLayout->setSpacing(16);

    auto *statsRow = new QHBoxLayout();
    statsRow->setSpacing(14);
    auto *donationsPanel = makeStatPanel(
        QStringLiteral("Donaciones este mes"),
        QStringLiteral("Donaciones registradas durante el mes actual"),
        &m_monthlyDonations,
        QStringLiteral("#B91C1C"),
        m_dashboardContent);
    auto *activeDonorsPanel = makeStatPanel(
        QStringLiteral("Donantes activos"),
        QStringLiteral("Padrón habilitado para la gestión"),
        &m_activeDonors,
        QStringLiteral("#1F2937"),
        m_dashboardContent);
    m_noContact = new QLabel(QStringLiteral("Sin contacto: —"), activeDonorsPanel);
    m_noContact->setProperty("role", "hint");
    m_noContact->setStyleSheet(QStringLiteral("QLabel { background: transparent; }"));
    qobject_cast<QVBoxLayout *>(activeDonorsPanel->layout())->addWidget(m_noContact);

    statsRow->addWidget(donationsPanel, 1);
    statsRow->addWidget(activeDonorsPanel, 1);

    AppointmentStatusChart *appointmentChart = nullptr;
    QListWidget *upcomingAppointments = nullptr;
    auto *oldestDonorsPanel = makeOldestDonorsPanel(m_dashboardContent, &m_donorGrid);
    auto *todayPanel = makeTodayPanel(m_dashboardContent, &appointmentChart);
    m_appointmentChart = appointmentChart;
    auto *upcomingPanel = makeUpcomingPanel(m_dashboardContent, &upcomingAppointments);
    m_upcomingAppointments = upcomingAppointments;

    auto *activityRow = new QHBoxLayout();
    activityRow->setSpacing(14);
    auto *leftColumn = new QVBoxLayout();
    leftColumn->setSpacing(14);
    leftColumn->addWidget(todayPanel, 1);
    leftColumn->addWidget(oldestDonorsPanel);
    auto *leftWidget = new QWidget(m_dashboardContent);
    leftWidget->setLayout(leftColumn);
    activityRow->addWidget(leftWidget, 1);
    activityRow->addWidget(upcomingPanel, 1);

    contentLayout->addLayout(statsRow);
    contentLayout->addLayout(activityRow, 1);
    contentLayout->addStretch(1);

    m_spinner = new Spinner(this);
    m_spinner->setFixedSize(42, 42);
    m_spinner->setVisible(false);

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addWidget(m_error);
    layout->addWidget(m_dashboardContent, 1);
    layout->addWidget(m_spinner, 0, Qt::AlignHCenter);
}

void DashboardView::refresh()
{
    setLoading(true);
    m_error->setVisible(false);
    m_service->fetchStats();
    m_upcomingAppointments->clear();
    m_appointmentService->fetchPage(1, 5);
    m_donorService->fetchPage(1, 6, QString(), std::nullopt, true);
}

void DashboardView::setLoading(bool loading)
{
    m_spinner->setVisible(loading);
    if (loading) {
        m_spinner->start();
    } else {
        m_spinner->stop();
    }
    m_dashboardContent->setEnabled(!loading);
}

void DashboardView::applyStats(const DashboardStats &stats)
{
    m_monthlyDonations->setText(QString::number(stats.donacionesMes));
    m_activeDonors->setText(QString::number(stats.donantesActivos));
    m_noContact->setText(QStringLiteral("Sin contacto: %1").arg(stats.donantesSinContacto));
    m_appointmentChart->setData(
        stats.turnosHoyRealizados,
        stats.turnosHoyPendientes,
        stats.turnosHoyConfirmados);
}
