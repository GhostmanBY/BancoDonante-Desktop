#ifndef ESTADOCHIP_H
#define ESTADOCHIP_H

#include "models/enums/PatientState.hpp"

#include <QFrame>

class QLabel;

class EstadoChip : public QFrame
{
    Q_OBJECT

public:
    explicit EstadoChip(QWidget *parent = nullptr);

    void setState(PatientState state);

    QSize sizeHint() const override;

private:
    QLabel *m_label = nullptr;
};

#endif
