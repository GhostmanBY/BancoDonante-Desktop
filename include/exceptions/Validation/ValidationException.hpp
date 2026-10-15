#ifndef VALIDATIONEXCEPTION_H
#define VALIDATIONEXCEPTION_H

#include <QString>
#include <exception>
#include <string>

class ValidationException : public std::exception
{
public:
    ValidationException(const QString &message, const QString &field, int code = 400)
        : m_message(message), m_what(message.toStdString()), m_field(field), m_code(code)
    {
    }

    const char *what() const noexcept override
    {
        return m_what.c_str();
    }

    QString message() const { return m_message; }
    QString field() const { return m_field; }
    int code() const { return m_code; }

private:
    QString m_message;
    std::string m_what;
    QString m_field;
    int m_code;
};

#endif
