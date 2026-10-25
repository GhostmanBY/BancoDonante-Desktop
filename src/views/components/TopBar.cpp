#include "views/components/TopBar.hpp"
#include "services/WhatsAppService.hpp"
#include <QDialog>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <qrcodegen.hpp>

#include <exception>

namespace {

QString roleLabel(UserRole role)
{
    switch (role) {
    case UserRole::ADMIN: return QStringLiteral("Administrador");
    case UserRole::AREA_MANAGER: return QStringLiteral("Jefe de área");
    case UserRole::MEDICAL: return QStringLiteral("Personal médico");
    case UserRole::NURSE: return QStringLiteral("Enfermería");
    }
    return {};
}

} // namespace

TopBar::TopBar(QWidget *parent)
    : QFrame(parent)
{
    setFixedHeight(56);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(24, 0, 24, 0);
    layout->setSpacing(12);

    layout->addStretch();

    auto *userBox = new QVBoxLayout();
    userBox->setSpacing(0);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setProperty("role", "user-name");

    m_roleLabel = new QLabel(this);
    m_roleLabel->setProperty("role", "user-role");

    userBox->addWidget(m_nameLabel);
    userBox->addWidget(m_roleLabel);

    m_qrButton = new QPushButton(QStringLiteral("Vincular WhatsApp · QR"), this);
    m_qrButton->setCursor(Qt::PointingHandCursor);
    m_qrButton->setVisible(false);
    m_whatsAppService = new WhatsAppService(this);

    m_logoutButton = new QPushButton(QStringLiteral("Cerrar sesión"), this);
    m_logoutButton->setCursor(Qt::PointingHandCursor);

    connect(m_logoutButton, &QPushButton::clicked, this, &TopBar::logoutRequested);
    connect(m_qrButton, &QPushButton::clicked, this, [this]() {
        auto *dialog = new QDialog(this);
        dialog->setWindowTitle(QStringLiteral("Vincular WhatsApp"));
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->setModal(true);
        auto *dialogLayout = new QVBoxLayout(dialog);
        auto *stateLabel = new QLabel(QStringLiteral("Consultando conexión…"), dialog);
        stateLabel->setAlignment(Qt::AlignCenter);
        auto *qrLabel = new QLabel(dialog);
        qrLabel->setAlignment(Qt::AlignCenter);
        auto *hintLabel = new QLabel(QStringLiteral("Escaneá este código desde WhatsApp en el teléfono vinculado."), dialog);
        hintLabel->setWordWrap(true);
        hintLabel->setAlignment(Qt::AlignCenter);
        dialogLayout->addWidget(stateLabel);
        dialogLayout->addWidget(qrLabel, 0, Qt::AlignCenter);
        dialogLayout->addWidget(hintLabel);
        dialog->resize(360, 420);

        auto *timer = new QTimer(dialog);
        timer->setInterval(2000);
        connect(timer, &QTimer::timeout, m_whatsAppService, &WhatsAppService::fetchConnection);
        connect(m_whatsAppService, &WhatsAppService::connectionFetched, dialog,
            [stateLabel, qrLabel](const QString &state, const QString &qr) {
                QString message;
                if (state == QLatin1String("connected")) message = QStringLiteral("WhatsApp está conectado.");
                else if (state == QLatin1String("waiting_qr") || state == QLatin1String("connecting")) message = QStringLiteral("Esperando que escanees el código…");
                else if (state == QLatin1String("offline")) message = QStringLiteral("El worker de WhatsApp está desconectado.");
                else message = QStringLiteral("No se pudo obtener el estado de WhatsApp.");
                stateLabel->setText(message);
                if (!qr.isEmpty() && state != QLatin1String("connected")) {
                    try {
                        const QByteArray utf8 = qr.toUtf8();
                        const qrcodegen::QrCode code = qrcodegen::QrCode::encodeText(
                            utf8.constData(), qrcodegen::QrCode::Ecc::MEDIUM);
                        constexpr int scale = 5;
                        constexpr int quiet = 4;
                        const int side = (code.getSize() + quiet * 2) * scale;
                        QImage image(side, side, QImage::Format_RGB32);
                        image.fill(Qt::white);
                        for (int y = 0; y < code.getSize(); ++y) {
                            for (int x = 0; x < code.getSize(); ++x) {
                                if (code.getModule(x, y)) {
                                    for (int dy = 0; dy < scale; ++dy) {
                                        for (int dx = 0; dx < scale; ++dx) {
                                            image.setPixelColor((x + quiet) * scale + dx,
                                                (y + quiet) * scale + dy, Qt::black);
                                        }
                                    }
                                }
                            }
                        }
                        qrLabel->setPixmap(QPixmap::fromImage(image));
                        qrLabel->show();
                    } catch (const std::exception &) {
                        qrLabel->clear();
                        stateLabel->setText(QStringLiteral("No se pudo generar el código QR."));
                    }
                } else {
                    qrLabel->clear();
                }
            });
        connect(m_whatsAppService, &WhatsAppService::failed, dialog,
            [stateLabel](const ApiError &error) {
                stateLabel->setText(error.detail.isEmpty()
                    ? QStringLiteral("No se pudo consultar la conexión de WhatsApp.")
                    : error.detail);
            });
        timer->start();
        m_whatsAppService->fetchConnection();
        dialog->open();
    });

    layout->addLayout(userBox);
    layout->addWidget(m_qrButton);
    layout->addWidget(m_logoutButton);
}

void TopBar::setUser(const SessionUser &user)
{
    m_nameLabel->setText(user.fullName());
    m_roleLabel->setText(roleLabel(user.role));
    const bool canLinkWhatsApp = user.role == UserRole::ADMIN || user.role == UserRole::AREA_MANAGER;
    m_qrButton->setVisible(canLinkWhatsApp);
}
