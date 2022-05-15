#include "qpskmodulator.h"

QpskModulator::QpskModulator()
{
    ampl = 0.;
    sp = 0.;
    sr = 0.;
    sf = 0.;
    dur = 0.;
    cobits = 0.;
    syncTime = 0.;
}

QpskModulator::QpskModulator(double a, double sphase, double srate,
                             double sfreq, double duration, double bits)
{
    ampl = a;
    sp = sphase;
    sr = srate;
    sf = sfreq;
    dur = duration;
    cobits = bits;
    syncTime = 0.;
    srand(time(0));
}

QpskModulator::~QpskModulator() {}

void QpskModulator::generateInputBits()
{
    inputBits.clear();
    if (cobits > 0) {
        for (int i = 0; i < cobits; i++) {
            inputBits.push_back(double(rand() < RAND_MAX * 0.5));
        }
    }
}

void QpskModulator::generateGeneralSignal()
{
    generalSignal.clear();
    int lengthOfSignal = round(sr * dur) + syncTime;
    // Частота сигнала, Гц.
    for (int t = syncTime; t < lengthOfSignal; ++t) {
        // Вычисление частоты.
        double w = 2. * M_PI * sf;
        generalSignal.push_back(ampl * sin(w * t / sr + sp));
    }
    syncTime = lengthOfSignal - 1;
}

void QpskModulator::getQpskComponents()
{
    if (!inputBits.empty()) {
        iComp.clear();
        qComp.clear();
        for (int i = 0; i < (int)inputBits.size(); i++) {
            if (i % 2 == 0) {
                iComp.push_back(inputBits[i]);
            } else {
                qComp.push_back(inputBits[i]);
            }
        }
    }
}

void QpskModulator::getQpskSignal()
{
    if (!iComp.empty() && !qComp.empty()) {
        qpskSignal.clear();
        int length = round(sr * dur);
        double iqStep = length / (cobits / 2.);
        for (int i = 0; i < length; i++) {
            int iqIndex = int(i / iqStep);
            double w = 2. * M_PI * sf;
            double iBuf = iComp[iqIndex] == 0 ? -1 : 1;
            double qBuf = qComp[iqIndex] == 0 ? -1 : 1;
            qpskSignal.push_back(iBuf * cos(w * i / sr + sp) + qBuf * sin(w * i / sr + sp));
        }
    }
}

void QpskModulator::getQpskSpectrum()
{
    if (!qpskSignal.empty()) {
        qpskSpectrum.clear();
        std::vector<std::complex<double>> spectrum;
        for (int i = 0; i < (int)qpskSignal.size(); i++) {
            spectrum.push_back(qpskSignal[i]);
        }

        QpskModulator::fft(spectrum, -1);
        for (int i = 0; i < (int)spectrum.size(); i++) {
            qpskSpectrum.push_back(abs(spectrum[i]));
        }
    }
}

void QpskModulator::fft(std::vector<std::complex<double>>& arr, int direction)
{
    unsigned int pts = 2;
    while (arr.size() > pts) {
        pts *= 2;
    }

    int pts_to_add = pts - arr.size();
    for (int ttt = 0; ttt < pts_to_add; ttt++) {
        arr.push_back(std::complex<double>(0, 0));
    }
    int n = arr.size();
    int i, j, istep;
    int m, mmax;
    double r, r1, theta, w_r, w_i, temp_r, temp_i;
    r = 3.14159 * direction;
    j = 0;

    for (i = 0; i < n; i++) {
        if (i < j) {
            temp_r = arr[j].real();
            temp_i = arr[j].imag();
            arr[j] = arr[i];
            arr[i] = std::complex<double>(temp_r, temp_i);
        }
        m = n >> 1;
        while (j >= m) {
            j -= m;
            m = (m + 1) / 2;
        }
        j += m;
    }
    mmax = 1;
    while (mmax < n) {
        istep = mmax << 1;
        r1 = r / (double)mmax;
        for (m = 0; m < mmax; m++) {
            theta = r1 * m;
            w_r = (double)cos((double)theta);
            w_i = (double)sin((double)theta);
            for (i = m; i < n; i += istep) {
                j = i + mmax;
                temp_r = w_r * arr[j].real() - w_i * arr[j].imag();
                temp_i = w_r * arr[j].imag() + w_i * arr[j].real();
                arr[j] = std::complex<double>((arr[i].real() - temp_r), (arr[i].imag() - temp_i));
                arr[i] += std::complex<double>(temp_r, temp_i);
            }
        }
        mmax = istep;
    }
    if (direction > 0) {
        double sqn = sqrt(n);
        for (i = 0; i < n; i++) {
            arr[i] /= sqn;
        }
    }
}
