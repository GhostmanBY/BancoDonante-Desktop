#include "views/components/Sidebar.hpp"

#include <QButtonGroup>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
auto *kButtonGroupProperty = "navButton";
}

Sidebar::Sidebar(QWidget *parent)
    : QFrame(parent)
{
    setFixedWidth(232);
    buildUi();
}

void Sidebar::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 20, 16, 20);
    layout->setSpacing(4);

    auto *brand = new QLabel(QStringLiteral("Banco de Sangre"), this);
    brand->setProperty("role", "brand");

    auto *brandSub = new QLabel(QStringLiteral("Sistema de gestión de donantes"), this);
    brandSub->setProperty("role", "brand-sub");
    brandSub->setWordWrap(true);

    layout->addWidget(brand);
    layout->addWidget(brandSub);
    layout->addSpacing(20);

    m_navLayout = new QVBoxLayout();
    m_navLayout->setSpacing(4);
    layout->addLayout(m_navLayout);
    layout->addStretch();
}

void Sidebar::addItem(
    const QString &id,
    const QString &label,
    const QString &glyph
)
{
    addItem(id, label, glyph, UserRole::NURSE);
}

void Sidebar::addItem(
    const QString &id,
    const QString &label,
    const QString &glyph,
    UserRole requiredRole
)
{
    Item item;
    item.id = id;
    item.label = label;
    item.glyph = glyph;
    item.requiredRole = requiredRole;
    m_items.append(item);
}

void Sidebar::addSeparator()
{
    Item item;
    item.isSeparator = true;
    m_items.append(item);
}

bool Sidebar::isItemAllowed(const Item &item, UserRole role) const
{
    switch (item.requiredRole) {
    case UserRole::ADMIN:
        return role == UserRole::ADMIN;
    case UserRole::AREA_MANAGER:
        return role == UserRole::ADMIN || role == UserRole::AREA_MANAGER;
    case UserRole::MEDICAL:
        return role == UserRole::ADMIN
            || role == UserRole::AREA_MANAGER
            || role == UserRole::MEDICAL;
    case UserRole::NURSE:
        return true;
    }
    return true;
}

QPushButton *Sidebar::createButton(const Item &item)
{
    auto *button = new QPushButton(
        QStringLiteral("%1  %2").arg(item.glyph, item.label),
        this
    );
    button->setCheckable(true);
    button->setProperty(kButtonGroupProperty, item.id);
    button->setCursor(Qt::PointingHandCursor);
    button->setToolTip(item.label);

    connect(button, &QPushButton::clicked, this, [this, id = item.id]() {
        emit itemClicked(id);
    });

    return button;
}

void Sidebar::rebuildForRole(UserRole role)
{
    m_currentRole = role;

    QLayoutItem *child = nullptr;
    while ((child = m_navLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }
    m_buttons.clear();

    auto *group = new QButtonGroup(this);
    group->setExclusive(true);

    for (int i = 0; i < m_items.size(); ++i) {
        const Item &item = m_items.at(i);
        if (item.isSeparator) {
            bool hasVisibleAfter = false;
            for (int j = i + 1; j < m_items.size() && !m_items.at(j).isSeparator; ++j) {
                if (isItemAllowed(m_items.at(j), role)) {
                    hasVisibleAfter = true;
                    break;
                }
            }
            if (!hasVisibleAfter) {
                continue;
            }

            auto *line = new QFrame(this);
            line->setFrameShape(QFrame::HLine);
            line->setFixedHeight(1);
            line->setStyleSheet(QStringLiteral(
                "QFrame { background: #1F2937; border: none; }"
            ));
            m_navLayout->addWidget(line);
            m_navLayout->addSpacing(8);
            continue;
        }

        if (!isItemAllowed(item, role)) {
            continue;
        }

        QPushButton *button = createButton(item);
        group->addButton(button);
        m_buttons.append(button);
        m_navLayout->addWidget(button);
    }

    m_navLayout->addStretch();

    if (!m_buttons.isEmpty()) {
        m_buttons.first()->setChecked(true);
    }
}

void Sidebar::setActiveItem(const QString &id)
{
    for (QPushButton *button : m_buttons) {
        if (button->property(kButtonGroupProperty).toString() == id) {
            button->setChecked(true);
            return;
        }
    }
}
