#ifndef OUTOFRANGEEXCEPTION_H
#define OUTOFRANGEEXCEPTION_H

#include "exceptions/Validation/ValidationException.hpp"

class OutOfRangeException : public ValidationException
{
public:
    OutOfRangeException(const QString &field, const QString &rango)
        : ValidationException(
              QStringLiteral("El valor de %1 debe estar dentro del rango %2").arg(field, rango),
              field)
    {
    }
};

#endif
