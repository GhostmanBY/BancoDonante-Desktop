#include "views/features/dashboard/DashboardView.hpp"

#include "views/components/Spinner.hpp"
#include "views/components/Toast.hpp"

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace {

QLabel *makeValueLabel(QWidget *parent, const QString &accent)
{
    auto *value = new QLabel(QStringLiteral("—"), parent);
    value->setAlignment(Qt::AlignCenter);
    value->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 30px; font-weight: 700; color: %1; background: transparent; }")
        .arg(accent));
    return value;
}

QWidget *makeCard(const QString &title, QLabel **valueOut, const QString &accent, QWidget *parent)
{
    auto *card = new QFrame(parent);
    card->setObjectName(QStringLiteral("kpiCard"));

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(6);

    auto *caption = new QLabel(title, card);
    caption->setAlignment(Qt::AlignCenter);
    caption->setStyleSheet(QStringLiteral(
        "QLabel { color: #6B7280; font-size: 12px; font-weight: 600; background: transparent; }"));

    *valueOut = makeValueLabel(card, accent);

    layout->addWidget(*valueOut);
    layout->addWidget(caption);
    return card;
}

} // namespace

DashboardView::DashboardView(QWidget *parent)
    : QWidget(parent),
      m_service(new DashboardService(this))
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

    refresh();
}

void DashboardView::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 20, 24, 16);
    layout->setSpacing(16);

    auto *title = new QLabel(QStringLiteral("Dashboard"), this);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 20px; font-weight: 700; color: #111827; }"));

    auto *subtitle = new QLabel(QStringLiteral("Estado general del banco de sangre"), this);
    subtitle->setProperty("role", "hint");

    m_error = new QLabel(this);
    m_error->setProperty("role", "error");
    m_error->setWordWrap(true);
    m_error->setVisible(false);

    m_spinner = new Spinner(this);
    m_spinner->setFixedSize(48, 48);

    m_cardsHost = new QWidget(this);
    auto *grid = new QGridLayout(m_cardsHost);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(14);

    grid->addWidget(makeCard(QStringLiteral("Donantes activos"), &m_activos, QStringLiteral("#B91C1C"), m_cardsHost), 0, 0);
    grid->addWidget(makeCard(QStringLiteral("Aptos"), &m_apto, QStringLiteral("#065F46"), m_cardsHost), 0, 1);
    grid->addWidget(makeCard(QStringLiteral("En revisión"), &m_revision, QStringLiteral("#92400E"), m_cardsHost), 0, 2);
    grid->addWidget(makeCard(QStringLiteral("No aptos"), &m_noApto, QStringLiteral("#991B1B"), m_cardsHost), 1, 0);
    grid->addWidget(makeCard(QStringLiteral("Suspendidos"), &m_suspended, QStringLiteral("#4B5563"), m_cardsHost), 1, 1);
    grid->addWidget(makeCard(QStringLiteral("Sin contacto"), &m_sinContacto, QStringLiteral("#C2410C"), m_cardsHost), 1, 2);
    grid->addWidget(makeCard(QStringLiteral("Turnos pendientes hoy"), &m_turnosPendientes, QStringLiteral("#1D4ED8"), m_cardsHost), 2, 0);
    grid->addWidget(makeCard(QStringLiteral("Turnos confirmados hoy"), &m_turnosConfirmados, QStringLiteral("#1E40AF"), m_cardsHost), 2, 1);
    grid->addWidget(makeCard(QStringLiteral("Donaciones este mes"), &m_donacionesMes, QStringLiteral("#DC2626"), m_cardsHost), 2, 2);

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addWidget(m_error);
    layout->addWidget(m_cardsHost, 1);
    layout->addWidget(m_spinner, 0, Qt::AlignHCenter);

    m_spinner->start();
    m_cardsHost->setEnabled(false);
}

void DashboardView::refresh()
{
    setLoading(true);
    m_error->setVisible(false);
    m_service->fetchStats();
}

void DashboardView::setLoading(bool loading)
{
    m_spinner->setVisible(loading);
    if (loading) {
        m_spinner->start();
    } else {
        m_spinner->stop();
    }
    m_cardsHost->setEnabled(!loading);
}

void DashboardView::applyStats(const DashboardStats &stats)
{
    m_activos->setText(QString::number(stats.donantesActivos));
    m_apto->setText(QString::number(stats.donantesApto));
    m_revision->setText(QString::number(stats.donantesRevision));
    m_noApto->setText(QString::number(stats.donantesNoApto));
    m_suspended->setText(QString::number(stats.donantesSuspended));
    m_sinContacto->setText(QString::number(stats.donantesSinContacto));
    m_turnosPendientes->setText(QString::number(stats.turnosHoyPendientes));
    m_turnosConfirmados->setText(QString::number(stats.turnosHoyConfirmados));
    m_donacionesMes->setText(QString::number(stats.donacionesMes));
}
