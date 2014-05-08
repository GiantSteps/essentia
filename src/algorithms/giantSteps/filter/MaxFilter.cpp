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

#include "MaxFilter.h"
#include "essentiamath.h"
#define HERKGIL
//TODO:Validate HERKGIL
using namespace std;

namespace essentia {
namespace standard {


const char* MaxFilter::name = "MaxFilter";
const char* MaxFilter::description = DOC("Superflux algorithm : Maximum filter and differentiation for onset detection robust again vibrato"
"Input : filterbank like spectrogram");


void MaxFilter::configure() {
 	
 	
 	//width has to be odd, then local binW represent half the width
    _width = parameter("width").toInt();
 

}

#ifdef HERKGIL

//ToODO: faster max filter but Not Working
void MaxFilter::compute() {


  	const vector<Real>& array = _array.get();
  	
	vector<Real>& filtered = _filtered.get();

   	int size= array.size();

	filtered.resize(size);
	Real maxs = 0;
	
	Real cur_diff = 0;

// for herk gil algo s represent the whole width
if(_width%2==0)_width++;
int kl=(_width-1)/2;

vector<Real> cs(_width-2);
vector<Real> ds(_width-2);



for(int u = kl ; u<size ; u+=_width-1){
		ds[0]=array[u];
		
		for (int i=1;i<=_width-2;i++){
			ds[i] = max(ds[i-1],array[u+i]);
		}
		cs[_width-2] = array[u-1];
		
		for (int i = 1 ; i <= _width-2 ; i++){
			cs[_width-i-2] = max(cs[_width-i-1],array[u-i-1]);
		}
		
		for (int i = 0 ; i <= _width-2 ; i++){
			filtered[u-kl+i] = max(cs[i],ds[i]);
			
		
		}
		
	
}

}



#else

void MaxFilter::compute() {
  	const vector<Real>& array = _array.get();
  	
	vector<Real>& filtered = _filtered.get();


  	int size= array.size();

	filtered.resize(size);
	Real maxs = 0;
	
	Real cur_diff = 0;


		//TODO : check relevance and may be go toward a less naive algorithm (Herk Gil?)
	
for(int j = _width ; j<size ; j++){


		// if the outgoing term is not last max the new max is faster to compute 

		if(j>_width && array[j-_width-1]<maxs){
			maxs = max(maxs,array[j+_width]);
		}	
		else{
			maxs =array[j-_width];
			for (int k = j-_width+1 ; k<=j+_width ; k++){
				maxs = max(maxs,array[k]);
			}
		}	
		filtered[j]=maxs;

	}
	

}


#endif



void MaxFilter::reset() {
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

const char* MaxFilter::name = standard::MaxFilter::name;
const char* MaxFilter::description = standard::MaxFilter::description;

MaxFilter::MaxFilter() : AlgorithmComposite() {

  _MaxFilter = standard::AlgorithmFactory::create("MaxFilter");
    declareInput(_array, "bands", "the input bands spectrogram");
    declareOutput(_filtered, "Differences", "MaxFilterd input");
}

MaxFilter::~MaxFilter() {
  delete _MaxFilter;

}

void MaxFilter::reset() {
  AlgorithmComposite::reset();
  _MaxFilter->reset();
}

AlgorithmStatus MaxFilter::process() {
  if (!shouldStop()) return PASS;

  _MaxFilter->compute();


  return FINISHED;
}

} // namespace streaming
} // namespace essentia
