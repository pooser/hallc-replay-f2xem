#!/usr/bin/python

import pickle
import numpy as np
import matplotlib.pyplot as plt
import scipy.optimize as optimization

hdd = pickle.load(open('data-dicts/hmsDataDict.pkl', 'rb'))
pdd = pickle.load(open('data-dicts/shmsDataDict.pkl', 'rb'))

edtmRate = 100.

def lt(rate, busy) :
    return 100.*(1. / (1.+rate*busy))

def calcSumErr(err1, err2) :
    return np.sqrt(err1**2 + err2**2) 

# Heuristic guess for linear dependence
def linDiff(busyWidth, trigRate, edtmRate) :
    return (edtmRate*busyWidth)*(1. - trigRate*busyWidth)

def linFit(x, a, b) :
    return a*x + b

def quadFit(x, a, b) :
    return a - (b*x / (x + edtmRate))

# Dave M. Corrections
def ltPoisson(busyWidth, trigRate, edtmRate) :
    return 1. - (trigRate + edtmRate)*busyWidth

def ltClock(busyWidth, trigRate, edtmRate) :
    return 1. - trigRate*busyWidth*(1. + (edtmRate / (edtmRate + trigRate)))

# def ltClock (busyWidth, trigRate, edtmRate) :
#     return 1. - (edtmRate + trigRate)*busyWidth + ((trigRate**2 + trigRate*edtmRate)*busyWidth**2)*(1. + (edtmRate / (edtmRate + trigRate)))

# Second order Poisson TS expansion
def dtFirstOrder(busyWidth, trigRate, edtmRate) :
    return (edtmRate + trigRate)*busyWidth

def dtSecondOrder(busyWidth, trigRate, edtmRate) :
    return ((trigRate**2 + trigRate*edtmRate)*busyWidth**2)*(1. + (edtmRate / (edtmRate + trigRate)))

def ltSecOrder(busyWidth, trigRate, edtmRate) :
    return 1. - (trigRate + edtmRate)*busyWidth + ((trigRate**2 + trigRate*edtmRate)*busyWidth**2)*(1. + (edtmRate / (edtmRate + trigRate)))

# Second order non-extended live time approximation
def testPoisson(busyWidth, trigRate, edtmRate) :
    return (1. / (trigRate + edtmRate)*busyWidth)

def testClock(busyWidth, trigRate, edtmRate) :
    return ((trigRate + edtmRate)*busyWidth / (1. + trigRate**2*busyWidth**2 +
                                               2.*busyWidth**2*edtmRate*trigRate +
                                               2.*trigRate*busyWidth + 2.*edtmRate*busyWidth))

# =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

hmsRate    = np.linspace(0, 7500, 7500)
hmsBusyMin = 200.e-6
hmsBusyMax = 250.e-6
hmsBusyAvg = 225.e-6
plt.figure('HMS Live Time')
plt.subplot(2, 1, 1)

plt.errorbar(hdd['trigRateMean'], hdd['tlt'], yerr=hdd['tltErr'], fmt='bo', mfc='none', ms=10, mew=2, label=r'$\mathrm{LT_{E}}$')
plt.errorbar(hdd['trigRateMean'], hdd['clt'], yerr=hdd['cltErr'], fmt='g^', mfc='none', ms=10, mew=2, label=r'$\mathrm{CLT_{A}}$')
# plt.errorbar(hdd['trigRateMean'], hdd['elt'], yerr=hdd['eltErr'], fmt='rs', mfc='none', ms=10, mew=2, label='ELT')
# plt.errorbar(hdd['trigRateMean'], hdd['cltEt'], yerr=hdd['cltEtErr'], fmt='rs', mfc='none', ms=10, mew=2, label=r'$\mathrm{CLT_{A}}$ (Event Type)')
# plt.errorbar(hdd['trigRateMean'], hdd['eltEt'], yerr=hdd['eltEtErr'], fmt='rs', mfc='none', ms=10, mew=2, label='ELT (Event Type)')
plt.errorbar(hdd['trigRateMean'], hdd['cltNoEdtm'], yerr=hdd['cltNoEdtmErr'], fmt='rs', mfc='none', ms=10, mew=2, label=r'$\mathrm{CLT_{P}}$')
# plt.errorbar(hdd['trigRateMean'], hdd['eltNoEdtm'], yerr=hdd['eltNoEdtmErr'], fmt='rs', mfc='none', ms=10, mew=2, label='ELT (No EDTM)')

plt.hlines(100, 0, 7500, colors='y', linestyles='-.', label='100% Live')
plt.plot(hmsRate, lt(hmsRate, hmsBusyMin), 'm--', label='Busy = 200 us')
plt.plot(hmsRate, lt(hmsRate, hmsBusyMax), 'k--', label='Busy = 250 us')

plt.xlim(0, 7500)
plt.ylim(5, 105)
plt.xlabel('Trigger Rate (Hz)')
plt.ylabel('Live Time (%)')
plt.title('HMS Live Time')
plt.legend(loc='best', fancybox='True', numpoints=1)
plt.subplot(2, 1, 2)

# plt.errorbar(hdd['trigRateMean'], np.asarray(hdd['tlt']) - np.asarray(hdd['clt']), yerr = calcSumErr(np.asarray(hdd['tltErr']), np.asarray(hdd['cltErr'])), 
#              fmt='cd', mfc='none', ms=10, mew=2, label=r'$\mathrm{LT_{E} - CLT_{A}}$')
# plt.errorbar(hdd['trigRateMean'], np.asarray(hdd['clt']) - np.asarray(hdd['cltEt']), yerr = calcSumErr(np.asarray(hdd['cltErr']), np.asarray(hdd['cltEtErr'])), 
#              fmt='cd', mfc='none', ms=10, mew=2, label=r'$\mathrm{CLT_{A} - CLT_{A}}$ (Event Type)')
plt.errorbar(hdd['trigRateMean'], np.asarray(hdd['tlt']) - np.asarray(hdd['cltNoEdtm']), yerr = calcSumErr(np.asarray(hdd['tltErr']), np.asarray(hdd['cltNoEdtmErr'])), 
             fmt='cd', mfc='none', ms=10, mew=2, label=r'$\mathrm{LT_{E} - CLT_{P}}$')
plt.errorbar(hdd['trigRateMean'], np.asarray(hdd['clt']) - np.asarray(hdd['cltNoEdtm']), yerr = calcSumErr(np.asarray(hdd['cltErr']), np.asarray(hdd['cltNoEdtmErr'])), 
             fmt='g*', mfc='none', ms=10, mew=2, label=r'$\mathrm{CLT_{A} - CLT_{P}}$')

# plt.plot(hmsRate, 100.*(ltClock(hmsBusyMin, hmsRate, edtmRate) - ltPoisson(hmsBusyMin, hmsRate, edtmRate)), 'g--', label='EDTM Non-Poissonian Bias (150 us Busy)')
# plt.plot(hmsRate, 100.*(ltClock(hmsBusyMax, hmsRate, edtmRate) - ltPoisson(hmsBusyMax, hmsRate, edtmRate)), 'm--', label='EDTM Non-Poissonian Bias (300 us Busy)')
# plt.plot(hmsRate, 100.*(ltClock(hmsBusyAvg, hmsRate, edtmRate) - ltPoisson(hmsBusyAvg, hmsRate, edtmRate)), 'r--', label='EDTM Non-Poissonian Bias (225 us Busy)')

# plt.plot(hmsRate, 100.*linDiff(hmsBusyMin, hmsRate, edtmRate), 'c--', label='EDTM Linear Bias (150 us Busy)')
# plt.plot(hmsRate, 100.*linDiff(hmsBusyMax, hmsRate, edtmRate), 'y--', label='EDTM Linear Bias (300 us Busy)')
# plt.plot(hmsRate, 100.*linDiff(hmsBusyAvg, hmsRate, edtmRate), 'g--', label='EDTM Linear Bias (225 us Busy)')

plt.hlines(0, 0, 7500, colors='y', linestyles='-.', label = 'Residiual = 0%')
plt.vlines(100, -0.5, 3.5, colors='g', linestyles='-.', label='EDTM Rate (100 Hz)')

hlfParams, hlfCov =  optimization.curve_fit(linFit, hdd['trigRateMean'][2:7], np.asarray(hdd['tlt'][2:7]) - np.asarray(hdd['cltNoEdtm'][2:7]), sigma = calcSumErr(np.asarray(hdd['tltErr'][2:7]), np.asarray(hdd['cltNoEdtmErr'][2:7])))
plt.plot(hmsRate, linFit(hmsRate, hlfParams[0], hlfParams[1]), 'k-.', label = 'Linear Fit')

hqfParams, hqfCov =  optimization.curve_fit(quadFit, hdd['trigRateMean'], np.asarray(hdd['clt']) - np.asarray(hdd['cltNoEdtm']), sigma = calcSumErr(np.asarray(hdd['cltErr']), np.asarray(hdd['cltNoEdtmErr'])))
plt.plot(hmsRate, quadFit(hmsRate, hqfParams[0], hqfParams[1]), 'm-.', label = 'Second Order Fit')

plt.xlim(0, 7500)
plt.ylim(-0.5, 3.5)
# plt.ylim(-0.5, 0.5)
plt.xlabel('Trigger Rate (Hz)')
plt.ylabel('Residuals (%)')
plt.legend(loc='best', fancybox='True', numpoints=1)
plt.savefig('hmsLiveTime.pdf')

# =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

shmsRate    = np.linspace(0, 2250, 2250)
shmsBusyMin = 200.e-6
shmsBusyMax = 250.e-6
shmsBusyAvg = 225.e-6

plt.figure('SHMS Live Time')
plt.subplot(2, 1, 1)
plt.errorbar(pdd['trigRateMean'], pdd['tlt'], yerr=pdd['tltErr'], fmt='bo', mfc='none', ms=10, mew=2, label=r'$\mathrm{LT_{E}}$')
plt.errorbar(pdd['trigRateMean'], pdd['clt'], yerr=pdd['cltErr'], fmt='g^', mfc='none', ms=10, mew=2, label=r'$\mathrm{CLT_{A}}$')
# plt.errorbar(pdd['trigRateMean'], pdd['elt'], yerr=pdd['eltErr'], fmt='rs', mfc='none', ms=10, mew=2, label='ELT')
# plt.errorbar(pdd['trigRateMean'], pdd['cltEt'], yerr=pdd['cltEtErr'], fmt='rs', mfc='none', ms=10, mew=2, label=r'$\mathrm{CLT_{A}}$ (Event Type)')
# plt.errorbar(pdd['trigRateMean'], pdd['eltEt'], yerr=pdd['eltEtErr'], fmt='rs', mfc='none', ms=10, mew=2, label='ELT (Event Type)')
plt.errorbar(pdd['trigRateMean'], pdd['cltNoEdtm'], yerr=pdd['cltNoEdtmErr'], fmt='rs', mfc='none', ms=10, mew=2, label=r'$\mathrm{CLT_{P}}$')
# plt.errorbar(pdd['trigRateMean'], pdd['eltNoEdtm'], yerr=pdd['eltNoEdtmErr'], fmt='rs', mfc='none', ms=10, mew=2, label='ELT (No EDTM)')

plt.hlines(100, 0, 2250, colors='y', linestyles='-.', label='100% Live')
plt.plot(shmsRate, lt(shmsRate, shmsBusyMin), 'm--', label='Busy = 200 us')
plt.plot(shmsRate, lt(shmsRate, shmsBusyMax), 'k--', label='Busy = 250 us')

plt.xlim(0, 2250)
plt.ylim(60, 105)
# plt.xlabel('Trigger Rate (Hz)')
plt.ylabel('Live Time (%)')
plt.title('SHMS Live Time')
plt.legend(loc='best', fancybox='True', numpoints=1)
plt.subplot(2, 1, 2)

# plt.errorbar(pdd['trigRateMean'], np.asarray(pdd['tlt']) - np.asarray(pdd['clt']), yerr = calcSumErr(np.asarray(pdd['tltErr']), np.asarray(pdd['cltErr'])), 
#              fmt='cd', mfc='none', ms=10, mew=2, label=r'$\mathrm{LT_{E} - CLT_{A}}$')
# plt.errorbar(pdd['trigRateMean'], np.asarray(pdd['clt']) - np.asarray(pdd['cltEt']), yerr = calcSumErr(np.asarray(pdd['cltErr']), np.asarray(pdd['cltEtErr'])), 
#              fmt='cd', mfc='none', ms=10, mew=2, label=r'$\mathrm{CLT_{A} - CLT_{A}}$ (Event Type)')
plt.errorbar(pdd['trigRateMean'], np.asarray(pdd['tlt']) - np.asarray(pdd['cltNoEdtm']), yerr = calcSumErr(np.asarray(pdd['tltErr']), np.asarray(pdd['cltNoEdtmErr'])), 
             fmt='cd', mfc='none', ms=10, mew=2, label=r'$\mathrm{LT_{E} - CLT_{P}}$')
plt.errorbar(pdd['trigRateMean'], np.asarray(pdd['clt']) - np.asarray(pdd['cltNoEdtm']), yerr = calcSumErr(np.asarray(pdd['cltErr']), np.asarray(pdd['cltNoEdtmErr'])), 
             fmt='g*', mfc='none', ms=10, mew=2, label=r'$\mathrm{CLT_{A} - CLT_{P}}$')

# plt.plot(shmsRate, 100.*(ltClock(shmsBusyMin, shmsRate, edtmRate) - ltPoisson(shmsBusyMin, shmsRate, edtmRate)), 'g--', label='EDTM Non-Poissonian Bias (150 us Busy)')
# plt.plot(shmsRate, 100.*(ltClock(shmsBusyMax, shmsRate, edtmRate) - ltPoisson(shmsBusyMax, shmsRate, edtmRate)), 'm--', label='EDTM Non-Poissonian Bias (300 us Busy)')
# plt.plot(shmsRate, 100.*(ltClock(shmsBusyAvg, shmsRate, edtmRate) - ltPoisson(shmsBusyAvg, shmsRate, edtmRate)), 'r--', label='EDTM Non-Poissonian Bias (225 us Busy)')

# plt.plot(shmsRate, 100.*linDiff(shmsBusyMin, shmsRate, edtmRate), 'c--', label='EDTM Linear Bias (150 us Busy)')
# plt.plot(shmsRate, 100.*linDiff(shmsBusyMax, shmsRate, edtmRate), 'y--', label='EDTM Linear Bias (300 us Busy)')
# plt.plot(shmsRate, 100.*linDiff(shmsBusyAvg, shmsRate, edtmRate), 'g--', label='EDTM Linear Bias (225 us Busy)')

plt.hlines(0, 0, 2250, colors='y', linestyles='-.', label = 'Residiual = 0%')
plt.vlines(100, -0.5, 3.5, colors='g', linestyles='-.', label='EDTM Rate (100 Hz)')

plfParams, plfCov =  optimization.curve_fit(linFit, pdd['trigRateMean'], np.asarray(pdd['tlt']) - np.asarray(pdd['cltNoEdtm']), sigma = calcSumErr(np.asarray(pdd['tltErr']), np.asarray(pdd['cltNoEdtmErr'])))
plt.plot(shmsRate, linFit(shmsRate, plfParams[0], plfParams[1]), 'k-.', label = 'Linear Fit')

pqfParams, pqfCov =  optimization.curve_fit(quadFit, pdd['trigRateMean'], np.asarray(pdd['clt']) - np.asarray(pdd['cltNoEdtm']), sigma = calcSumErr(np.asarray(pdd['cltErr']), np.asarray(pdd['cltNoEdtmErr'])))
plt.plot(hmsRate, quadFit(hmsRate, pqfParams[0], pqfParams[1]), 'm-.', label = 'Second Order Fit')

plt.xlim(0, 2250)
plt.ylim(-0.5, 3.5)
# plt.ylim(-0.5, 0.5)
plt.xlabel('Trigger Rate (Hz)')
plt.ylabel('Residuals (%)')
plt.legend(loc='best', fancybox='True', numpoints=1)
plt.savefig('shmsLiveTime.pdf')

plt.show()
