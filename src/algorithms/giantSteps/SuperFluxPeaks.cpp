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

#include "SuperFluxPeaks.h"
#include <complex>
#include <limits>
#include "essentiamath.h"

using namespace std;

namespace essentia {
namespace standard {


const char* SuperFluxPeaks::name = "SuperFluxPeaks";
const char* SuperFluxPeaks::description = DOC("get Peaks");


void SuperFluxPeaks::configure() {
		frameRate = parameter("frameRate").toReal();
        _pre_avg = int(frameRate* parameter("pre_avg").toReal() / 1000.);
        _pre_max = int(frameRate * parameter("pre_max").toReal() / 1000.);
        
        // convert to seconds
    	_combine = parameter("combine").toReal()/1000.;

_threshold = parameter("threshold").toInt();

		_movAvg->configure("size",_pre_avg+1);
		_maxf->configure("width",_pre_max+1);


}


void SuperFluxPeaks::compute() {
  const vector<Real>& signal = _signal.get();
vector<Real>& peaks = _peaks.get();
  if (signal.empty()) {
peaks.resize(0);
    return;
  }

int size = signal.size();


vector<Real> avg(size);

_movAvg->input("signal").set(signal);
_movAvg->output("signal").set(avg);
_movAvg->compute();

vector<Real> maxs(size);

_maxf->input("signal").set(signal);
_maxf->output("signal").set(maxs);
_maxf->compute();


peaks.resize(size);
int nDetec=0;
Real peakTime = 0;
for( int i =0 ; i < size;i++){
	if(signal[i]==maxs[i] && signal[i]>avg[i]+_threshold && signal[i]>0){
		peakTime = i/frameRate;
		if((nDetec>0 && peakTime-peaks[nDetec-1]>_combine)  ||  nDetec ==0) peaks[nDetec]=peakTime;

		nDetec++;
	}

}

peaks.resize(nDetec);

return;


  
}


void SuperFluxPeaks::reset() {
  Algorithm::reset();

}


// TODO in the case of lower accuracy in evaluation
// implement post-processing steps for methods in OnsetDetection, which required it
// wrapping the OnsetDetection algo
// - smoothing?
// - etc., whatever was requiered in original matlab implementations

} // namespace standard
} // namespace essentia



#include "algorithmfactory.h"

namespace essentia {
namespace streaming {

const char* SuperFluxPeaks::name = standard::SuperFluxPeaks::name;
const char* SuperFluxPeaks::description = standard::SuperFluxPeaks::description;

SuperFluxPeaks::SuperFluxPeaks() : AlgorithmComposite() {

  _SuperFluxPeaks = standard::AlgorithmFactory::create("SuperFluxPeaks");


  declareInput(_signal, 1, "signal", "the input signal");   // 1
  declareOutput(_peaks, 1, "peaks", "Onsets"); // 0


}

SuperFluxPeaks::~SuperFluxPeaks() {
  delete _SuperFluxPeaks;

}

void SuperFluxPeaks::reset() {
  AlgorithmComposite::reset();
  _SuperFluxPeaks->reset();
}

AlgorithmStatus SuperFluxPeaks::process() {
  if (!shouldStop()) return PASS;

 
  //const vector<Real>& signal = _pool.value<vector<Real> >("internal.signal");


  _SuperFluxPeaks->compute();


  return FINISHED;
}

} // namespace streaming
} // namespace essentia
