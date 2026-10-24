#include "views/components/Spinner.hpp"

#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

Spinner::Spinner(QWidget *parent)
    : QWidget(parent)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(16);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        m_angle = (m_angle + 8) % 360;
        update();
    });
}

QSize Spinner::sizeHint() const
{
    return {32, 32};
}

QSize Spinner::minimumSizeHint() const
{
    return {16, 16};
}

void Spinner::start()
{
    m_timer->start();
    show();
}

void Spinner::stop()
{
    m_timer->stop();
    hide();
}

bool Spinner::isSpinning() const
{
    return m_timer->isActive();
}

void Spinner::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int size = qMin(width(), height()) - 4;
    const QRectF rect(
        (width() - size) / 2.0,
        (height() - size) / 2.0,
        size,
        size
    );

    QPen pen(QColor("#B91C1C"));
    pen.setWidthF(3.0);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    painter.drawArc(rect, m_angle * 16, 90 * 16);
}
