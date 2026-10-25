#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

#include "services/CycleRuleService.hpp"

#include <QWidget>

class QSpinBox;
class QVBoxLayout;
class WhatsAppService;
struct WSPTemplateItem;

class SettingsView : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsView(QWidget *parent = nullptr);

    void refresh();

private:
    void buildUi();
    void buildRuleRow(QVBoxLayout *rulesLayout, const CycleRuleItem &rule);
    void clearRuleRows();
    void buildTemplateRow(QVBoxLayout *templatesLayout, const WSPTemplateItem &item);
    void clearTemplateRows();

    QSpinBox *m_restDays = nullptr;
    CycleRuleService *m_ruleService = nullptr;
    WhatsAppService *m_whatsAppService = nullptr;
    QVBoxLayout *m_rulesLayout = nullptr;
    QWidget *m_rulesBox = nullptr;
    QVBoxLayout *m_templatesLayout = nullptr;
    QWidget *m_templatesBox = nullptr;
};

#endif
