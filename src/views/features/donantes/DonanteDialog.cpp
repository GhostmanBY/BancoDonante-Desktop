#include "views/features/donantes/DonanteDialog.hpp"

#include "dtos/Patientes/CreatePatientesDTO.hpp"
#include "dtos/Patientes/UpdatePatientesDTO.hpp"
#include "models/enums/BloodGroup.hpp"
#include "models/enums/PatientState.hpp"
#include "models/enums/Sex.hpp"
#include "models/enums/UserRole.hpp"
#include "services/DonorService.hpp"
#include "services/SessionManager.hpp"
#include "views/components/AddressFields.hpp"
#include "views/components/Toast.hpp"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QStyle>
#include <QVBoxLayout>

namespace {

const BloodGroup kBloodGroups[] = {
    BloodGroup::A_POS, BloodGroup::A_NEG,
    BloodGroup::B_POS, BloodGroup::B_NEG,
    BloodGroup::AB_POS, BloodGroup::AB_NEG,
    BloodGroup::O_POS, BloodGroup::O_NEG,
};

const PatientState kStates[] = {
    PatientState::APTO,
    PatientState::REVISON,
    PatientState::NO_APTO,
    PatientState::SUSPENDED,
};

QString stateLabel(PatientState state)
{
    switch (state) {
    case PatientState::APTO: return QStringLiteral("Apto");
    case PatientState::REVISON: return QStringLiteral("En revisión");
    case PatientState::NO_APTO: return QStringLiteral("No apto");
    case PatientState::SUSPENDED: return QStringLiteral("Suspendido");
    }
    return {};
}

} // namespace

DonanteDialog::DonanteDialog(
    const std::optional<Patientes> &donante,
    QWidget *parent)
    : QDialog(parent),
      m_service(new DonorService(this)),
      m_donante(donante)
{
    setWindowTitle(m_donante.has_value()
        ? QStringLiteral("Editar donante")
        : QStringLiteral("Nuevo donante"));
    setModal(true);
    resize(680, 700);
    setMinimumSize(600, 560);

    buildUi();
    prefill();

    connect(m_service, &DonorService::createSucceeded, this, [this](const Patientes &donante) {
        m_createdId = donante.id();
        if (wantsAddress()) {
            const auto dto = m_addressFields->toDto(m_createdId);
            if (dto.has_value()) {
                m_service->createAddress(*dto);
                return;
            }
        }
        finishSave(QStringLiteral("Donante creado correctamente"));
    });
    connect(m_service, &DonorService::updateSucceeded, this, [this]() {
        if (wantsAddress()) {
            const auto dto = m_addressFields->toDto(m_donante->id());
            if (dto.has_value()) {
                m_service->createAddress(*dto);
                return;
            }
        }
        finishSave(QStringLiteral("Donante actualizado correctamente"));
    });
    connect(m_service, &DonorService::addressCreated, this, [this]() {
        finishSave(QStringLiteral("Donante y dirección guardados correctamente"));
    });
    connect(m_service, &DonorService::failed, this, &DonanteDialog::onFailed);
}

void DonanteDialog::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 16);
    layout->setSpacing(14);

    auto *title = new QLabel(
        m_donante.has_value()
            ? QStringLiteral("Editar datos del donante")
            : QStringLiteral("Cargar un nuevo donante"),
        this
    );
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 16px; font-weight: 700; color: #111827; }"));

    auto *content = new QWidget();
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 4, 0);
    contentLayout->setSpacing(14);
    contentLayout->addWidget(title);

    auto *form = new QGridLayout();
    form->setSpacing(12);

    m_name = new QLineEdit(this);
    m_name->setPlaceholderText(QStringLiteral("Nombre"));
    m_lastname = new QLineEdit(this);
    m_lastname->setPlaceholderText(QStringLiteral("Apellido"));
    m_mail = new QLineEdit(this);
    m_mail->setPlaceholderText(QStringLiteral("nombre@mail.com"));
    m_phone = new QLineEdit(this);
    m_phone->setPlaceholderText(QStringLiteral("Teléfono"));

    m_documentType = new QComboBox(this);
    m_documentType->addItems({QStringLiteral("DNI"), QStringLiteral("LC"), QStringLiteral("LE"), QStringLiteral("Pasaporte")});
    m_documentNumber = new QLineEdit(this);
    m_documentNumber->setPlaceholderText(QStringLiteral("Número"));

    m_birthDate = new QDateEdit(this);
    m_birthDate->setCalendarPopup(true);
    m_birthDate->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    m_birthDate->setDate(QDate::currentDate().addYears(-25));

    m_birthPlace = new QLineEdit(this);
    m_birthPlace->setPlaceholderText(QStringLiteral("Lugar de nacimiento (opcional)"));

    m_sex = new QComboBox(this);
    m_sex->addItem(QStringLiteral("Masculino"), static_cast<int>(Sex::M));
    m_sex->addItem(QStringLiteral("Femenino"), static_cast<int>(Sex::F));

    m_peso = new QDoubleSpinBox(this);
    m_peso->setRange(30.0, 300.0);
    m_peso->setDecimals(1);
    m_peso->setSuffix(QStringLiteral(" kg"));
    m_peso->setValue(70.0);

    m_altura = new QDoubleSpinBox(this);
    m_altura->setRange(100.0, 230.0);
    m_altura->setDecimals(0);
    m_altura->setSuffix(QStringLiteral(" cm"));
    m_altura->setValue(170.0);

    m_bloodGroup = new QComboBox(this);
    for (const BloodGroup group : kBloodGroups) {
        m_bloodGroup->addItem(::toString(group), static_cast<int>(group));
    }

    auto addRow = [&](int row, int col, const QString &label, QWidget *field) {
        auto *box = new QVBoxLayout();
        box->setSpacing(4);
        auto *caption = new QLabel(label, this);
        caption->setProperty("role", "hint");
        box->addWidget(caption);
        box->addWidget(field);
        form->addLayout(box, row, col);
    };

    addRow(0, 0, QStringLiteral("Nombre *"), m_name);
    addRow(0, 1, QStringLiteral("Apellido *"), m_lastname);
    addRow(1, 0, QStringLiteral("Mail *"), m_mail);
    addRow(1, 1, QStringLiteral("Teléfono *"), m_phone);
    addRow(2, 0, QStringLiteral("Tipo de documento"), m_documentType);
    addRow(2, 1, QStringLiteral("Nro. de documento *"), m_documentNumber);
    addRow(3, 0, QStringLiteral("Fecha de nacimiento *"), m_birthDate);
    addRow(3, 1, QStringLiteral("Lugar de nacimiento"), m_birthPlace);

    // El estado arranca siempre como "apto"; solo se ajusta al editar (evaluación médica).
    if (m_donante.has_value()) {
        m_state = new QComboBox(this);
        for (const PatientState state : kStates) {
            m_state->addItem(stateLabel(state), static_cast<int>(state));
        }
        
        addRow(4, 0, QStringLiteral("Estado de donación"), m_state);
        addRow(4, 1, QStringLiteral("Sexo"), m_sex);
        addRow(5, 0, QStringLiteral("Peso"), m_peso);
        addRow(5, 1, QStringLiteral("Altura"), m_altura);
        addRow(6, 0, QStringLiteral("Grupo sanguíneo"), m_bloodGroup);
    } else {
        addRow(4, 0, QStringLiteral("Sexo"), m_sex);
        addRow(4, 1, QStringLiteral("Peso"), m_peso);
        addRow(5, 0, QStringLiteral("Altura"), m_altura);
        addRow(5, 1, QStringLiteral("Grupo sanguíneo"), m_bloodGroup);
    }

    const UserRole role = SessionManager::instance().user().role;
    const bool canManageAddresses =
        role == UserRole::ADMIN || role == UserRole::MEDICAL || role == UserRole::NURSE;
    if (canManageAddresses) {
        m_addressFields = new AddressFields(content);
    }

    m_formError = new QLabel(content);
    m_formError->setProperty("role", "error");
    m_formError->setWordWrap(true);
    m_formError->setVisible(false);

    auto *buttons = new QDialogButtonBox(this);
    m_acceptBtn = buttons->addButton(
        m_donante.has_value() ? QStringLiteral("Guardar cambios") : QStringLiteral("Crear donante"),
        QDialogButtonBox::AcceptRole);
    m_acceptBtn->setProperty("variant", "primary");
    buttons->addButton(QStringLiteral("Cancelar"), QDialogButtonBox::RejectRole);

    contentLayout->addLayout(form);
    if (m_addressFields) {
        contentLayout->addWidget(m_addressFields);
    }
    contentLayout->addWidget(m_formError);
    contentLayout->addStretch();

    auto *scroll = new QScrollArea(this);
    scroll->setWidget(content);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setMinimumHeight(480);
    scroll->setMaximumHeight(700);

    layout->addWidget(scroll, 1);
    layout->addWidget(buttons, 0, Qt::AlignRight);

    connect(buttons, &QDialogButtonBox::accepted, this, &DonanteDialog::submit);
    connect(buttons, &QDialogButtonBox::rejected, this, &DonanteDialog::reject);
}

void DonanteDialog::prefill()
{
    if (!m_donante.has_value()) {
        return;
    }

    const Patientes &d = *m_donante;
    m_name->setText(d.name());
    m_lastname->setText(d.lastname());
    m_mail->setText(d.mail());
    m_phone->setText(d.phone());
    m_documentType->setCurrentText(d.documentType());
    m_documentNumber->setText(d.documentNumber());

    if (d.birthDate().has_value() && d.birthDate()->isValid()) {
        m_birthDate->setDate(d.birthDate()->date());
    }
    if (d.birthPlace().has_value()) {
        m_birthPlace->setText(*d.birthPlace());
    }

    m_state->setCurrentIndex(static_cast<int>(d.stateDonation()));
    m_sex->setCurrentIndex(static_cast<int>(d.sex()));
    if (d.peso().has_value()) {
        m_peso->setValue(*d.peso());
    }
    if (d.altura().has_value()) {
        m_altura->setValue(*d.altura());
    }
    if (d.bloodGroup().has_value()) {
        m_bloodGroup->setCurrentIndex(static_cast<int>(*d.bloodGroup()));
    }
}

QString DonanteDialog::birthDateValue() const
{
    return m_birthDate->date().toString(QStringLiteral("yyyy-MM-dd"));
}

bool DonanteDialog::wantsAddress() const
{
    return m_addressFields && m_addressFields->isChecked();
}

void DonanteDialog::finishSave(const QString &message)
{
    Toast::show(this, message, Toast::Type::Success);
    emit saved();
    accept();
}

bool DonanteDialog::validate()
{
    bool ok = true;

    const auto check = [&ok](QLineEdit *field, bool valid) {
        field->setProperty("error", valid ? "false" : "true");
        field->style()->unpolish(field);
        field->style()->polish(field);
        ok = ok && valid;
    };

    check(m_name, !m_name->text().trimmed().isEmpty());
    check(m_lastname, !m_lastname->text().trimmed().isEmpty());
    check(m_mail, m_mail->text().contains(QLatin1Char('@')));
    check(m_phone, m_phone->text().trimmed().size() >= 6);
    check(m_documentNumber, !m_documentNumber->text().trimmed().isEmpty());

    if (!ok) {
        m_formError->setText(QStringLiteral("Revisá los campos marcados: nombre, apellido, mail válido, teléfono (mín. 6 caracteres) y documento."));
        m_formError->setVisible(true);
    } else {
        m_formError->setVisible(false);
    }

    if (ok && m_addressFields && !m_addressFields->validate()) {
        ok = false;
        m_formError->setText(QStringLiteral("Si querés cargar una dirección, la calle es obligatoria."));
        m_formError->setVisible(true);
    }

    return ok;
}

void DonanteDialog::submit()
{
    if (!validate()) {
        return;
    }

    m_acceptBtn->setEnabled(false);

    const QString name = m_name->text().trimmed();
    const QString lastname = m_lastname->text().trimmed();
    const QString mail = m_mail->text().trimmed();
    const QString phone = m_phone->text().trimmed();
    const QString documentType = m_documentType->currentText();
    const QString documentNumber = m_documentNumber->text().trimmed();
    const std::optional<QString> birthPlace =
        m_birthPlace->text().trimmed().isEmpty()
            ? std::nullopt
            : std::optional(m_birthPlace->text().trimmed());
    const Sex sex = static_cast<Sex>(m_sex->currentData().toInt());
    const BloodGroup group = static_cast<BloodGroup>(m_bloodGroup->currentData().toInt());

    if (m_donante.has_value()) {
        const PatientState state =
            static_cast<PatientState>(m_state->currentData().toInt());
        m_service->update(UpdatePatientesDTO(
            m_donante->id(),
            name, lastname, mail, phone, documentType, documentNumber,
            birthDateValue(), birthPlace,
            state, sex, m_peso->value(), m_altura->value(), group));
    } else {
        m_service->create(CreatePatientesDTO(
            name, lastname, mail, phone, documentType, documentNumber,
            birthDateValue(), birthPlace,
            sex, m_peso->value(), m_altura->value(), group));
    }
}

void DonanteDialog::onFailed(const ApiError &error)
{
    m_acceptBtn->setEnabled(true);

    QString message = error.detail.isEmpty()
        ? QStringLiteral("No se pudo guardar el donante.")
        : error.detail;

    if (!error.field.isEmpty()) {
        message = QStringLiteral("%1 (%2)").arg(message, error.field);
    }

    m_formError->setText(message);
    m_formError->setVisible(true);
    Toast::show(this, message, Toast::Type::Error);
}
