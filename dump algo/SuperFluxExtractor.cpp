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

#include "SuperFluxExtractor.h"
#include "essentiamath.h"

using namespace std;

namespace essentia {
namespace standard {


const char* SuperFluxExtractor::name = "SuperFluxExtractor";
const char* SuperFluxExtractor::description = DOC("Superflux algorithm : Maximum filter and differentiation for onset detection robust again vibrato"
"Input : frames of audio");


void SuperFluxExtractor::configure() {
 	
 	
 	_windowSize = parameter("windowSize").toInt();
 	
 	
 		
	_specF->configure("size",_windowSize);
	
	_triF->configure("sampleRate",parameter("sampleRate").toInt());
 	
 	// SuperFlux Novelty Parameters
    _binW = parameter("binWidth").toInt();    
	_frameWi = parameter("frameWidth").toInt();


	
	_sfN->configure("Positive",true,
					"frameWidth",_frameWi,
					"binWidth",_binW)
					
	
	
	// Peak peaking parameters
	_threshold = parameter("threshold").toReal();
	_combine = parameter("combine").toInt();
	

	_sfP->configure(	
	"frameRate", 172,
    "threshold", _threshold,
	"combine",_combine,
    "pre_avg",  100,
	"pre_max", 30
	)


// local Buffers
circBuffer = TNT::Array2D<Real>(_windowSize/2+1,_frameWi);
int circIdx = 0;
tmpSpec.resize(_windowSize/2 +1);

}


void SuperFluxExtractor::compute() {

  	const vector<Real>& audio = _audio.get();
	Real& bin  = _isOnset.get();
	
	if(audio.size()!= _windowSize){
	_windowSize = audio.size();
	tmpSpec.resize(_windowSize/2 + 1);
	}
	
	
	specF->input("frame").set(audio);
	specF->output("spectrum").set(tmpSpec);
	
	_triF->input("spectrum").set(tmpSpec);
if(circBuffer.size()<_frameWi){
	_triF->output("bands").set(circBuffer

	}

  int nFrames = bands.dim1();
  int nBands= bands.dim2();
  if(!nFrames || !nBands){
  throw EssentiaException("SuperFluxExtractor : empty bands or frames");
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
}







void SuperFluxExtractor::reset() {
  Algorithm::reset();

}


// TODO in the case of lower accuracy in evaluation
// implement post-processing steps for methods in OnsetDetection, which required it
// wrapping the OnsetDetection algo
// - smoothing?
// - etc., whatever was requiered in original matlab implementations

} // namespace standard
} // namespace essentia


// #include "poolstorage.h"
// #include "algorithmfactory.h"
// 
// namespace essentia {
// namespace streaming {
// 
// const char* SuperFluxExtractor::name = standard::SuperFluxExtractor::name;
// const char* SuperFluxExtractor::description = standard::SuperFluxExtractor::description;
// 
// SuperFluxExtractor::SuperFluxExtractor() : AlgorithmComposite() {
// 
//   _SuperFluxExtractor = standard::AlgorithmFactory::create("SuperFluxExtractor");
//     declareInput(_bands, "bands", "the input bands spectrogram");
//     declareOutput(_diffs, "Differences", "SuperFluxExtractord input");
// }
// 
// SuperFluxExtractor::~SuperFluxExtractor() {
//   delete _SuperFluxExtractor;
// 
// }
// 
// void SuperFluxExtractor::reset() {
//   AlgorithmComposite::reset();
//   _SuperFluxExtractor->reset();
// }
// 
// AlgorithmStatus SuperFluxExtractor::process() {
//   if (!shouldStop()) return PASS;
// 
//   _SuperFluxExtractor->compute();
// 
// 
//   return FINISHED;
// }
// 
// } // namespace streaming
// } // namespace essentia
