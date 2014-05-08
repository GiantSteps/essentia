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

#ifndef ESSENTIA_waveshaper_H
#define ESSENTIA_waveshaper_H

#include "algorithmfactory.h"
#include <complex>


namespace essentia {
namespace standard {

class waveshaper : public Algorithm {

 private:
  Input<std::vector<Real> > _signal;
  Output<std::vector<Real> > _signalout;

	Algorithm* _splinef;



 public:
  waveshaper()   {
    declareInput(_signal, "array", "the array to be shaped");
    declareOutput(_signalout, "shaped_signal", "shaped");
    _splinef = AlgorithmFactory::create("Spline");
  }

  void declareParameters() {
	std::vector<Real> defaultP(2);
	defaultP[0]=0;
	defaultP[1]=1;

    declareParameter("xPoints", "the x-coordinates where data is specified (the points must be arranged in ascending order and cannot contain duplicates)", "",defaultP);
    declareParameter("yPoints", "the y-coordinates to be interpolated (i.e. the known data)", "",  defaultP);
	declareParameter("spline","spline mode","{true,false}", true);
	declareParameter("symmetric","symetric mode","{true,false}", true);
	declareParameter("normalize","normalisation mode","{true,false}", true);
  }

  void configure();
  void compute();


  static const char* name;
  static const char* description;
  
  
  private:
	std::vector<Real> _xpts,_ypts;

	bool _symmetric;
	bool _normalize;
	bool _spline;

};

} // namespace standard
} // namespace essentia

#include "streamingalgorithmwrapper.h"

namespace essentia {
namespace streaming {

class waveshaper : public StreamingAlgorithmWrapper {

 protected:
  Sink<std::vector<Real> > _signal;
  Source<std::vector<Real> > _signalout;

 public:
  waveshaper() {
    declareAlgorithm("waveshaper");
    declareInput(_signal, TOKEN, "array");
    declareOutput(_signalout, TOKEN, "shaped_signal");
  }
};

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_AUTOCORRELATION_H
