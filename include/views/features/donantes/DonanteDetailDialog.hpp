#ifndef DONANTEDETAILDIALOG_H
#define DONANTEDETAILDIALOG_H

#include "models/Patientes.hpp"
#include "services/ApiError.hpp"

#include <QDialog>
#include <QJsonObject>

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
    void onAddresses(const QJsonObject &page);
    void onCycles(const QJsonObject &page);
    void onCycleDetail(const QJsonObject &cycle);
    void onObservations(const QJsonArray &items);
    void onFailed(const ApiError &error);
    void updateToggleLabel();

    DonorService *m_service = nullptr;
    RestraintService *m_restraintService = nullptr;
    Patientes m_donante;
    bool m_pendingActive = true;

    QLabel *m_stateLabel = nullptr;
    QLabel *m_lastDonationLabel = nullptr;
    QPushButton *m_toggleBtn = nullptr;

    QTableWidget *m_addressesTable = nullptr;
    QListWidget *m_cyclesList = nullptr;
    QTableWidget *m_donationsTable = nullptr;
    QTableWidget *m_observationsTable = nullptr;
};

#endif
