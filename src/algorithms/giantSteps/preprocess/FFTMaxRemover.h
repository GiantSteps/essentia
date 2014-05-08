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

#ifndef ESSENTIA_FFTMAXREMOVER_H
#define ESSENTIA_FFTMAXREMOVER_H

#include "algorithmfactory.h"

namespace essentia {
namespace standard {

class FFTMaxRemover : public Algorithm {

 private:
  Input<std::vector<Real> > _signal;
  Output<std::vector<Real> > _Harmonic;
  Output<std::vector<Real> > _Noise;
 
  Algorithm* _frameCutter;
  Algorithm* _windowing;
  Algorithm* _fft;
  Algorithm* _ifft;
  Algorithm* _c2p;
  Algorithm* _p2c;
  
  
  float ratio;

	
  std::vector<Real> _frame;
  std::vector<Real> _frameWindowed;
  std::vector<Real> _hspectrum;
  std::vector<Real> _lspectrum;
  
  int _minFrequencyBin;
  int _maxFrequencyBin;
  int _numberFFTBins;
  
  int hopSize;
  int frameSize;



  std::vector<Real> _phase;




 public:
  FFTMaxRemover() {
    declareInput(_signal, "signal", "the input signal");
    declareOutput(_Harmonic, "Harmonic", "Harmonic part");
	declareOutput(_Noise, "Noise", "Noisy part");
    _frameCutter = AlgorithmFactory::create("FrameCutter");
    _windowing = AlgorithmFactory::create("Windowing");
    _fft = AlgorithmFactory::create("FFT");
    _ifft = AlgorithmFactory::create("IFFT");
    _c2p = AlgorithmFactory::create("CartesianToPolar");
    _p2c = AlgorithmFactory::create("PolarToCartesian");
    
  }

  ~FFTMaxRemover() {
    if (_frameCutter) delete _frameCutter;
    if (_windowing) delete _windowing;
  }

  void declareParameters() {
    declareParameter("frameSize", "the frame size for computing onset detection function", "(0,inf)", 2048);
	declareParameter("ratio", "the ratio of max bins deleted", "(0,1)", .2);
}

  void reset();
  void configure();
  void compute();

  void computeInfoGain();
  void computeBeatEmphasis();

  static const char* name;
  static const char* version;
  static const char* description;
};

} // namespace standard
} // namespace essentia

#include "streamingalgorithmcomposite.h"
#include "pool.h"

namespace essentia {
namespace streaming {

class FFTMaxRemover : public AlgorithmComposite {

 protected:
  SinkProxy<Real> _signal;
  Source<Real> _Harmonic;
  Source<Real> _Noise;

  Pool _pool;
  Algorithm* _poolStorage;
  standard::Algorithm * _FFTMaxRemover;


 public:
  FFTMaxRemover();
  ~FFTMaxRemover();

  void declareParameters() {

    declareParameter("frameSize", "the frame size for computing onset detection function", "(0,inf)", 2048);
declareParameter("ratio", "the ratio of max bins deleted", "(0,1)", .2);
  }

  void configure() {
    _FFTMaxRemover->configure(
                                     INHERIT("frameSize"),
                                     INHERIT("ratio")
                                     );
  }

  void declareProcessOrder() {
    declareProcessStep(SingleShot(_poolStorage));
    declareProcessStep(SingleShot(this));
  }

  AlgorithmStatus process();
  void reset();

  static const char* name;
  static const char* description;

};

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_FFTMaxRemover_H
