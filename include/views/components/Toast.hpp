#ifndef TOAST_H
#define TOAST_H

#include <QFrame>
#include <QString>

class QLabel;
class QWidget;

class Toast : public QFrame
{
    Q_OBJECT

public:
    enum class Type
    {
        Success,
        Error,
        Info,
    };

    explicit Toast(QWidget *parent = nullptr);

    using QWidget::show;

    static void show(
        QWidget *parent,
        const QString &message,
        Type type = Type::Info,
        int durationMs = 3000
    );

private:
    static QString typeToString(Type type);
    void reposition();

    QLabel *m_label = nullptr;
    int m_durationMs = 3000;
};

#endif
