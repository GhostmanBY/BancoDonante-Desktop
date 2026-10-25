#ifndef PASSWORDREVEAL_H
#define PASSWORDREVEAL_H

#include <QIcon>

class QAction;
class QLineEdit;

/**
 * Botón con "ojito" dentro de un QLineEdit para mostrar/ocultar la
 * contraseña. La acción queda al final del campo y alterna el modo de
 * eco, el ícono y el tooltip.
 */
class PasswordReveal
{
public:
    static QIcon icon(bool hidden);
    static QAction *attach(QLineEdit *edit);
};

#endif
