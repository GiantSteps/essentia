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
	
	_rawMode = parameter("rawmode").toBool();

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


bool isStream = size <= max(_pre_avg,_pre_max )+1;
E_DEBUG(EAlgorithm,"sfpeaks is Stream" << isStream << "size " << size);
if (!isStream)peaks.resize(size);
	// Streaming mode hack, when >0 onset detected

	
	int nDetec=0;
	Real peakTime = 0;
	for( int i =0 ; i < size;i++){
		if(_rawMode){peaks[i]=0;}
		//cout << signal[i] <<"  " << maxs[i] <<"  " << avg[i] << endl;
		if(signal[i]==maxs[i] && signal[i]>avg[i]+_threshold && signal[i]>0){
			peakTime = i/frameRate;
			if((nDetec>0 && peakTime-peaks[nDetec-1]>_combine)  ||  nDetec ==0) {
				if(_rawMode){
				peaks[i]=1;
				}
				else{
					peaks[nDetec]=peakTime;
					
					}
				nDetec++;
			}
		}
		
		
	}

	if(!_rawMode)peaks.resize(nDetec);

return;
 
}






} // namespace standard
} // namespace essentia



#include "algorithmfactory.h"

namespace essentia {
namespace streaming {

const char* SuperFluxPeaks::name = standard::SuperFluxPeaks::name;
const char* SuperFluxPeaks::description = standard::SuperFluxPeaks::description;


AlgorithmStatus SuperFluxPeaks::process() {

 	bool producedData = false;


	AlgorithmStatus status = acquireData();
	if (status != OK) {
	  // acquireData() returns SYNC_OK if we could reserve both inputs and outputs
	  // being here means that there is either not enough input to process,
	  // or that the output buffer is full, in which cases we need to return from here
	  // cout << "peaks no fed" << endl;
	  return status;
	}
cout << "peaks fed" << endl;
	_algo->input("novelty").set(_signal.tokens());
	_algo->output("peaks").set(_peaks.tokens());

	_algo->compute();
	

	// give back the tokens that were reserved
	releaseData();

	return OK;
  
}

} // namespace streaming
} // namespace essentia
