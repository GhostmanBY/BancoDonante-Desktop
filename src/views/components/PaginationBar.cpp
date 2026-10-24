#include "views/components/PaginationBar.hpp"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

PaginationBar::PaginationBar(QWidget *parent)
    : QFrame(parent)
{
    m_info = new QLabel(this);

    m_first = new QPushButton(QStringLiteral("«"), this);
    m_prev = new QPushButton(QStringLiteral("‹"), this);
    m_next = new QPushButton(QStringLiteral("›"), this);
    m_last = new QPushButton(QStringLiteral("»"), this);

    m_first->setFixedWidth(40);
    m_prev->setFixedWidth(40);
    m_next->setFixedWidth(40);
    m_last->setFixedWidth(40);

    m_sizeCombo = new QComboBox(this);
    m_sizeCombo->addItem(QStringLiteral("20 por página"), 20);
    m_sizeCombo->addItem(QStringLiteral("50 por página"), 50);
    m_sizeCombo->addItem(QStringLiteral("100 por página"), 100);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 8, 4, 4);
    layout->setSpacing(6);
    layout->addWidget(m_info);
    layout->addStretch();
    layout->addWidget(m_sizeCombo);
    layout->addWidget(m_first);
    layout->addWidget(m_prev);
    layout->addWidget(m_next);
    layout->addWidget(m_last);

    connect(m_first, &QPushButton::clicked, this, [this]() { emit pageChanged(1); });
    connect(m_prev, &QPushButton::clicked, this, [this]() { emit pageChanged(m_page - 1); });
    connect(m_next, &QPushButton::clicked, this, [this]() { emit pageChanged(m_page + 1); });
    connect(m_last, &QPushButton::clicked, this, [this]() { emit pageChanged(m_totalPages); });
    connect(m_sizeCombo, &QComboBox::currentIndexChanged, this, [this](int index) {
        emit sizeChanged(m_sizeCombo->itemData(index).toInt());
    });

    setPagination(1, 1, 0, QStringLiteral("Donantes"));
}

void PaginationBar::setPagination(int page, int totalPages, int total, QString entity)
{
    m_page = qMax(1, page);
    m_totalPages = qMax(1, totalPages);
    m_total = qMax(0, total);
    m_entity = entity;

    updateInfo();
    updateButtons();
}

void PaginationBar::setLoading(bool loading)
{
    m_first->setEnabled(!loading && m_page > 1);
    m_prev->setEnabled(!loading && m_page > 1);
    m_next->setEnabled(!loading && m_page < m_totalPages);
    m_last->setEnabled(!loading && m_page < m_totalPages);
    m_sizeCombo->setEnabled(!loading);
}

void PaginationBar::updateButtons()
{
    const bool hasPrev = m_page > 1;
    const bool hasNext = m_page < m_totalPages;

    m_first->setEnabled(hasPrev);
    m_prev->setEnabled(hasPrev);
    m_next->setEnabled(hasNext);
    m_last->setEnabled(hasNext);
}

void PaginationBar::updateInfo()
{
    if (m_total == 0) {
        m_info->setText(QStringLiteral("Sin resultados"));
        return;
    }

    m_info->setText(QStringLiteral("%1 %2 · Página %3 de %4")
                    .arg(m_total)
                    .arg(m_entity)
                    .arg(m_page)
                    .arg(m_totalPages));
}
