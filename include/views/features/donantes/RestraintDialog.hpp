#ifndef RESTRAINTDIALOG_H
#define RESTRAINTDIALOG_H

#include "models/Patientes.hpp"
#include "services/ApiError.hpp"

#include <QDialog>

class QLabel;
class QLineEdit;
class QDateEdit;
class QPushButton;
class RestraintService;
struct RestraintTemplateItem;

class RestraintDialog : public QDialog
{
    Q_OBJECT

public:
    // Asignación manual (sin plantilla).
    explicit RestraintDialog(const Patientes &donante, QWidget *parent = nullptr);
    // Asignación desde una plantilla del catálogo: precarga motivo y
    // rango por defecto; el usuario puede editarlos antes de asignar.
    // Se envía template_id para dejar constancia de la procedencia.
    RestraintDialog(const Patientes &donante,
                    const RestraintTemplateItem &prefill,
                    QWidget *parent = nullptr);

signals:
    void assigned(int cancelledAppointments);

private:
    void buildUi();
    void submit();
    void onFailed(const ApiError &error);
    bool validate();

    RestraintService *m_service = nullptr;
    Patientes m_donante;
    int m_templateId = 0;

    QLineEdit *m_motivo = nullptr;
    QDateEdit *m_startDate = nullptr;
    QDateEdit *m_endDate = nullptr;
    QLabel *m_formError = nullptr;
    QPushButton *m_acceptBtn = nullptr;
};

#endif
