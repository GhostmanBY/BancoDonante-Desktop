#include "views/features/settings/SettingsView.hpp"

#include "components/AppSettings.hpp"
#include "views/components/Toast.hpp"

#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QVBoxLayout>

SettingsView::SettingsView(QWidget *parent)
    : QWidget(parent),
      m_ruleService(new CycleRuleService(this))
{
    buildUi();
    refresh();

    connect(m_ruleService, &CycleRuleService::fetched, this,
        [this](const QList<CycleRuleItem> &rules) {
            clearRuleRows();
            if (rules.isEmpty()) {
                auto *empty = new QLabel(
                    QStringLiteral("No hay reglas de ciclo configuradas."), m_rulesBox);
                empty->setProperty("role", "hint");
                m_rulesLayout->addWidget(empty);
                return;
            }
            for (const CycleRuleItem &rule : rules) {
                buildRuleRow(m_rulesLayout, rule);
            }
        });
    connect(m_ruleService, &CycleRuleService::updateSucceeded, this, [this]() {
        Toast::show(this, QStringLiteral("Regla de ciclo guardada"), Toast::Type::Success);
        m_ruleService->fetchAll();
    });
    connect(m_ruleService, &CycleRuleService::failed, this, [this](const ApiError &error) {
        const QString message = error.detail.isEmpty()
            ? QStringLiteral("No se pudieron cargar las reglas de ciclo")
            : error.detail;
        Toast::show(this, message, Toast::Type::Error);
    });
}

void SettingsView::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 16);
    layout->setSpacing(16);

    auto *title = new QLabel(QStringLiteral("Ajustes"), this);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 18px; font-weight: 700; color: #111827; }"));

    auto *subtitle = new QLabel(
        QStringLiteral("Configuración general del sistema de gestión de donantes"), this);
    subtitle->setProperty("role", "hint");

    auto caption = [](const QString &text) {
        auto *label = new QLabel(text);
        label->setProperty("role", "hint");
        return label;
    };

    auto *section = new QLabel(QStringLiteral("Donaciones"), this);
    section->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 14px; font-weight: 600; color: #374151; }"));

    m_restDays = new QSpinBox(this);
    m_restDays->setRange(0, 365);
    m_restDays->setSuffix(QStringLiteral(" días"));
    m_restDays->setMinimumWidth(160);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignLeft);
    form->setSpacing(10);
    form->addRow(caption(QStringLiteral("Plazo mínimo de descanso entre donaciones")), m_restDays);

    auto *hint = new QLabel(
        QStringLiteral("Al dar un turno se avisa si el donante no cumplió el plazo. "
                       "La fecha de última donación se pinta de verde al cumplirlo."),
        this);
    hint->setProperty("role", "hint");
    hint->setWordWrap(true);

    auto *saveRestBtn = new QPushButton(QStringLiteral("Guardar"), this);
    saveRestBtn->setProperty("variant", "primary");
    saveRestBtn->setMaximumWidth(140);

    connect(saveRestBtn, &QPushButton::clicked, this, [this]() {
        AppSettings::instance().setRestDays(m_restDays->value());
        Toast::show(this, QStringLiteral("Ajustes guardados"), Toast::Type::Success);
    });

    auto *cycleSection = new QLabel(QStringLiteral("Reglas de ciclos de donación"), this);
    cycleSection->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 14px; font-weight: 600; color: #374151; }"));

    auto *cycleHint = new QLabel(
        QStringLiteral("Rigen la espera entre donaciones, entre series y el máximo por serie. "
                       "Se aplican al registrar una donación."),
        this);
    cycleHint->setProperty("role", "hint");
    cycleHint->setWordWrap(true);

    m_rulesBox = new QWidget(this);
    m_rulesLayout = new QVBoxLayout(m_rulesBox);
    m_rulesLayout->setContentsMargins(0, 0, 0, 0);
    m_rulesLayout->setSpacing(12);

    auto *rulesScroll = new QScrollArea(this);
    rulesScroll->setWidget(m_rulesBox);
    rulesScroll->setWidgetResizable(true);
    rulesScroll->setFrameShape(QFrame::NoFrame);
    rulesScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    rulesScroll->setMaximumHeight(320);

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addSpacing(8);
    layout->addWidget(section);
    layout->addLayout(form);
    layout->addWidget(hint);
    layout->addWidget(saveRestBtn, 0, Qt::AlignLeft);
    layout->addSpacing(8);
    layout->addWidget(cycleSection);
    layout->addWidget(cycleHint);
    layout->addWidget(rulesScroll, 1);
}

void SettingsView::buildRuleRow(QVBoxLayout *rulesLayout, const CycleRuleItem &rule)
{
    auto caption = [](const QString &text) {
        auto *label = new QLabel(text);
        label->setProperty("role", "hint");
        return label;
    };

    auto *box = new QWidget(m_rulesBox);
    auto *boxLayout = new QVBoxLayout(box);
    boxLayout->setContentsMargins(12, 12, 12, 12);
    boxLayout->setSpacing(8);

    const QString sexLabel = rule.sex.isEmpty()
        ? QStringLiteral("todos")
        : (rule.sex == QLatin1String("M") ? QStringLiteral("varones") : QStringLiteral("mujeres"));
    auto *ruleTitle = new QLabel(
        QStringLiteral("%1 (%2)").arg(rule.typeDonationName, sexLabel), box);
    ruleTitle->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 13px; font-weight: 600; color: #111827; }"));
    boxLayout->addWidget(ruleTitle);

    auto *maxDonations = new QSpinBox(box);
    maxDonations->setRange(1, 100);
    maxDonations->setValue(rule.maxDonations);
    maxDonations->setMinimumWidth(120);

    auto *waitDays = new QSpinBox(box);
    waitDays->setRange(0, 365);
    waitDays->setSuffix(QStringLiteral(" días"));
    waitDays->setValue(rule.waitDays);
    waitDays->setMinimumWidth(120);

    auto *waitBetween = new QSpinBox(box);
    waitBetween->setRange(0, 365);
    waitBetween->setSuffix(QStringLiteral(" días"));
    waitBetween->setValue(rule.waitBetweenCyclesDays);
    waitBetween->setMinimumWidth(120);

    auto *active = new QCheckBox(QStringLiteral("Activa"), box);
    active->setChecked(rule.isActive);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignLeft);
    form->setSpacing(8);
    form->addRow(caption(QStringLiteral("Máx. donaciones por serie")), maxDonations);
    form->addRow(caption(QStringLiteral("Espera entre donaciones de la serie")), waitDays);
    form->addRow(caption(QStringLiteral("Espera entre series")), waitBetween);
    form->addRow(QString(), active);

    auto *saveBtn = new QPushButton(QStringLiteral("Guardar regla"), box);
    saveBtn->setProperty("variant", "primary");
    saveBtn->setMaximumWidth(140);

    const int ruleId = rule.id;
    connect(saveBtn, &QPushButton::clicked, this, [this, ruleId, maxDonations, waitDays, waitBetween, active]() {
        m_ruleService->update(
            ruleId,
            maxDonations->value(),
            waitDays->value(),
            waitBetween->value(),
            active->isChecked());
    });

    boxLayout->addLayout(form);
    boxLayout->addWidget(saveBtn, 0, Qt::AlignLeft);
    rulesLayout->addWidget(box);
}

void SettingsView::clearRuleRows()
{
    while (QLayoutItem *item = m_rulesLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

void SettingsView::refresh()
{
    m_restDays->setValue(AppSettings::instance().restDays());
    m_ruleService->fetchAll();
}
