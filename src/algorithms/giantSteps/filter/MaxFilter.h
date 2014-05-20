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

using namespace std;

namespace essentia {
namespace standard {

class MaxFilter : public Algorithm {

 private:
 
  Input< vector<Real>  > _array;
  Output< vector<Real> > _filtered;


  
  	int _width;

 public:
  MaxFilter() {
    declareInput(_array, "signal", "signal to be filtered");
    declareOutput(_filtered, "signal", "filtered output ");

    
  }

  ~MaxFilter() {

  }

  void declareParameters() {
    declareParameter("width", "window size for max filter : has to be odd as the window is centered on sample", "[3,inf)", 3);

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

#include "streamingalgorithmwrapper.h"


namespace essentia {
namespace streaming {

class MaxFilter : public StreamingAlgorithmWrapper {

 protected:
  Sink<vector<Real> > _array;
  Source<vector<Real> > _filtered;


 public:
  MaxFilter(){
    declareAlgorithm("MaxFilter");
    declareInput(_array,TOKEN, "signal");
    declareOutput(_filtered,TOKEN, "signal");

  }


};

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_MaxFilter_H
