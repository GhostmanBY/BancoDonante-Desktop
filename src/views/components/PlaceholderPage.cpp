#include "views/components/PlaceholderPage.hpp"

#include "views/components/EmptyState.hpp"

#include <QVBoxLayout>

PlaceholderPage::PlaceholderPage(
    const QString &title,
    const QString &subtitle,
    QWidget *parent
)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new EmptyState(title, subtitle, QStringLiteral("♦"), this));
}
