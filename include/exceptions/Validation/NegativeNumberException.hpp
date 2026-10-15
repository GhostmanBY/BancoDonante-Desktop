#ifndef NEGATIVENUMBEREXCEPTION_H
#define NEGATIVENUMBEREXCEPTION_H

#include "exceptions/Validation/ValidationException.hpp"

class NegativeNumberException : public ValidationException
{
public:
    explicit NegativeNumberException(const QString &field)
        : ValidationException(QStringLiteral("El %1 no puede ser negativo").arg(field), field)
    {
    }
};

#endif
