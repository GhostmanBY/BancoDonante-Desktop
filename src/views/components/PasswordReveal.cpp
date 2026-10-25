#include "views/components/PasswordReveal.hpp"

#include <QAction>
#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>

QIcon PasswordReveal::icon(bool hidden)
{
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.scale(2.0, 2.0);

    const QColor color = hidden ? QColor(QStringLiteral("#4B5563"))
                                 : QColor(QStringLiteral("#9CA3AF"));
    QPen pen(color, 1.4);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    QPainterPath eye;
    eye.moveTo(1.0, 8.0);
    eye.quadTo(8.0, 1.5, 15.0, 8.0);
    eye.quadTo(8.0, 14.5, 1.0, 8.0);
    eye.closeSubpath();
    painter.drawPath(eye);

    if (hidden) {
        painter.setBrush(color);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(8.0, 8.0), 2.4, 2.4);
    } else {
        painter.drawLine(QPointF(1.5, 14.5), QPointF(14.5, 1.5));
    }

    return QIcon(pixmap);
}

QAction *PasswordReveal::attach(QLineEdit *edit)
{
    QAction *action = edit->addAction(icon(true), QLineEdit::TrailingPosition);
    action->setToolTip(QStringLiteral("Mostrar contraseña"));

    QObject::connect(action, &QAction::triggered, edit, [edit, action]() {
        const bool hidden = edit->echoMode() != QLineEdit::Normal;

        if (hidden) {
            edit->setEchoMode(QLineEdit::Normal);
            edit->setCursorPosition(edit->text().size());
        } else {
            edit->setEchoMode(QLineEdit::Password);
        }

        action->setIcon(icon(!hidden));
        action->setToolTip(hidden ? QStringLiteral("Ocultar contraseña")
                                  : QStringLiteral("Mostrar contraseña"));
    });

    return action;
}
