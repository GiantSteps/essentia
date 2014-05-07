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

#ifndef ESSENTIA_DILATEDIFF_H
#define ESSENTIA_DILATEDIFF_H

#include "algorithmfactory.h"

namespace essentia {
namespace standard {

class DilateDiff : public Algorithm {

 private:
 
  Input<TNT::Array2D<Real>  > _bands;
  Output<std::vector<Real> > _diffs;


  
  int _binW;
  int _frameW;






 public:
  DilateDiff() {
    declareInput(_bands, "bands", "the input bands spectrogram");
    declareOutput(_diffs, "Differences", "DilateDiffd input");

    
  }

  ~DilateDiff() {

  }

  void declareParameters() {
    declareParameter("binWidth", "height(n of frequency bins) of the DilateDiffFilter", "(0,inf)", 3);
	declareParameter("frameWidth", "number of frame for differentiation", "(0,inf)", 5);
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

class DilateDiff : public AlgorithmComposite {

 protected:
  SinkProxy<Real> _bands;
  Source<Real> _diffs;


  Pool _pool;
  Algorithm* _poolStorage;
  standard::Algorithm * _DilateDiff;


 public:
  DilateDiff();
  ~DilateDiff();

  void declareParameters() {
    declareParameter("binWidth", "height(n of frequency bins) of the DilateDiffFilter", "(0,inf)", 3);
	declareParameter("frameWidth", "number of frame for differentiation", "(0,inf)", 5);
  }

  void configure() {
    _DilateDiff->configure(
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

#endif // ESSENTIA_DilateDiff_H
