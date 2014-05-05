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



 public:
  waveshaper()   {
    declareInput(_signal, "array", "the array to be shaped");
    declareOutput(_signalout, "shaped_signal", "shaped");

  }

  void declareParameters() {

	declareParameter("p1x","coordinate","(-inf,inf)",0);
	declareParameter("p1y","coordinate","(-inf,inf)",0);
	declareParameter("p2x","coordinate","(-inf,inf)",0.3);
	declareParameter("p2y","coordinate","(-inf,inf)",0.3);
	declareParameter("p3x","coordinate","(-inf,inf)",0.6);
	declareParameter("p3y","coordinate","(-inf,inf)",0.6);
	declareParameter("p4x","coordinate","(-inf,inf)",1);
	declareParameter("p4y","coordinate","(-inf,inf)",1);
	declareParameter("symmetric","symetric mode","{true,false}", true);
	declareParameter("normalize","normalisation mode","{true,false}", true);
  }

  void configure();
  void compute();


  static const char* name;
  static const char* description;
  
  
  private:
	Real _p1x;
	Real  _p2x;
	Real  _p3x;
	Real  _p4x;
	Real  _p1y;
	Real  _p2y;
	Real  _p3y;
	Real  _p4y;
	double _c1;
	double _c2;
	double _c3;
	bool _symmetric;
	bool _normalize;

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
