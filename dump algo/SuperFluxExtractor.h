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

#ifndef ESSENTIA_SUPERFLUXEXTRACTOR_H
#define ESSENTIA_SUPERFLUXEXTRACTOR_H

#include "algorithmfactory.h"

namespace essentia {
namespace standard {

class SuperFluxExtractor : public Algorithm {

 private:
 
  Input<std::vector<Real>  > _audio;
  Output<Real> _isOnset;


  
  	int _binW;
  	int _frameWi;
	bool _pos;
	
	vector<Real> tmpSpec;
	

std::queue<vector<Real> > circBuffer;
int circIdx;

	Algorithm* _specF;
	Algorithm* _triF;
	Algorithm* _sfN;
	Algorithm* _sfP;


 public:
  SuperFluxExtractor() {
    declareInput(_bands, "bands", "the input bands spectrogram");
    declareOutput(_diffs, "Differences", "SuperFluxExtractord input");
	_sfN = AlgorithmFactory::create("SuperFluxNovelty");
	_sfP = AlgorithmFactory::create("SuperFluxPeaks");
	_specF = AlgorithmFactory::create("Spectrum");
	_triF = AlgorithmFactory::create("TriangularBands");
  }

  ~SuperFluxExtractor() {

  }

  void declareParameters() {
  
    declareParameter("sampleRate", "SampleRate of audio signal", "(0,inf)", 44100);
    declareParameter("windowSize", "windowSize", "(0,inf)", 2048);
    
    //novelty
    declareParameter("binWidth", "height(n of frequency bins) of the SuperFluxExtractorFilter", "[3,inf)", 3);
	declareParameter("frameWidth", "number of frame for differentiation", "(0,inf)", 2);
	
	//peaks
	declareParameter("frameRate", "frameRate", "(0,inf)", 172);
    declareParameter("threshold", "threshold for peak-picking", "(0,inf)", 1.25);
	declareParameter("combine", "ms for onset combination", "(0,inf)", 30);
    declareParameter("pre_avg", "use N miliseconds past information for moving average", "(0,inf)", 100);
	declareParameter("pre_max", "use N miliseconds past information for moving maximum", "(0,inf)", 30);
	
}

  void reset();
  void configure();
  void compute();


  static const char* name;
  static const char* version;
  static const char* description;
};

} // namespace standard
} // namespace essentia

// #include "streamingalgorithmcomposite.h"
// #include "pool.h"
// 
// namespace essentia {
// namespace streaming {
// 
// class SuperFluxExtractor : public AlgorithmComposite {
// 
//  protected:
//   SinkProxy<Real> _bands;
//   Source<Real> _diffs;
// 
// 
//   Pool _pool;
//   Algorithm* _poolStorage;
//   standard::Algorithm * _SuperFluxExtractor;
// 
// 
//  public:
//   SuperFluxExtractor();
//   ~SuperFluxExtractor();
// 
//   void declareParameters() {
//     declareParameter("binWidth", "height(n of frequency bins) of the SuperFluxExtractorFilter", "[3,inf)", 3);
// 	declareParameter("frameWidth", "number of frame for differentiation", "(0,inf)", 5);
//   }
// 
//   void configure() {
//     _SuperFluxExtractor->configure(
//                                      INHERIT("frameWidth"),
//                                      INHERIT("binWidth")
//                                      );
//   }
// 
//   void declareProcessOrder() {
//     declareProcessStep(SingleShot(_poolStorage));
//     declareProcessStep(SingleShot(this));
//   }
// 
//   AlgorithmStatus process();
//   void reset();
// 
//   static const char* name;
//   static const char* description;
// 
// };
// 
// } // namespace streaming
// } // namespace essentia
// 
// #endif // ESSENTIA_SuperFluxExtractor_H
