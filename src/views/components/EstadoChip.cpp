#include "views/components/EstadoChip.hpp"

#include <QHBoxLayout>
#include <QLabel>

namespace {

struct ChipStyle
{
    QString text;
    QString background;
    QString foreground;
    QString border;
};

ChipStyle styleFor(PatientState state)
{
    switch (state) {
    case PatientState::APTO:
        return {
            QStringLiteral("Apto"),
            QStringLiteral("#D1FAE5"),
            QStringLiteral("#065F46"),
            QStringLiteral("#6EE7B7"),
        };
    case PatientState::REVISON:
        return {
            QStringLiteral("En revisión"),
            QStringLiteral("#FEF3C7"),
            QStringLiteral("#92400E"),
            QStringLiteral("#FCD34D"),
        };
    case PatientState::NO_APTO:
        return {
            QStringLiteral("No apto"),
            QStringLiteral("#FEE2E2"),
            QStringLiteral("#991B1B"),
            QStringLiteral("#FCA5A5"),
        };
    case PatientState::SUSPENDED:
        return {
            QStringLiteral("Suspendido"),
            QStringLiteral("#F3F4F6"),
            QStringLiteral("#4B5563"),
            QStringLiteral("#D1D5DB"),
        };
    }
    return {};
}

} // namespace

EstadoChip::EstadoChip(QWidget *parent)
    : QFrame(parent)
{
    m_label = new QLabel(this);
    m_label->setAlignment(Qt::AlignCenter);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 3, 10, 3);
    layout->addWidget(m_label);
}

void EstadoChip::setState(PatientState state)
{
    const ChipStyle style = styleFor(state);

    m_label->setText(style.text);
    m_label->setStyleSheet(QStringLiteral(
        "QLabel { color: %1; background: transparent; font-weight: 600; font-size: 11px; }"
    ).arg(style.foreground));

    setStyleSheet(QStringLiteral(
        "QFrame { background: %1; border: 1px solid %2; border-radius: 10px; }"
    ).arg(style.background, style.border));
}

QSize EstadoChip::sizeHint() const
{
    return m_label->sizeHint() + QSize(20, 6);
}
