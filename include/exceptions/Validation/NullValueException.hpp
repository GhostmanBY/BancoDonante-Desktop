#ifndef NULLVALUEEXCEPTION_H
#define NULLVALUEEXCEPTION_H

#include "exceptions/Validation/ValidationException.hpp"

class NullValueException : public ValidationException
{
public:
    explicit NullValueException(const QString &field)
        : ValidationException(QStringLiteral("El %1 no puede ser nulo o vacio").arg(field), field)
    {
    }
};

#endif
