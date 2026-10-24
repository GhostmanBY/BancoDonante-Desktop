#ifndef EMPTYSTATE_H
#define EMPTYSTATE_H

#include <QWidget>

class QLabel;

class EmptyState : public QWidget
{
    Q_OBJECT

public:
    explicit EmptyState(
        const QString &title,
        const QString &subtitle = QString(),
        const QString &glyph = QStringLiteral("♦"),
        QWidget *parent = nullptr
    );

private:
    QLabel *m_glyphLabel = nullptr;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_subtitleLabel = nullptr;
};

#endif
