#ifndef MISSINGVALUEEXCEPTION_H
#define MISSINGVALUEEXCEPTION_H

#include "exceptions/Validation/ValidationException.hpp"

class MissingValueException : public ValidationException
{
public:
    explicit MissingValueException(const QString &field)
        : ValidationException(QStringLiteral("El %1 es obligatorio").arg(field), field)
    {
    }
};

#endif
