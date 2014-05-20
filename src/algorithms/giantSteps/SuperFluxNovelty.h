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
using namespace std;

namespace essentia {
namespace standard {

class SuperFluxNovelty : public Algorithm {

 private:
 
  Input<std::vector< std::vector<Real> >  > _bands;
  Output<std::vector<Real> > _diffs;


  
  	int _binW;
  	int _frameWi;
	bool _pos;


	Algorithm* _maxf;


 public:
  SuperFluxNovelty() {
    declareInput(_bands, "bands", "the input bands spectrogram");
    declareOutput(_diffs, "Differences", "SuperFluxNoveltyd input");
	_maxf = AlgorithmFactory::create("MaxFilter");
    
  }

  ~SuperFluxNovelty() {

  }

  void declareParameters() {
    declareParameter("binWidth", "height(n of frequency bins) of the SuperFluxNoveltyFilter", "[3,inf)", 3);
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

#include "streamingalgorithmwrapper.h"
#include "streamingalgorithmcomposite.h"

namespace essentia {
namespace streaming {

class SuperFluxNovelty : public Algorithm {

 protected:
  Sink< std::vector<Real> > _bands;
  Source<vector< Real > > _diffs;
  
  
  essentia::standard::Algorithm* _algo;

int bufferSize=3;

 public:
  SuperFluxNovelty(){

    _algo = standard::AlgorithmFactory::create("SuperFluxNovelty");
    declareInput(_bands, bufferSize,1,"bands","the input bands spectrogram");
    declareOutput(_diffs,1,1,"Differences","SuperFluxNoveltyd input");

  }




  void declareParameters() {
    declareParameter("binWidth", "height(n of frequency bins) of the SuperFluxNoveltyFilter", "[3,inf)", 3);
	declareParameter("frameWidth", "number of frame for differentiation", "(0,inf)", 5);
  }

void configure(){
    _bands.setAcquireSize(_algo->parameter("frameWidth").toInt()+1);
    _bands.setReleaseSize(1);
    

    
   }

// 
  AlgorithmStatus process();
  void reset(){
  _algo->reset();
  };

  static const char* name;
  static const char* description;

};

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_SuperFluxNovelty_H
