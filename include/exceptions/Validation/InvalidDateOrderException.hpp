#ifndef INVALIDDATEORDEREXCEPTION_H
#define INVALIDDATEORDEREXCEPTION_H

#include "exceptions/Validation/ValidationException.hpp"

class InvalidDateOrderException : public ValidationException
{
public:
    InvalidDateOrderException(const QString &startField, const QString &endField)
        : ValidationException(
              QStringLiteral("La fecha %1 no puede ser posterior a %2").arg(startField, endField),
              endField)
    {
    }
};

#endif
