#ifndef TURNOSVIEW_H
#define TURNOSVIEW_H

#include "models/enums/AppointmentState.hpp"
#include "services/ApiError.hpp"
#include "services/AppointmentService.hpp"

#include <QSet>
#include <QWidget>
#include <optional>

class AppointmentService;
class EmptyState;
class PaginationBar;
class QComboBox;
class QLabel;
class QPushButton;
class QStackedWidget;
class QTableView;
class TurnosModel;
class WhatsAppService;

class TurnosView : public QWidget
{
    Q_OBJECT

public:
    explicit TurnosView(QWidget *parent = nullptr);

    void refresh();

private:
    void buildUi();
    void loadPage(int page);
    void onStateFilterChanged();
    void onPageChanged(int page);
    void onSizeChanged(int size);
    void openContextMenu(const QPoint &pos);
    void openCompleteDialog();
    void openRescheduleDialog();
    void changeSelectedState(AppointmentState state);
    void markExpiredAsNoShow(const QList<TurnoItem> &items);
    void updateNotifyButton();
    void notifySelectedPatient();
    bool canWrite() const;
    std::optional<AppointmentState> selectedStateFilter() const;

    AppointmentService *m_service = nullptr;
    WhatsAppService *m_whatsAppService = nullptr;
    TurnosModel *m_model = nullptr;

    QComboBox *m_stateCombo = nullptr;
    QPushButton *m_refreshBtn = nullptr;
    QPushButton *m_notifyBtn = nullptr;

    QStackedWidget *m_bodyStack = nullptr;
    QTableView *m_table = nullptr;
    PaginationBar *m_pagination = nullptr;
    EmptyState *m_empty = nullptr;
    QWidget *m_loading = nullptr;

    int m_page = 1;
    int m_size = 20;
    bool m_hasLoadedOnce = false;
    QSet<int> m_autoNoShowTried;
};

#endif
