#ifndef PAGINATIONBAR_H
#define PAGINATIONBAR_H

#include <QFrame>
#include <QString>

class QComboBox;
class QLabel;
class QPushButton;

class PaginationBar : public QFrame
{
    Q_OBJECT

public:
    explicit PaginationBar(QWidget *parent = nullptr);

    void setPagination(int page, int totalPages, int total, QString entity);
    void setLoading(bool loading);

signals:
    void pageChanged(int page);
    void sizeChanged(int size);

private:
    void updateButtons();
    void updateInfo();

    QLabel *m_info = nullptr;
    QPushButton *m_first = nullptr;
    QPushButton *m_prev = nullptr;
    QPushButton *m_next = nullptr;
    QPushButton *m_last = nullptr;
    QComboBox *m_sizeCombo = nullptr;

    QString m_entity = "";
    int m_page = 1;
    int m_totalPages = 1;
    int m_total = 0;
};

#endif
