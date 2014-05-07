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

#include "SuperFluxOnsets.h"
#include <complex>
#include <limits>
#include "essentiamath.h"

using namespace std;

namespace essentia {
namespace standard {


const char* SuperFluxOnsets::name = "SuperFluxOnsets";
const char* SuperFluxOnsets::description = DOC("Remove n First Max FFT");


void SuperFluxOnsets::configure() {
  frameSize = parameter("frameSize").toInt();
  hopSize = frameSize/4.;

  // Frames are cut starting from zero as in the paper and consistently with
  // OnsetRate algorithm
  _frameCutter->configure("frameSize", frameSize,
                          "hopSize", hopSize,
                          "startFromZero", true);

  _windowing->configure("size", frameSize,
                        "zeroPadding", 0,
                        "type", "hann");

// 
     _numberFFTBins = int(frameSize)/2 + 1;
     _phase.resize(_numberFFTBins);
// 
    ratio = parameter("ratio").toReal();



}


void SuperFluxOnsets::compute() {
  const vector<Real>& signal = _signal.get();
vector<Real>& Harmonic = _Harmonic.get();
vector<Real>& Noise = _Noise.get();
  if (signal.empty()) {
Harmonic.resize(0);
Noise.resize(0);
    return;
  }
  
	Harmonic.resize(signal.size());
	Noise.resize(signal.size());
  _frameCutter->input("signal").set(signal);
  _frameCutter->output("frame").set(_frame);

  _windowing->input("frame").set(_frame);
  _windowing->output("frame").set(_frameWindowed);





  vector<complex<Real> > frameFFT;
  _fft->input("frame").set(_frameWindowed);
  _fft->output("fft").set(frameFFT);

  vector<Real> spectrum;
  vector<Real> phase;
  _c2p->input("complex").set(frameFFT);
  _c2p->output("magnitude").set(_hspectrum);
  _c2p->output("phase").set(_phase);
  
  
  _p2c->input("phase").set(_phase);
  _p2c->input("magnitude").set(_hspectrum);
  _p2c->output("complex").set(frameFFT);
  
  _ifft->input("fft").set(frameFFT);
  _ifft->output("frame").set(_frame);
  

	vector<Real> _spectrum_s (_numberFFTBins,0);
	
  size_t numberFrames=0;

  while (true) {
    // get a frame
    _frameCutter->compute();

    if (!_frame.size()) {
      break;
    }

    _windowing->compute();
    _fft->compute();
    _c2p->compute();
	
	_spectrum_s = _hspectrum;
	sort(_spectrum_s.begin(),_spectrum_s.end());
	
	int idx = (1.-ratio)*_numberFFTBins;
	Real minT= _spectrum_s[idx];
    // supress maxs
    for (int i=0; i<_numberFFTBins; ++i) {
		if(_hspectrum[i]>minT)_hspectrum[i]=0;
    }
    
    _p2c->compute();
    _ifft->compute();
    
    for (int i =  0; i < frameSize ; i++){
    
	Harmonic[numberFrames*hopSize + i]+= _frame[i];
	Noise[numberFrames*hopSize + i]+= _frame[i];
}
    numberFrames += 1;
    
    
  }


  if (!numberFrames) {
    return;
  }



  
}


void SuperFluxOnsets::reset() {
  Algorithm::reset();
  if (_frameCutter) _frameCutter->reset();
  if (_windowing) _windowing->reset();
  if (_fft) _fft->reset();
  if (_ifft) _ifft->reset();
  if (_c2p) _c2p->reset();
  if (_p2c) _p2c->reset();
}


// TODO in the case of lower accuracy in evaluation
// implement post-processing steps for methods in OnsetDetection, which required it
// wrapping the OnsetDetection algo
// - smoothing?
// - etc., whatever was requiered in original matlab implementations

} // namespace standard
} // namespace essentia


#include "poolstorage.h"
#include "algorithmfactory.h"

namespace essentia {
namespace streaming {

const char* SuperFluxOnsets::name = standard::SuperFluxOnsets::name;
const char* SuperFluxOnsets::description = standard::SuperFluxOnsets::description;

SuperFluxOnsets::SuperFluxOnsets() : AlgorithmComposite() {

  _SuperFluxOnsets = standard::AlgorithmFactory::create("SuperFluxOnsets");
  _poolStorage = new PoolStorage<Real>(&_pool, "internal.signal");

  declareInput(_signal, 1, "signal", "the input signal");   // 1
  declareOutput(_Harmonic, 0, "Harmonic", "Harmonic part"); // 0
	declareOutput(_Noise, 0, "Noise", "Noisy part");
  _signal >> _poolStorage->input("data"); // attach input proxy

  // NB: We want to have the same output stream type as in OnsetDetection for
  // consistency. We need to increase buffer size of the output because the
  // algorithm works on the level of entire track and we need to push all values
  // in the output source at once.
  _Harmonic.setBufferType(BufferUsage::forLargeAudioStream);
    _Noise.setBufferType(BufferUsage::forLargeAudioStream);
}

SuperFluxOnsets::~SuperFluxOnsets() {
  delete _SuperFluxOnsets;
  delete _poolStorage;
}

void SuperFluxOnsets::reset() {
  AlgorithmComposite::reset();
  _SuperFluxOnsets->reset();
}

AlgorithmStatus SuperFluxOnsets::process() {
  if (!shouldStop()) return PASS;

 
  //const vector<Real>& signal = _pool.value<vector<Real> >("internal.signal");

  _SuperFluxOnsets->input("signal").set(_pool.value<vector<Real> >("internal.signal"));
  _SuperFluxOnsets->output("Harmonic").set(_Harmonic);
  _SuperFluxOnsets->output("Noise").set(_Noise);
  _SuperFluxOnsets->compute();


  return FINISHED;
}

} // namespace streaming
} // namespace essentia
