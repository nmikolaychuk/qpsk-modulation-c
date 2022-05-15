#ifndef QPSKMODULATOR_H
#define QPSKMODULATOR_H

#endif // QPSKMODULATOR_H

#include <vector>
#include <random>
#include <math.h>
#include <complex>

class QpskModulator
{
private:
    // Амплитуда несущего сигнала.
    double ampl;
    // Начальная фаза несущего сигнала.
    double sp;
    // Частота дискретизации несущего сигнала.
    double sr;
    // Частота несущей синусоиды, Гц.
    double sf;
    // Длительность несущего сигнала, сек.
    double dur;
    // Длина входной последовательности, бит.
    int cobits;
    // Синхронизация начальной фазы.
    double syncTime;

public:
    // Буфер для хранения входной последовательности бит.
    std::vector<double> inputBits;
    // Буфер для хранения несущего сигнала.
    std::vector<double> generalSignal;
    // Буфер для хранения I-компоненты.
    std::vector<double> iComp;
    // Буфер для хранения Q-компоненты.
    std::vector<double> qComp;
    // Буфер для хранения модулированного сигнала.
    std::vector<double> qpskSignal;
    // Буфер для хранения модуля спектра модулированного сигнала.
    std::vector<double> qpskSpectrum;

    QpskModulator();
    QpskModulator(double a, double sp, double sr, double sf, double dur, double bits);
    ~QpskModulator();

    // Генерировать последовательность входных бит.
    void generateInputBits();
    // Генерировать несущий сигнал (синусоиду).
    void generateGeneralSignal();
    // Получить синфазную и квадратурную компоненты.
    void getQpskComponents();
    // Получить модулированный сигнал.
    void getQpskSignal();
    // Получить спектр модулированного сигнала.
    void getQpskSpectrum();

private:
    // Быстрое преобразование фурье.
    void fft(std::vector<std::complex<double>>& arr, int direction);
};
