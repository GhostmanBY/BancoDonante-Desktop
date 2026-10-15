#ifndef INVALIDVALUEEXCEPTION_H
#define INVALIDVALUEEXCEPTION_H

#include "exceptions/Validation/ValidationException.hpp"

class InvalidValueException : public ValidationException
{
public:
    explicit InvalidValueException(const QString &field)
        : ValidationException(QStringLiteral("Se ingreso un tipo de dato invalido en %1").arg(field), field)
    {
    }
};

#endif
