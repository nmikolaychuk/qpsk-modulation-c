#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Загрузка интерфейса.
    ui->setupUi(this);

    // Обработчики.
    connect(ui->getInputBitsButton, SIGNAL(released()), this, SLOT(getInputBits()));
    connect(ui->doModulationButton, SIGNAL(released()), this, SLOT(doModulation()));
    connect(ui->doDemodulationButton, SIGNAL(released()), this, SLOT(doDemodulation()));

    // Инициализация графиков.
    initializePlots();

    // Объекты модулятора/демодулятора.
    modulator = new QpskModulator(ampl, sp, sr, sf, dur, cobits);
    demodulator = new QpskDemodulator(ampl, sp, sr, sf, dur, cobits);
}

void MainWindow::initializePlots()
{
    // Цвет фона.
    ui->inputModSignal->setBackground(QColor(235, 235, 235));
    ui->outputModSignal->setBackground(QColor(235, 235, 235));
    ui->generalSignal->setBackground(QColor(235, 235, 235));
    ui->qpskIComp->setBackground(QColor(235, 235, 235));
    ui->qpskQComp->setBackground(QColor(235, 235, 235));
    ui->qpskSpectrum->setBackground(QColor(235, 235, 235));
    ui->inputDemodSignal->setBackground(QColor(235, 235, 235));
    ui->qpskICompDemod->setBackground(QColor(235, 235, 235));
    ui->qpskQCompDemod->setBackground(QColor(235, 235, 235));
    ui->iBits->setBackground(QColor(235, 235, 235));
    ui->qBits->setBackground(QColor(235, 235, 235));
    ui->outputDemodSignal->setBackground(QColor(235, 235, 235));
    // Подписи по оси ординат.
    ui->inputModSignal->yAxis->setTicks(false);
    ui->outputModSignal->yAxis->setTicks(false);
    ui->generalSignal->yAxis->setTicks(false);
    ui->qpskIComp->yAxis->setTicks(false);
    ui->qpskQComp->yAxis->setTicks(false);
    ui->qpskSpectrum->yAxis->setTicks(false);
    ui->inputDemodSignal->yAxis->setTicks(false);
    ui->qpskICompDemod->yAxis->setTicks(false);
    ui->qpskQCompDemod->yAxis->setTicks(false);
    ui->iBits->yAxis->setTicks(false);
    ui->qBits->yAxis->setTicks(false);
    ui->outputDemodSignal->yAxis->setTicks(false);
}

void MainWindow::getBitsToPlot(QVector<double> &x, QVector<double> &y,
                               std::vector<double> bits)
{
    if (!bits.empty()) {
        x.clear();
        y.clear();
        for (int i = 0; i < (int)bits.size(); i++) {
            x.append(i);
            y.append(bits[i]);
            if (i == (int)bits.size() - 1) {
                x.append(i+1);
                y.append(bits[i]);
            }
        }
    }
}

void MainWindow::getGraphToPlot(QVector<double> &x, QVector<double> &y,
                                std::vector<double> graphic)
{
    if (!graphic.empty()) {
        x.clear();
        y.clear();
        for (int i = 0; i < (int)graphic.size(); i++) {
            x.append(i);
            y.append(graphic[i]);
        }
    }
}

void MainWindow::plot(QCustomPlot* obj, std::vector<double> arr, QString yLab,
                      QCPGraph::LineStyle lstyle, QPen pen, PlotType type)
{
    if (!arr.empty()) {
        QVector<double> x, y;
        if (type == PlotType::BITS) {
            getBitsToPlot(x, y, arr);
        } else if (type == PlotType::GRAPH) {
            getGraphToPlot(x, y, arr);
        }


        obj->addGraph();
        obj->graph(0)->setData(x, y);
        obj->graph(0)->setPen(pen);
        obj->graph(0)->setBrush(QBrush(QColor(pen.color().red(),
                                              pen.color().green(),
                                              pen.color().blue(), 20)));
        obj->graph(0)->setLineStyle(lstyle);

        // Нахождение диапазона по "y".
        double min = *std::min_element(y.constBegin(), y.constEnd());
        double max = *std::max_element(y.constBegin(), y.constEnd());

        obj->xAxis->setRange(0, arr.size());
        obj->yAxis->setRange(min, max);
        obj->yAxis->setLabel(yLab);
        obj->replot();
    }
}

void MainWindow::getInputBits()
{
    modulator->generateGeneralSignal();
    modulator->generateInputBits();
    modulator->getQpskComponents();
    plot(ui->inputModSignal, modulator->inputBits, QString("BITS"),
         QCPGraph::lsStepLeft, QPen(QColor(255, 0, 0)), PlotType::BITS);
}

void MainWindow::doModulation()
{
    modulator->getQpskSignal();
    modulator->getQpskSpectrum();
    plot(ui->qpskIComp, modulator->iComp,  QString("I"),
         QCPGraph::lsStepLeft, QPen(QColor(0, 0, 255)), PlotType::BITS);
    plot(ui->qpskQComp, modulator->qComp, QString("Q"),
         QCPGraph::lsStepLeft, QPen(QColor(0, 255, 0)), PlotType::BITS);
    plot(ui->generalSignal, modulator->generalSignal, QString("НС"),
         QCPGraph::lsLine, QPen(QColor(200, 50, 100)), PlotType::GRAPH);
    plot(ui->qpskSpectrum, modulator->qpskSpectrum, QString("SPEC"),
         QCPGraph::lsLine, QPen(QColor(255, 0, 255)), PlotType::GRAPH);
    plot(ui->outputModSignal, modulator->qpskSignal, QString("QPSK"),
         QCPGraph::lsLine, QPen(QColor(50, 150, 105)), PlotType::GRAPH);
}

void MainWindow::doDemodulation()
{
    if (!modulator->qpskSignal.empty()) {
        demodulator->inputQpsk = modulator->qpskSignal;
        demodulator->getSmoothingComponents();
        demodulator->getBitsComponents();
        demodulator->getDemodBits();
        plot(ui->inputDemodSignal, demodulator->inputQpsk, QString("QPSK"),
             QCPGraph::lsLine, QPen(QColor(50, 150, 105)), PlotType::GRAPH);
        plot(ui->qpskICompDemod, demodulator->smoothIComp, QString("I"),
             QCPGraph::lsLine, QPen(QColor(0, 255, 0)), PlotType::GRAPH);
        plot(ui->qpskQCompDemod, demodulator->smoothQComp, QString("Q"),
             QCPGraph::lsLine, QPen(QColor(200, 50, 105)), PlotType::GRAPH);
        plot(ui->iBits, demodulator->bitsIComp, QString("I-BITS"),
             QCPGraph::lsStepLeft, QPen(QColor(255, 150, 105)), PlotType::BITS);
        plot(ui->qBits, demodulator->bitsQComp, QString("Q-BITS"),
             QCPGraph::lsStepLeft, QPen(QColor(255, 0, 105)), PlotType::BITS);
        plot(ui->outputDemodSignal, demodulator->demodBits, QString("BITS"),
             QCPGraph::lsStepLeft, QPen(QColor(50, 100, 205)), PlotType::BITS);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete modulator;
    delete demodulator;
}
