#ifndef DONANTEDETAILDIALOG_H
#define DONANTEDETAILDIALOG_H

#include "models/Patientes.hpp"
#include "services/ApiError.hpp"
#include "services/CycleRuleService.hpp"

#include <QDialog>
#include <QJsonObject>
#include <QList>

class DonorService;
class RestraintService;
class QJsonArray;
class QLabel;
class QPushButton;
class QTableWidget;
class QListWidget;

class DonanteDetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DonanteDetailDialog(
        const Patientes &donante,
        QWidget *parent = nullptr);

    void markToggled();
    void setPendingActive(bool active) { m_pendingActive = active; }

signals:
    void toggleRequested(int donanteId, bool activate);

private:
    void buildUi();
    QWidget *buildDatosTab();
    QWidget *buildDireccionesTab();
    QWidget *buildCiclosTab();
    QWidget *buildObservacionesTab();
    void refreshAddresses();
    void refreshCycles();
    void refreshObservaciones();
    void refreshRestricciones();
    void onAddresses(const QJsonObject &page);
    void onCycles(const QJsonObject &page);
    void onCycleDetail(const QJsonObject &cycle);
    void onObservations(const QJsonArray &items);
    void onRestraints(const QJsonArray &items);
    void onFailed(const ApiError &error);
    void updateToggleLabel();
    void updateLastDonationLabelColor();

    DonorService *m_service = nullptr;
    RestraintService *m_restraintService = nullptr;
    CycleRuleService *m_ruleService = nullptr;
    QList<CycleRuleItem> m_cycleRules;
    Patientes m_donante;
    bool m_pendingActive = true;

    QLabel *m_stateLabel = nullptr;
    QLabel *m_lastDonationLabel = nullptr;
    QLabel *m_restraintLabel = nullptr;
    QPushButton *m_toggleBtn = nullptr;

    QTableWidget *m_addressesTable = nullptr;
    QListWidget *m_cyclesList = nullptr;
    QTableWidget *m_donationsTable = nullptr;
    QTableWidget *m_observationsTable = nullptr;
};

#endif
