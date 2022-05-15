#include "qpskdemodulator.h"

QpskDemodulator::QpskDemodulator()
{
    ampl = 0.;
    sp = 0.;
    sr = 0.;
    sf = 0.;
    dur = 0.;
    cobits = 0.;
}

QpskDemodulator::QpskDemodulator(double a, double sphase, double srate,
                                 double sfreq, double duration, double bits)
{
    ampl = a;
    sp = sphase;
    sr = srate;
    sf = sfreq;
    dur = duration;
    cobits = bits;
}

QpskDemodulator::~QpskDemodulator() {}

std::vector<double> QpskDemodulator::doSincConvolution(std::vector<double> arr, std::vector<double> sinc)
{
    std::vector<double> convolution;
    if (!arr.empty() && !sinc.empty()) {
        for (int i = 0; i < (int)arr.size(); i++) {
            double buf = 0.;
            for (int j = 0; j < (int)sinc.size(); j++) {
                if ((-j + i) >= 0) {
                    buf += arr[-j + i] * sinc[j];
                }
            }
            convolution.push_back(buf);
        }
    }
    return convolution;
}

void QpskDemodulator::getSmoothingComponents()
{
    if (!inputQpsk.empty()) {
        smoothIComp.clear();
        smoothQComp.clear();

        std::vector<double> sinc;
        for (int i = 0; i < (int)inputQpsk.size(); i++) {
            if (i == 0) {
                sinc.push_back(1.);
            } else {
                sinc.push_back(sin(0.3 * i) / (0.3 * i));
            }
        }

        std::vector<double> iComp;
        std::vector<double> qComp;
        double w = 2. * M_PI * sf;
        for (int i = 0; i < (int)inputQpsk.size(); i++) {
            iComp.push_back(cos(w * i / sr + sp) * inputQpsk[i]);
            qComp.push_back(sin(w * i / sr + sp) * inputQpsk[i]);
        }

        smoothIComp = doSincConvolution(iComp, sinc);
        smoothQComp = doSincConvolution(qComp, sinc);
    }
}

void QpskDemodulator::getBitsComponents()
{
    if (!smoothIComp.empty() && !smoothQComp.empty()) {
        bitsIComp.clear();
        bitsQComp.clear();
        int size = (int)smoothIComp.size();
        for (int i = 0; i < size; i++) {
            bitsIComp.push_back((smoothIComp[i] < 0) ? 0 : 1);
            bitsQComp.push_back((smoothQComp[i] < 0) ? 0 : 1);
        }
    }
}

void QpskDemodulator::getDemodBits()
{
    if (!bitsIComp.empty() && !bitsQComp.empty()) {
        demodBits.clear();
        int period = (sr * dur) / cobits;
        int size = (int)bitsIComp.size();
        for (int i = 0; i <= size; i+=period*2) {
            if (i != 0) {
                demodBits.push_back(bitsIComp[i - period]);
                demodBits.push_back(bitsQComp[i - period]);
            }
        }
    }
}
