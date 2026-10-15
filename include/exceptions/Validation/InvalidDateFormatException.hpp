#ifndef INVALIDDATEFORMATEXCEPTION_H
#define INVALIDDATEFORMATEXCEPTION_H

#include "exceptions/Validation/ValidationException.hpp"

class InvalidDateFormatException : public ValidationException
{
public:
    InvalidDateFormatException(const QString &field, const QString &format)
        : ValidationException(
              QStringLiteral("La fecha %1 no cumple con el formato %2").arg(field, format),
              field)
    {
    }
};

#endif
