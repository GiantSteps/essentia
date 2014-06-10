/*
 * Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Essentia
 *
 * Essentia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

#include "nsdf.h"
#include "essentiamath.h"

using namespace std;
using namespace essentia;
using namespace standard;

const char* Nsdf::name = "Nsdf";
const char* Nsdf::description = DOC(
"This algorithm returns the normalized Square Difference function of the signal.\n"
"It uses the version most commonly used in signal processing, which doesn't remove "
"the mean from the observations.\n"
"\n"
"References:\n"
"  [1] MPM\n"
"  \n\n"
"  \n"
"  ");


void Nsdf::configure() {
  string ntype = parameter("normalization").toString();

  if (ntype == "standard") {
    _unbiasedNormalization = false;
  }
  else if (ntype == "unbiased") {
    _unbiasedNormalization = true;
  }

  _fft->output("fft").set(_fftBuffer);
  _ifft->input("fft").set(_fftBuffer);
}

void Nsdf::compute() {

  const std::vector<Real>& signal = _signal.get();
  vector<Real>& correlation = _correlation.get();

  if (signal.size() == 0) {
    correlation.resize(0);
    return;
  }

  _fft->input("frame").set(_paddedSignal);
  _ifft->output("frame").set(_corr);

  int size = int(signal.size());
  int sizeFFT = int(nextPowerTwo(2*size));

  // formula to get the auto-correlation (in matlab) is:
  //  [M,N] = size(x)
  //  X = fft(x,2^nextpow2(2*M-1));
  //  c = ifft(abs(X).^2);

  // copy signal, and zero-pad it; use _corr as temporary array
  _paddedSignal.resize(sizeFFT);
  for (int i=0; i<size; i++) _paddedSignal[i] = signal[i];
  for (int i=size; i<sizeFFT; i++) _paddedSignal[i] = 0.0;

  // first compute fft
  _fft->compute();

  // take squared amplitude of the spectrum
  // (using magnitude would compute sqrt*sqrt)
  for (int i=0; i<int(_fftBuffer.size()); i++) {
    _fftBuffer[i] = complex<Real>(_fftBuffer[i].real() * _fftBuffer[i].real() +
                                  _fftBuffer[i].imag() * _fftBuffer[i].imag(),
                                  0.0); // squared amplitude -> complex part = 0
  }

  // step 3
  _ifft->compute();


  //nsdf optimisation see MPM smarter way to find pitch
  
  _m.resize(size);
  
  _m[0]=_corr[0]*2;
  for (int i = 1 ; i<signal.size();i++){
   _m[i]=_m[i-1] - (signal[i]*signal[i] +signal[size-i]*signal[size-i]);
  }
  
  
  correlation.resize(size);
  // copy results in output array, scaling on the go (normalizing the output of the IFFT)
  Real scale = 1.0 / sizeFFT;
  

  if (_unbiasedNormalization) {
    for (int i=0; i<size; i++) {
      correlation[i] = 2*_corr[i]/std::max(_m[i],0.000001f);// * scale / (size - i);
    }
  }
  else {
    for (int i=0; i<size; i++) {
      correlation[i] = 2*_corr[i]/std::max(_m[i],0.000001f);// * scale;
    }
  }
  
  
  
  

}
