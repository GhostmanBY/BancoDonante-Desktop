#ifndef DONANTESVIEW_H
#define DONANTESVIEW_H

#include "models/Patientes.hpp"
#include "models/enums/PatientState.hpp"
#include "services/ApiError.hpp"
#include "services/CycleRuleService.hpp"

#include <QList>
#include <QWidget>
#include <optional>

class DonantesModel;
class DonorService;
class EmptyState;
class PaginationBar;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QStackedWidget;
class QTableView;
class QTimer;

class DonantesView : public QWidget
{
    Q_OBJECT

public:
    explicit DonantesView(QWidget *parent = nullptr);

    void refresh();

private:
    void buildUi();
    void loadPage(int page);
    void onSearchChanged();
    void onStateFilterChanged();
    void onActiveFilterChanged();
    void onPageChanged(int page);
    void onSizeChanged(int size);
    void onSearchDebounced();
    void openCreateDialog();
    void openEditDialog();
    void toggleSelected();
    void openDetail();
    void openContextMenu(const QPoint &pos);
    void openAppointmentDialog();
    void openAppointmentFor(const Patientes &donor);
    bool confirmAppointment(const QString &message);
    void openRestraintCatalog();
    void updateActionButtons();
    bool canWrite() const;
    bool canToggle() const;
    std::optional<PatientState> selectedStateFilter() const;
    std::optional<bool> selectedActiveFilter() const;

    DonorService *m_service = nullptr;
    DonantesModel *m_model = nullptr;
    CycleRuleService *m_ruleService = nullptr;
    QList<CycleRuleItem> m_cycleRules;
    std::optional<Patientes> m_pendingAppointment;
    bool m_rulesLoaded = false;

    QLineEdit *m_search = nullptr;
    QComboBox *m_stateCombo = nullptr;
    QComboBox *m_activeCombo = nullptr;
    QPushButton *m_createBtn = nullptr;
    QPushButton *m_editBtn = nullptr;
    QPushButton *m_toggleBtn = nullptr;
    QPushButton *m_refreshBtn = nullptr;
    QTimer *m_debounce = nullptr;

    QStackedWidget *m_bodyStack = nullptr;
    QTableView *m_table = nullptr;
    PaginationBar *m_pagination = nullptr;
    EmptyState *m_empty = nullptr;
    QWidget *m_loading = nullptr;

    int m_page = 1;
    int m_size = 20;
    bool m_hasLoadedOnce = false;
};

#endif
