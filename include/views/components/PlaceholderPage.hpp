#ifndef PLACEHOLDERPAGE_H
#define PLACEHOLDERPAGE_H

#include <QWidget>

class PlaceholderPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlaceholderPage(
        const QString &title,
        const QString &subtitle = QString(),
        QWidget *parent = nullptr
    );
};

#endif
