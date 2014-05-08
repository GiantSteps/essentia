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

#ifndef ESSENTIA_MaxFilter_H
#define ESSENTIA_MaxFilter_H

#include "algorithmfactory.h"

namespace essentia {
namespace standard {

class MaxFilter : public Algorithm {

 private:
 
  Input<std::vector<Real>  > _array;
  Output<std::vector<Real> > _filtered;


  
  	int _width;






 public:
  MaxFilter() {
    declareInput(_array, "signal", "the input bands spectrogram");
    declareOutput(_filtered, "signal", "MaxFilterd input");

    
  }

  ~MaxFilter() {

  }

  void declareParameters() {
    declareParameter("width", "window size for max filter ", "(2,inf)", 3);

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

class MaxFilter : public AlgorithmComposite {

 protected:
  SinkProxy<Real> _array;
  Source<Real> _filtered;


  Pool _pool;
  Algorithm* _poolStorage;
  standard::Algorithm * _MaxFilter;


 public:
  MaxFilter();
  ~MaxFilter();

  void declareParameters() {
    declareParameter("width", "height(n of frequency bins) of the MaxFilterFilter", "(0,inf)", 3);

  }

  void configure() {
    _MaxFilter->configure(                                     
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

#endif // ESSENTIA_MaxFilter_H
