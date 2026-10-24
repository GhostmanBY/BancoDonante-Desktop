#ifndef RESTRAINTCATALOGDIALOG_H
#define RESTRAINTCATALOGDIALOG_H

#include "models/Patientes.hpp"
#include "services/ApiError.hpp"

#include <QDialog>
#include <QList>

class QLabel;
class QPushButton;
class QTableWidget;
class RestraintService;
struct RestraintTemplateItem;

class RestraintCatalogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RestraintCatalogDialog(const Patientes &donante,
                                    QWidget *parent = nullptr);

signals:
    void assigned(int cancelledAppointments);

private:
    void buildUi();
    void reload();
    void onTemplatesFetched(const QList<RestraintTemplateItem> &templates);
    void openAssign();
    void openManualAssign();
    void openCreate();
    void openEdit();
    void onFailed(const ApiError &error);
    void updateButtons();
    bool canManage() const;
    const RestraintTemplateItem *selectedTemplate() const;

    RestraintService *m_service = nullptr;
    Patientes m_donante;
    QList<RestraintTemplateItem> m_templates;

    QLabel *m_hint = nullptr;
    QTableWidget *m_table = nullptr;
    QLabel *m_formError = nullptr;
    QPushButton *m_assignBtn = nullptr;
    QPushButton *m_manualBtn = nullptr;
    QPushButton *m_newBtn = nullptr;
    QPushButton *m_editBtn = nullptr;
};

#endif
