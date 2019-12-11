#include "chartwindow.h"
#include <QDebug>

/**
 * @brief Initializes the ChartWindow.
 * @param parent Parent QWidget. Can be left empty.
 */
ChartWindow::ChartWindow(QWidget *parent)
: QMainWindow(parent) {
    resize(800, 600);
}

void ChartWindow::setChart(QtCharts::QChart *chart) {
    if (this->centralWidget()) {
        this->centralWidget()->deleteLater();
    }

    this->chartView_priv.reset(new QtCharts::QChartView());
    chartView_priv->setRenderHint(QPainter::Antialiasing);
    chartView_priv->setChart(chart);
    this->setCentralWidget(chartView_priv.data());
}

void ChartWindow::renderPng(const QString &fileName) {
    if (!this->centralWidget()) {
        qWarning() << "Can't render chart without the central widget";
        return;
    }

    QPixmap p(this->size());
    this->centralWidget()->render(&p);
    p.save(fileName, "PNG");
}
