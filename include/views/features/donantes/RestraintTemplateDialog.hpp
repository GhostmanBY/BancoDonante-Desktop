#ifndef RESTRAINTTEMPLATEDIALOG_H
#define RESTRAINTTEMPLATEDIALOG_H

#include "services/ApiError.hpp"

#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class RestraintService;
struct RestraintTemplateItem;

class RestraintTemplateDialog : public QDialog
{
    Q_OBJECT

public:
    // Alta en el catálogo global.
    explicit RestraintTemplateDialog(QWidget *parent = nullptr);
    // Edición de una plantilla existente (cambia los defaults
    // globales; no toca las asignaciones ya hechas).
    explicit RestraintTemplateDialog(const RestraintTemplateItem &item,
                                     QWidget *parent = nullptr);

signals:
    void saved();

private:
    void buildUi();
    void submit();
    void onFailed(const ApiError &error);
    bool validate();

    RestraintService *m_service = nullptr;
    int m_templateId = 0;

    QLineEdit *m_motivo = nullptr;
    QSpinBox *m_months = nullptr;
    QLabel *m_formError = nullptr;
    QPushButton *m_acceptBtn = nullptr;
};

#endif
