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

#ifndef ESSENTIA_Whitener_H
#define ESSENTIA_Whitener_H

#include "algorithmfactory.h"

namespace essentia {
namespace standard {

class Whitener : public Algorithm {

 private:
  Input<std::vector<Real> > _signal;
  Output<std::vector<Real> > _out;
 
  Algorithm* _frameCutter;
  Algorithm* _windowing;
  Algorithm* _fft;
  Algorithm* _ifft;
  Algorithm* _c2p;
  Algorithm* _p2c;
  Algorithm* _whiteningf;
  Algorithm* _peaksf;
  
  


	
  std::vector<Real> _frame;
  std::vector<Real> _frameWindowed;
  std::vector<Real> _hspectrum;
  std::vector<Real> _lspectrum;
  std::vector<Real> _peaks,_mags,_magsw,_whitened;
  
  int _minFrequencyBin;
  int _maxFrequencyBin;
  int _numberFFTBins;
  
  int hopSize;
  int frameSize;
  int sampleRate;



  std::vector<Real> _phase;




 public:
  Whitener() {
    declareInput(_signal, "signal", "the input signal");
    declareOutput(_out, "out", "out part");
    _frameCutter = AlgorithmFactory::create("FrameCutter");
    _windowing = AlgorithmFactory::create("Windowing");
    _fft = AlgorithmFactory::create("FFT");
    _ifft = AlgorithmFactory::create("IFFT");
    _c2p = AlgorithmFactory::create("CartesianToPolar");
    _p2c = AlgorithmFactory::create("PolarToCartesian");
    _whiteningf = AlgorithmFactory::create("SpectralWhitening");
    _peaksf = AlgorithmFactory::create("SpectralPeaks");

    
  }

  ~Whitener() {
    if (_frameCutter) delete _frameCutter;
    if (_windowing) delete _windowing;
  }

  void declareParameters() {
    declareParameter("frameSize", "the frame size for computing onset detection function", "(0,inf)", 2048);
    declareParameter("hopSize", "the hop size for computing onset detection function", "(0,inf)", 512);
    declareParameter("sampleRate", "the sampleRatefor computing onset detection function", "(0,inf)", 44100);
	declareParameter("peaksNumber", "the number of peaks to be whitened", "(0,inf)", 5);
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

class Whitener : public AlgorithmComposite {

 protected:
  SinkProxy<Real> _signal;
  Source<Real> _out;

  Pool _pool;
  Algorithm* _poolStorage;
  standard::Algorithm * _Whitener;


 public:
  Whitener();
  ~Whitener();

  void declareParameters() {

    declareParameter("frameSize", "the frame size for computing onset detection function", "(0,inf)", 2048);
        declareParameter("hopSize", "the hop size for computing onset detection function", "(0,inf)", 512);
    declareParameter("sampleRate", "the sampleRatefor computing onset detection function", "(0,inf)", 44100);
	declareParameter("peaksNumber", "the number of peaks to be whitened", "(0,inf)", 5);
  }

  void configure() {
    _Whitener->configure(
                                     INHERIT("frameSize"),
                                     INHERIT("peaksNumber"),
                                     INHERIT("sampleRate"),
                                     INHERIT("hopSize")
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

#endif // ESSENTIA_Whitener_H
