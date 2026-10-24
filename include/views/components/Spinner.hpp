#ifndef SPINNER_H
#define SPINNER_H

#include <QWidget>

class QTimer;

class Spinner : public QWidget
{
    Q_OBJECT

public:
    explicit Spinner(QWidget *parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void start();
    void stop();
    bool isSpinning() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QTimer *m_timer = nullptr;
    int m_angle = 0;
};

#endif
