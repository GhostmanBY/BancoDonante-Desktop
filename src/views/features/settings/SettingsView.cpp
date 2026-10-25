#include "views/features/settings/SettingsView.hpp"

#include "views/components/Toast.hpp"
#include "services/WhatsAppService.hpp"

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFrame>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollArea>
#include <QSet>
#include <QSpinBox>
#include <QStringList>
#include <QVBoxLayout>

namespace {

QLabel *makeHint(const QString &text, QWidget *parent = nullptr)
{
    auto *label = new QLabel(text, parent);
    label->setProperty("role", "hint");
    label->setWordWrap(true);
    return label;
}

QLabel *makeSectionTitle(const QString &text, QWidget *parent = nullptr)
{
    auto *label = new QLabel(text, parent);
    label->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 15px; font-weight: 700; color: #1F2937; background: transparent; }"));
    return label;
}

QFrame *makeCard(QWidget *parent = nullptr)
{
    auto *card = new QFrame(parent);
    card->setObjectName(QStringLiteral("settingsCard"));
    card->setStyleSheet(QStringLiteral(
        "QFrame#settingsCard { background: #FFFFFF; border: 1px solid #E5E7EB; border-radius: 10px; }"));
    return card;
}

QLabel *makeFieldLabel(const QString &text, QWidget *parent = nullptr)
{
    auto *label = new QLabel(text, parent);
    label->setStyleSheet(QStringLiteral(
        "QLabel { color: #6B7280; font-size: 12px; background: transparent; }"));
    return label;
}

QString templatePreview(QString text)
{
    return text
        .replace(QStringLiteral("{{nombre}}"), QStringLiteral("María"))
        .replace(QStringLiteral("{{apellido}}"), QStringLiteral("Gómez"))
        .replace(QStringLiteral("{{nombre_completo}}"), QStringLiteral("María Gómez"))
        .replace(QStringLiteral("{{telefono}}"), QStringLiteral("+54 11 5555-1234"))
        .replace(QStringLiteral("{{fecha}}"), QStringLiteral("15/11/2026"))
        .replace(QStringLiteral("{{hora}}"), QStringLiteral("09:30"));
}

} // namespace

SettingsView::SettingsView(QWidget *parent)
    : QWidget(parent),
      m_ruleService(new CycleRuleService(this)),
      m_whatsAppService(new WhatsAppService(this))
{
    buildUi();
    refresh();

    connect(m_ruleService, &CycleRuleService::fetched, this,
        [this](const QList<CycleRuleItem> &rules) {
            clearRuleRows();
            if (rules.isEmpty()) {
                auto *empty = new QLabel(
                    QStringLiteral("Todavía no hay reglas de ciclo configuradas."), m_rulesBox);
                empty->setProperty("role", "hint");
                empty->setAlignment(Qt::AlignCenter);
                empty->setMinimumHeight(76);
                empty->setStyleSheet(QStringLiteral(
                    "QLabel { color: #6B7280; background: #F9FAFB; border: 1px dashed #D1D5DB; border-radius: 8px; padding: 16px; }"));
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

    connect(m_whatsAppService, &WhatsAppService::managedTemplatesFetched, this,
        [this](const QList<WSPTemplateItem> &templates) {
            clearTemplateRows();
            if (templates.isEmpty()) {
                auto *empty = makeHint(
                    QStringLiteral("No hay plantillas de WhatsApp configuradas."), m_templatesBox);
                empty->setAlignment(Qt::AlignCenter);
                empty->setMinimumHeight(72);
                empty->setStyleSheet(QStringLiteral(
                    "QLabel { color: #6B7280; background: #F9FAFB; border: 1px dashed #D1D5DB; border-radius: 8px; padding: 16px; }"));
                m_templatesLayout->addWidget(empty);
                return;
            }
            for (const WSPTemplateItem &item : templates) {
                buildTemplateRow(m_templatesLayout, item);
            }
        });
    connect(m_whatsAppService, &WhatsAppService::templateSaved, this, [this](int) {
        Toast::show(this, QStringLiteral("Plantilla guardada"), Toast::Type::Success);
        m_whatsAppService->fetchTemplates();
    });
    connect(m_whatsAppService, &WhatsAppService::templateActivationChanged, this,
        [this](int, bool active) {
            Toast::show(this, active
                ? QStringLiteral("Plantilla activada")
                : QStringLiteral("Plantilla desactivada"), Toast::Type::Success);
            m_whatsAppService->fetchTemplates();
        });
    connect(m_whatsAppService, &WhatsAppService::failed, this, [this](const ApiError &error) {
        const QString message = error.detail.isEmpty()
            ? QStringLiteral("No se pudo completar la operación de plantillas")
            : error.detail;
        Toast::show(this, message, Toast::Type::Error);
    });
}

void SettingsView::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(28, 22, 28, 20);
    root->setSpacing(16);

    auto *header = new QWidget(this);
    auto *headerLayout = new QVBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(4);

    auto *title = new QLabel(QStringLiteral("Ajustes"), header);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 22px; font-weight: 700; color: #111827; background: transparent; }"));
    auto *subtitle = makeHint(
        QStringLiteral("Configurá los criterios que usa el sistema para gestionar donantes y donaciones."),
        header);
    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);
    root->addWidget(header);

    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(QStringLiteral(
        "QScrollArea { background: transparent; }"
        "QScrollArea > QWidget > QWidget { background: transparent; }"));

    auto *content = new QWidget(scroll);
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(0, 2, 8, 8);
    contentLayout->setSpacing(16);

    auto *cycleCard = makeCard(content);
    auto *cycleLayout = new QVBoxLayout(cycleCard);
    cycleLayout->setContentsMargins(20, 18, 20, 18);
    cycleLayout->setSpacing(14);

    auto *cycleHeader = new QVBoxLayout();
    cycleHeader->setSpacing(4);
    cycleHeader->addWidget(makeSectionTitle(QStringLiteral("Reglas de ciclos de donación"), cycleCard));
    cycleHeader->addWidget(makeHint(
        QStringLiteral("Administrá los intervalos y límites que se aplican al registrar una donación."),
        cycleCard));
    cycleLayout->addLayout(cycleHeader);

    m_rulesBox = new QWidget(cycleCard);
    m_rulesLayout = new QVBoxLayout(m_rulesBox);
    m_rulesLayout->setContentsMargins(0, 0, 0, 0);
    m_rulesLayout->setSpacing(10);
    cycleLayout->addWidget(m_rulesBox);

    auto *whatsAppCard = makeCard(content);
    auto *whatsAppLayout = new QVBoxLayout(whatsAppCard);
    whatsAppLayout->setContentsMargins(20, 18, 20, 18);
    whatsAppLayout->setSpacing(14);

    auto *whatsAppHeader = new QVBoxLayout();
    whatsAppHeader->setSpacing(4);
    whatsAppHeader->addWidget(makeSectionTitle(QStringLiteral("Mensajes de WhatsApp"), whatsAppCard));
    whatsAppHeader->addWidget(makeHint(
        QStringLiteral("Editá los textos que se envían automáticamente a los donantes."),
        whatsAppCard));
    whatsAppLayout->addLayout(whatsAppHeader);

    m_templatesBox = new QWidget(whatsAppCard);
    m_templatesLayout = new QVBoxLayout(m_templatesBox);
    m_templatesLayout->setContentsMargins(0, 0, 0, 0);
    m_templatesLayout->setSpacing(10);
    whatsAppLayout->addWidget(m_templatesBox);

    contentLayout->addWidget(cycleCard);
    contentLayout->addWidget(whatsAppCard);
    contentLayout->addStretch(1);

    scroll->setWidget(content);
    root->addWidget(scroll, 1);
}

void SettingsView::buildRuleRow(QVBoxLayout *rulesLayout, const CycleRuleItem &rule)
{
    auto *card = new QFrame(m_rulesBox);
    card->setObjectName(QStringLiteral("cycleRuleCard"));
    card->setStyleSheet(QStringLiteral(
        "QFrame#cycleRuleCard { background: #F9FAFB; border: 1px solid #E5E7EB; border-radius: 8px; }"));
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(12);

    const QString sexLabel = rule.sex.isEmpty()
        ? QStringLiteral("Todos")
        : (rule.sex == QLatin1String("M") ? QStringLiteral("Varones") : QStringLiteral("Mujeres"));
    auto *heading = new QHBoxLayout();
    auto *ruleTitle = new QLabel(QStringLiteral("%1 · %2").arg(rule.typeDonationName, sexLabel), card);
    ruleTitle->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 14px; font-weight: 700; color: #1F2937; background: transparent; }"));
    heading->addWidget(ruleTitle);
    heading->addStretch(1);

    auto *active = new QCheckBox(QStringLiteral("Activa"), card);
    active->setChecked(rule.isActive);
    heading->addWidget(active);
    layout->addLayout(heading);

    auto *fields = new QGridLayout();
    fields->setContentsMargins(0, 0, 0, 0);
    fields->setHorizontalSpacing(14);
    fields->setVerticalSpacing(6);

    auto makeSpin = [card](int minimum, int maximum, int value, const QString &suffix = QString()) {
        auto *spin = new QSpinBox(card);
        spin->setRange(minimum, maximum);
        spin->setValue(value);
        spin->setSuffix(suffix);
        spin->setMinimumWidth(120);
        return spin;
    };

    auto *maxDonations = makeSpin(1, 100, rule.maxDonations);
    auto *waitDays = makeSpin(0, 365, rule.waitDays, QStringLiteral(" días"));
    auto *waitBetween = makeSpin(0, 365, rule.waitBetweenCyclesDays, QStringLiteral(" días"));

    fields->addWidget(makeFieldLabel(QStringLiteral("Máx. donaciones por serie"), card), 0, 0);
    fields->addWidget(makeFieldLabel(QStringLiteral("Espera entre donaciones"), card), 0, 1);
    fields->addWidget(makeFieldLabel(QStringLiteral("Espera entre series"), card), 0, 2);
    fields->addWidget(maxDonations, 1, 0);
    fields->addWidget(waitDays, 1, 1);
    fields->addWidget(waitBetween, 1, 2);
    fields->setColumnStretch(0, 1);
    fields->setColumnStretch(1, 1);
    fields->setColumnStretch(2, 1);
    layout->addLayout(fields);

    auto *saveBtn = new QPushButton(QStringLiteral("Guardar regla"), card);
    saveBtn->setProperty("variant", "primary");
    connect(saveBtn, &QPushButton::clicked, this, [this, id = rule.id, maxDonations, waitDays, waitBetween, active]() {
        m_ruleService->update(
            id,
            maxDonations->value(),
            waitDays->value(),
            waitBetween->value(),
            active->isChecked());
    });
    layout->addWidget(saveBtn, 0, Qt::AlignRight);
    rulesLayout->addWidget(card);
}

void SettingsView::buildTemplateRow(QVBoxLayout *templatesLayout, const WSPTemplateItem &item)
{
    auto *card = new QFrame(m_templatesBox);
    card->setObjectName(QStringLiteral("cycleRuleCard"));
    card->setStyleSheet(QStringLiteral(
        "QFrame#cycleRuleCard { background: #F9FAFB; border: 1px solid #E5E7EB; border-radius: 8px; }"));
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(8);

    auto *heading = new QHBoxLayout();
    auto *name = new QLabel(item.name, card);
    name->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 14px; font-weight: 700; color: #1F2937; background: transparent; }"));
    heading->addWidget(name);
    heading->addStretch(1);

    auto *status = new QLabel(item.isActive ? QStringLiteral("Activa") : QStringLiteral("Inactiva"), card);
    status->setStyleSheet(item.isActive
        ? QStringLiteral("QLabel { color: #166534; background: #DCFCE7; border-radius: 10px; padding: 3px 9px; font-size: 11px; font-weight: 600; }")
        : QStringLiteral("QLabel { color: #6B7280; background: #E5E7EB; border-radius: 10px; padding: 3px 9px; font-size: 11px; font-weight: 600; }"));
    heading->addWidget(status);
    layout->addLayout(heading);

    auto *sample = makeHint(templatePreview(item.text), card);
    sample->setMaximumHeight(42);
    layout->addWidget(sample);

    auto *actions = new QHBoxLayout();
    actions->addStretch(1);
    auto *edit = new QPushButton(QStringLiteral("Editar"), card);
    auto *toggle = new QPushButton(item.isActive ? QStringLiteral("Desactivar") : QStringLiteral("Activar"), card);
    actions->addWidget(edit);
    actions->addWidget(toggle);
    layout->addLayout(actions);
    templatesLayout->addWidget(card);

    connect(edit, &QPushButton::clicked, this, [this, item]() {
        QDialog dialog(this);
        dialog.setWindowTitle(QStringLiteral("Editar plantilla de WhatsApp"));
        dialog.setMinimumSize(620, 590);

        auto *dialogLayout = new QVBoxLayout(&dialog);
        dialogLayout->setContentsMargins(24, 22, 24, 20);
        dialogLayout->setSpacing(12);

        auto *dialogTitle = new QLabel(item.name, &dialog);
        dialogTitle->setStyleSheet(QStringLiteral(
            "QLabel { font-size: 18px; font-weight: 700; color: #111827; }"));
        dialogLayout->addWidget(dialogTitle);

        auto *nameLabel = makeFieldLabel(QStringLiteral("Nombre visible"), &dialog);
        auto *nameEdit = new QLineEdit(item.name, &dialog);
        nameEdit->setMaxLength(100);
        dialogLayout->addWidget(nameLabel);
        dialogLayout->addWidget(nameEdit);

        auto *textLabel = makeFieldLabel(QStringLiteral("Texto del mensaje"), &dialog);
        auto *textEdit = new QPlainTextEdit(&dialog);
        textEdit->setPlainText(item.text);
        textEdit->setPlaceholderText(QStringLiteral("Escribí el mensaje que recibirá el donante…"));
        textEdit->setMinimumHeight(150);
        dialogLayout->addWidget(textLabel);
        dialogLayout->addWidget(textEdit);

        auto *variablesHint = makeHint(
            QStringLiteral("Variables: {{nombre}}, {{apellido}}, {{nombre_completo}}, {{telefono}}, {{fecha}} y {{hora}}"),
            &dialog);
        dialogLayout->addWidget(variablesHint);

        auto *previewTitle = new QLabel(QStringLiteral("Vista previa"), &dialog);
        previewTitle->setStyleSheet(QStringLiteral("QLabel { font-weight: 600; color: #374151; }"));
        auto *preview = new QLabel(&dialog);
        preview->setWordWrap(true);
        preview->setMinimumHeight(58);
        preview->setStyleSheet(QStringLiteral(
            "QLabel { color: #1F2937; background: #F9FAFB; border: 1px solid #E5E7EB; border-radius: 8px; padding: 10px 12px; }"));
        auto *validation = makeHint(QString(), &dialog);
        validation->setProperty("role", "error");
        dialogLayout->addWidget(previewTitle);
        dialogLayout->addWidget(preview);
        dialogLayout->addWidget(validation);

        auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
        buttons->button(QDialogButtonBox::Save)->setText(QStringLiteral("Guardar cambios"));
        buttons->button(QDialogButtonBox::Save)->setProperty("variant", "primary");
        dialogLayout->addWidget(buttons);

        const QSet<QString> supported = {
            QStringLiteral("nombre"), QStringLiteral("apellido"), QStringLiteral("nombre_completo"),
            QStringLiteral("telefono"), QStringLiteral("fecha"), QStringLiteral("hora")};
        const QRegularExpression tokenPattern(QStringLiteral("\\{\\{([^{}]+)\\}\\}"));
        const auto updatePreview = [=]() {
            const QString text = textEdit->toPlainText();
            preview->setText(templatePreview(text));
            QSet<QString> unknown;
            auto matches = tokenPattern.globalMatch(text);
            while (matches.hasNext()) {
                const QString token = matches.next().captured(1).trimmed();
                if (!supported.contains(token)) {
                    unknown.insert(token);
                }
            }
            const bool missingRequired = nameEdit->text().trimmed().isEmpty() || text.trimmed().isEmpty();
            if (!unknown.isEmpty()) {
                QStringList unknownList = unknown.values();
                unknownList.sort();
                validation->setText(QStringLiteral("Variables no reconocidas: %1").arg(unknownList.join(QStringLiteral(", "))));
            } else if (missingRequired) {
                validation->setText(QStringLiteral("Completá el nombre y el texto del mensaje."));
            } else {
                validation->clear();
            }
            buttons->button(QDialogButtonBox::Save)->setEnabled(!missingRequired && unknown.isEmpty());
        };
        connect(textEdit, &QPlainTextEdit::textChanged, &dialog, updatePreview);
        connect(nameEdit, &QLineEdit::textChanged, &dialog, updatePreview);
        updatePreview();

        connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
        connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        if (dialog.exec() == QDialog::Accepted) {
            m_whatsAppService->updateTemplate(
                item.id, item.motive, nameEdit->text().trimmed(), textEdit->toPlainText().trimmed());
        }
    });

    connect(toggle, &QPushButton::clicked, this, [this, item]() {
        const bool nextActive = !item.isActive;
        const QString action = nextActive ? QStringLiteral("activar") : QStringLiteral("desactivar");
        const auto answer = QMessageBox::question(
            this,
            QStringLiteral("Cambiar estado de plantilla"),
            QStringLiteral("¿Querés %1 «%2»?").arg(action, item.name),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No);
        if (answer == QMessageBox::Yes) {
            m_whatsAppService->setTemplateActive(item.id, nextActive);
        }
    });
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

void SettingsView::clearTemplateRows()
{
    while (QLayoutItem *item = m_templatesLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

void SettingsView::refresh()
{
    m_ruleService->fetchAll();
    m_whatsAppService->fetchTemplates();
}
