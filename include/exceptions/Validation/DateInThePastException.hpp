#ifndef DATEINTHEPASTEXCEPTION_H
#define DATEINTHEPASTEXCEPTION_H

#include "exceptions/Validation/ValidationException.hpp"

class DateInThePastException : public ValidationException
{
public:
    explicit DateInThePastException(const QString &field)
        : ValidationException(QStringLiteral("La fecha %1 es anterior al dia actual").arg(field), field)
    {
    }
};

#endif
