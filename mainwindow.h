#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <algorithm>
#include "qcustomplot.h"
#include "qpskmodulator.h"
#include "qpskdemodulator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class PlotType
{
    BITS = 0,
    GRAPH = 1
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Получить векторы "x", "y" для отрисовки битов.
    void getBitsToPlot(QVector<double> &x, QVector<double> &y, std::vector<double> bits);
    // Получить векторы "x", "y" для отрисовки графика.
    void getGraphToPlot(QVector<double> &x, QVector<double> &y, std::vector<double> graphic);
    // Инициализировать графики.
    void initializePlots();
    // Построить график.
    void plot(QCustomPlot* obj, std::vector<double> arr, QString yLab,
              QCPGraph::LineStyle lstyle, QPen pen, PlotType type);

private slots:
    void getInputBits();
    void doModulation();
    void doDemodulation();

private:
    Ui::MainWindow *ui;
    QpskModulator *modulator;
    QpskDemodulator *demodulator;

    double ampl = 1.;
    double sp = 0.;
    double sr = 512.;
    double sf = 20e3;
    double dur = 1.;
    int cobits = 16;
};
#endif // MAINWINDOW_H
