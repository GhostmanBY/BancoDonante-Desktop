#include "views/components/Toast.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPointer>
#include <QStyle>
#include <QTimer>

Toast::Toast(QWidget *parent)
    : QFrame(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName(QStringLiteral("toast"));

    m_label = new QLabel(this);
    m_label->setWordWrap(true);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(16, 12, 16, 12);
    layout->addWidget(m_label);
}

QString Toast::typeToString(Type type)
{
    switch (type) {
    case Type::Success: return QStringLiteral("success");
    case Type::Error: return QStringLiteral("error");
    case Type::Info: return QStringLiteral("info");
    }
    return QStringLiteral("info");
}

void Toast::reposition()
{
    auto *parentWidget = qobject_cast<QWidget *>(parent());
    if (!parentWidget) {
        return;
    }

    move(
        (parentWidget->width() - width()) / 2,
        16
    );
}

void Toast::show(
    QWidget *parent,
    const QString &message,
    Type type,
    int durationMs
)
{
    if (!parent) {
        return;
    }

    auto *toast = new Toast(parent);
    toast->setProperty("type", typeToString(type));
    toast->m_label->setProperty("type", typeToString(type));
    toast->m_label->setText(message);
    toast->m_durationMs = durationMs;
    toast->setStyleSheet(parent->styleSheet()); // inherit theme

    // Force style re-resolution for the dynamic property
    toast->style()->unpolish(toast);
    toast->style()->polish(toast);

    toast->adjustSize();
    toast->reposition();
    toast->show();
    toast->raise();

    QPointer<Toast> guard(toast);
    QTimer::singleShot(durationMs, toast, [guard]() {
        if (guard) {
            guard->close();
        }
    });
}
