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

#ifndef ESSENTIA_gs_movingaverage_H
#define ESSENTIA_gs_movingaverage_H

#include "algorithmfactory.h"


namespace essentia {
namespace standard {

class gs_movingaverage : public Algorithm {

 protected:
  Input<std::vector<Real> > _x;
  Output<std::vector<Real> > _y;

  Algorithm* _filter;

 public:
  gs_movingaverage() {
    declareInput(_x, "signal", "the input audio signal");
    declareOutput(_y, "signal", "the filtered signal");

    _filter = AlgorithmFactory::create("IIR");
  }

  ~gs_movingaverage() {
    if (_filter) delete _filter;
  }

  void declareParameters() {
    declareParameter("size", "the size of the window [audio samples]", "(1,inf)", 6);
  }

  void reset() {
    _filter->reset();
  }

  void configure();
  void compute();

  static const char* name;
  static const char* description;

};

} // namespace standard
} // namespace essentia

#include "streamingalgorithmwrapper.h"

namespace essentia {
namespace streaming {

class gs_movingaverage : public StreamingAlgorithmWrapper {

 protected:
  Sink<Real> _x;
  Source<Real> _y;

  int preferredSize = 4096;

 public:
  gs_movingaverage() {
    declareAlgorithm("gs_movingaverage");
    Algorithm::declareInput(_x, preferredSize,1, "signal","input sig");
    Algorithm::declareOutput(_y, preferredSize,1, "signal","output sig");

    _y.setBufferType(BufferUsage::forAudioStream);
  }
};

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_gs_movingaverage_H
