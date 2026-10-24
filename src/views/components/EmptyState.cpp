#include "views/components/EmptyState.hpp"

#include <QLabel>
#include <QVBoxLayout>

EmptyState::EmptyState(
    const QString &title,
    const QString &subtitle,
    const QString &glyph,
    QWidget *parent
)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(8);

    m_glyphLabel = new QLabel(glyph, this);
    m_glyphLabel->setObjectName(QStringLiteral("emptyGlyph"));
    m_glyphLabel->setProperty("role", "empty-glyph");
    m_glyphLabel->setAlignment(Qt::AlignCenter);

    m_titleLabel = new QLabel(title, this);
    m_titleLabel->setObjectName(QStringLiteral("emptyTitle"));
    m_titleLabel->setProperty("role", "empty-title");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setWordWrap(true);

    layout->addWidget(m_glyphLabel);
    layout->addWidget(m_titleLabel);

    if (!subtitle.isEmpty()) {
        m_subtitleLabel = new QLabel(subtitle, this);
        m_subtitleLabel->setObjectName(QStringLiteral("emptySubtitle"));
        m_subtitleLabel->setProperty("role", "empty-subtitle");
        m_subtitleLabel->setAlignment(Qt::AlignCenter);
        m_subtitleLabel->setWordWrap(true);
        layout->addWidget(m_subtitleLabel);
    }
}
