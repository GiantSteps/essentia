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

#include "SuperFluxNovelty.h"
#include "essentiamath.h"

using namespace std;

namespace essentia {
namespace standard {


const char* SuperFluxNovelty::name = "SuperFluxNovelty";
const char* SuperFluxNovelty::description = DOC("Superflux algorithm : Maximum filter and differentiation for onset detection robust again vibrato"
"Input : filterbank like spectrogram");


void SuperFluxNovelty::configure() {
 	
 	
 	//width has to be odd, then local binW represent half the width
    _binW = parameter("binWidth").toInt();
    if(_binW%2==0)_binW++;
    _maxf->configure("width",_binW);
    
    
	_binW= int((_binW-1)/2); 
    
	_frameWi = parameter("frameWidth").toInt();

	_pos = parameter("Positive").toBool();

	
	




}


void SuperFluxNovelty::compute() {

  	const TNT::Array2D<Real>& bands = _bands.get();
  	
	vector<Real>& diffs = _diffs.get();

  int nFrames = bands.dim1();
  int nBands= bands.dim2();
  if(!nFrames || !nBands){
  throw EssentiaException("SuperFluxNovelty : empty bands or frames");
  }
	diffs.resize(nFrames);
	vector<Real> maxsBuffer(nBands,0);


Real cur_diff;

for (int i = _frameWi ; i< nFrames;i++){

	diffs[i]=0;
	vector<Real> tmpBuffer(bands[i-_frameWi],bands[i-_frameWi]+nBands);
	_maxf->input("signal").set(tmpBuffer);
	_maxf->output("signal").set(maxsBuffer);
	_maxf->compute();
	
	for (int j = 0;j<nBands;j++){
		 cur_diff= bands[i][j]-maxsBuffer[j];
		if(!(_pos && cur_diff<0)){diffs[i] +=cur_diff ; }
		
	}
	
	//diffs[i]/=(nBands-2.*_binW);
}
return;
}







void SuperFluxNovelty::reset() {
  Algorithm::reset();

}


// TODO in the case of lower accuracy in evaluation
// implement post-processing steps for methods in OnsetDetection, which required it
// wrapping the OnsetDetection algo
// - smoothing?
// - etc., whatever was requiered in original matlab implementations

} // namespace standard
} // namespace essentia


#include "poolstorage.h"
#include "algorithmfactory.h"

namespace essentia {
namespace streaming {

const char* SuperFluxNovelty::name = standard::SuperFluxNovelty::name;
const char* SuperFluxNovelty::description = standard::SuperFluxNovelty::description;

SuperFluxNovelty::SuperFluxNovelty() : AlgorithmComposite() {

  _SuperFluxNovelty = standard::AlgorithmFactory::create("SuperFluxNovelty");
    declareInput(_bands, "bands", "the input bands spectrogram");
    declareOutput(_diffs, "Differences", "SuperFluxNoveltyd input");
}

SuperFluxNovelty::~SuperFluxNovelty() {
  delete _SuperFluxNovelty;

}

void SuperFluxNovelty::reset() {
  AlgorithmComposite::reset();
  _SuperFluxNovelty->reset();
}

AlgorithmStatus SuperFluxNovelty::process() {
  if (!shouldStop()) return PASS;

  _SuperFluxNovelty->compute();


  return FINISHED;
}

} // namespace streaming
} // namespace essentia
