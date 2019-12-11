#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QChart>
#include <QChartView>
#include <QCloseEvent>
#include <QMainWindow>
#include <QScopedPointer>

/**
 * @brief The ChartWindow class instances are used to display various measurement diagrams
 */
class ChartWindow : public QMainWindow {
    Q_OBJECT

private:
    QScopedPointer<QtCharts::QChartView> chartView_priv; //!< The Widget that is used to display the chart itself

public:
    explicit ChartWindow(QWidget *parent = nullptr);
    void setChart(QtCharts::QChart *chart);
    void renderPng(const QString &fileName);


};

#endif // CHARTWINDOW_H
