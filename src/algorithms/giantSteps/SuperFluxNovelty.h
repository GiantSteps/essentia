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

#ifndef ESSENTIA_SUPERFLUXNOVELTY_H
#define ESSENTIA_SUPERFLUXNOVELTY_H

#include "algorithmfactory.h"

namespace essentia {
namespace standard {

class SuperFluxNovelty : public Algorithm {

 private:
 
  Input<TNT::Array2D<Real>  > _bands;
  Output<std::vector<Real> > _diffs;


  
  	int _binW;
  	int _frameWi;
	bool _pos;


Algorithm* _maxf;


 public:
  SuperFluxNovelty() {
    declareInput(_bands, "bands", "the input bands spectrogram");
    declareOutput(_diffs, "Differences", "SuperFluxNoveltyd input");

    
  }

  ~SuperFluxNovelty() {

  }

  void declareParameters() {
    declareParameter("binWidth", "height(n of frequency bins) of the SuperFluxNoveltyFilter", "(0,inf)", 3);
	declareParameter("frameWidth", "number of frame for differentiation", "(0,inf)", 2);
	declareParameter("Positive", "keep only positive ones", "{false,true}", true);
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

#include "streamingalgorithmcomposite.h"
#include "pool.h"

namespace essentia {
namespace streaming {

class SuperFluxNovelty : public AlgorithmComposite {

 protected:
  SinkProxy<Real> _bands;
  Source<Real> _diffs;


  Pool _pool;
  Algorithm* _poolStorage;
  standard::Algorithm * _SuperFluxNovelty;


 public:
  SuperFluxNovelty();
  ~SuperFluxNovelty();

  void declareParameters() {
    declareParameter("binWidth", "height(n of frequency bins) of the SuperFluxNoveltyFilter", "(0,inf)", 3);
	declareParameter("frameWidth", "number of frame for differentiation", "(0,inf)", 5);
  }

  void configure() {
    _SuperFluxNovelty->configure(
                                     INHERIT("frameWidth"),
                                     INHERIT("binWidth")
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

#endif // ESSENTIA_SuperFluxNovelty_H
