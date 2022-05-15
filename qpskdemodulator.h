#ifndef QPSKDEMODULATOR_H
#define QPSKDEMODULATOR_H

#endif // QPSKDEMODULATOR_H

#include <vector>
#include <math.h>

class QpskDemodulator
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

public:
    // Буфер для хранения исходного Qpsk сигнала.
    std::vector<double> inputQpsk;
    // Буфер для хранения сглаженной I-компоненты из Qpsk.
    std::vector<double> smoothIComp;
    // Буфер для хранения сглаженной Q-компоненты из Qpsk.
    std::vector<double> smoothQComp;
    // Буфер для хранения битов I-компоненты.
    std::vector<double> bitsIComp;
    // Буфер для хранения битов Q-компоненты.
    std::vector<double> bitsQComp;
    // Буфер для хранения последовательности бит после демодуляции.
    std::vector<double> demodBits;

    QpskDemodulator();
    QpskDemodulator(double a, double sp, double sr, double sf, double dur, double bits);
    ~QpskDemodulator();

    // Получить сглаженные компоненты I, Q из входного Qpsk сигнала.
    void getSmoothingComponents();
    // Получить биты компонент I, Q из сглаженных компонент.
    void getBitsComponents();
    // Получить последовательность бит после демодуляции.
    void getDemodBits();

private:
    // Свернуть сигнал с Sinc.
    std::vector<double> doSincConvolution(std::vector<double> arr, std::vector<double> sinc);
};
