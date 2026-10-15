#ifndef INVALIDDATERANGEEXCEPTION_H
#define INVALIDDATERANGEEXCEPTION_H

#include "exceptions/Validation/ValidationException.hpp"

class InvalidDateRangeException : public ValidationException
{
public:
    explicit InvalidDateRangeException(const QString &field)
        : ValidationException(QStringLiteral("La fecha %1 excede a una fecha realista.").arg(field), field)
    {
    }
};

#endif
