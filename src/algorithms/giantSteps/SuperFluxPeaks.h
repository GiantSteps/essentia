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

#ifndef ESSENTIA_SuperFluxPeaks_H
#define ESSENTIA_SuperFluxPeaks_H

#include "algorithmfactory.h"

namespace essentia {
namespace standard {

class SuperFluxPeaks : public Algorithm {

 private:
  Input<std::vector<Real> > _signal;
  Output<std::vector<Real> > _peaks;
 
Algorithm* _movAvg;
Algorithm* _maxf;
  
 int _pre_avg;
int _pre_max;
Real _combine;
Real _threshold;


  int hopSize;
  Real frameRate;





 public:
  SuperFluxPeaks() {
    declareInput(_signal, "novelty", "the input novelty");
	declareOutput(_peaks, "peaks", "the input novelty");
    _movAvg = AlgorithmFactory::create("MovingAverage");
    _maxf = AlgorithmFactory::create("MaxFilter");
    
  }

  ~SuperFluxPeaks() {

  }

  void declareParameters() {
	declareParameter("frameRate", "frameRate", "(0,inf)", 172);
    declareParameter("threshold", "threshold for peak-picking", "(0,inf)", 1.25);
	declareParameter("combine", "ms for onset combination", "(0,inf)", 30);
    declareParameter("pre_avg", "use N miliseconds past information for moving average", "(0,inf)", 100);
	declareParameter("pre_max", "use N miliseconds past information for moving maximum", "(0,inf)", 30);


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

class SuperFluxPeaks : public AlgorithmComposite {

 protected:
  SinkProxy<Real> _signal;
  SourceProxy<Real> _peaks;


  Pool _pool;
  Algorithm* _poolStorage;
  standard::Algorithm * _SuperFluxPeaks;


 public:
  SuperFluxPeaks();
  ~SuperFluxPeaks();

  void declareParameters() {

    declareParameter("threshold", "threshold for peak-picking", "(0,inf)", 1.25);
	declareParameter("combine", "ms for onset combination", "(0,inf)", 30);
    declareParameter("pre_avg", "use N miliseconds past information for moving average", "(0,inf)", 100);
	declareParameter("pre_max", "use N miliseconds past information for moving maximum", "(0,inf)", 30);
  }

  void configure() {
    _SuperFluxPeaks->configure(
                                     INHERIT("threshold"),
                                     INHERIT("combine"),
                                     INHERIT("pre_avg"),
                                     INHERIT("combine")
                                     
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

#endif // ESSENTIA_SuperFluxPeaks_H
