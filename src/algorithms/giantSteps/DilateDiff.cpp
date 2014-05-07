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

#include "DilateDiff.h"
#include <complex>
#include "essentiamath.h"

using namespace std;

namespace essentia {
namespace standard {


const char* DilateDiff::name = "DilateDiff";
const char* DilateDiff::description = DOC("Maximum filter");


void DilateDiff::configure() {
 	
    _binW = int(parameter("binWidth").toInt()/2);
	_frameW = parameter("frameWidth").toInt();








}


void DilateDiff::compute() {
  	const TNT::Array2D<Real>& bands = _bands.get();
  	
	vector<Real>& diffs = _diffs.get();
	


  int nFrames = bands.dim2();
  int nBands= bands.dim1();
  if(!nFrames || !nBands){
  throw EssentiaException("DilateDiff : empty bands or frames");
  }
	diffs.resize(nFrames);
	vector<Real> maxs = vector<Real>(nBands,0.);
	
//TODO: reduce vector length for unecessary bounds

for (int i = _frameW ; i< nFrames;i++){

	for(int j = _binW ; j<nBands-_binW ; j++){
		
		for (int k = j-_binW ; k<j+_binW ; k++){
		maxs[j] = max(maxs[j],bands[i][k]);
		
		}
	
	}
	diffs[i]=0;
	for(int j = _binW ; j<nBands-_binW ; j++){
		
		diffs[i] += bands[i-_frameW][j]-maxs[j]; 
	
	}
	diffs[i]/=nBands-2*_binW;
	




}




	




  
}




void DilateDiff::reset() {
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

const char* DilateDiff::name = standard::DilateDiff::name;
const char* DilateDiff::description = standard::DilateDiff::description;

DilateDiff::DilateDiff() : AlgorithmComposite() {

  _DilateDiff = standard::AlgorithmFactory::create("DilateDiff");
    declareInput(_bands, "bands", "the input bands spectrogram");
    declareOutput(_diffs, "Differences", "DilateDiffd input");
}

DilateDiff::~DilateDiff() {
  delete _DilateDiff;

}

void DilateDiff::reset() {
  AlgorithmComposite::reset();
  _DilateDiff->reset();
}

AlgorithmStatus DilateDiff::process() {
  if (!shouldStop()) return PASS;

  _DilateDiff->compute();


  return FINISHED;
}

} // namespace streaming
} // namespace essentia
