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

#include "waveshaper.h"


using namespace std;
using namespace essentia;
using namespace standard;
using namespace util;





const char* waveshaper::name = "waveshaper";
const char* waveshaper::description = DOC(
"shapes the signal to a waveshaping model made of continuous linear part\n"
"optionally spline it"
"normalize option stands for pre-normalizing before waveshaping process\n"
"symmetric option waveshapes the absolute value of the signal\n"
"References:\n"
" \n");


void waveshaper::configure() {

_symmetric = parameter("symmetric").toBool();
_normalize = parameter("normalize").toBool();



vector<Real> _curxpts = parameter("xPoints").toVectorReal();
vector<Real> _curypts = parameter("yPoints").toVectorReal();


// check integrity
if(_curxpts.size()==0){
throw EssentiaException("no xpts or ypts") ;
return;
}
if(_curxpts.size()!=_curypts.size() ){
	throw EssentiaException("xpts and ypts have different size ") ;
	return;
}
for (int i = 1 ; i< _curxpts.size();i++){
	if(_curxpts[i]<_curxpts[i-1]){throw EssentiaException("xpts has to be ordered ") ;
	return;}
}






// make symmetric if asked
int start = 0;
int avoidDouble = 0; // handleing 0 center double
if (_symmetric){
	_xpts.resize(2*_curxpts.size());
	_ypts.resize(2*_curxpts.size());
	for(int i = _curxpts.size()-1 ; i>=0 ;i--){
		_xpts[(_curxpts.size()-1)-i]= double(-1.*_curxpts[i]);
		_ypts[(_curypts.size()-1)-i]= double(-1.*_curypts[i]);
	}
	
	start= _curxpts.size();
 	if( _curxpts[0]==0){
 	cout<<"av"<<endl;
 	avoidDouble = -1;}

}

for(int i = avoidDouble==0?0:1 ; i<_curxpts.size() ;i++){
	_xpts[i+start+avoidDouble] =double(_curxpts[i]);
	_ypts[i+start+avoidDouble] =double(_curypts[i]);
}
// resize taking into acount deleted 0 center
if (avoidDouble!=0){
	_xpts.resize(_xpts.size()-1);
	_ypts.resize(_ypts.size()-1);
}

// setup the spline
cout<<"val"<<endl;
for (int i = 0 ; i < _xpts.size();i++){
cout<<_xpts[i]<<endl;
}
				
_s.set_points(_xpts,_ypts);

}

void waveshaper::compute() {
	
  const std::vector<Real>& signal = _signal.get();
  vector<Real>& signalout = _signalout.get();
	
  if (int(signal.size()) <= 1) {
    signalout.resize(0);
    throw EssentiaException("nosignal");
    return;
  }
  else{
  signalout.resize( int(signal.size()));
  Real max =1.0;
  if (_normalize){
  max = 1e-37;
  for (int i = 0;i<int(signal.size());i++){
  max = std::max(max,abs(signal[i]));
  }
  }
  bool neg = false;
  
  for(int i = 0 ; i< int(signal.size());i++){
	Real cur = signal[i];
  	if(_normalize){cur/=max;}
	
	
	if (_spline){ 

	 signalout[i] =Real(_s(double(cur)));

			}
	else{
		if(cur<=_xpts[0]){
			signalout[i]=_ypts[0];
		}
		else if(cur>=_xpts[_xpts.size()-1]){
			signalout[i]=_ypts[_xpts.size()-1];
		}
		else{
			for(int j = 1 ; j< _xpts.size();j++){
				if(cur>_xpts[j-1]&&cur<=_xpts[j]){
				signalout[i] = (Real)(_ypts[j-1]+(cur-_xpts[j-1])*1.0*(_ypts[j]-_ypts[j-1])*1.0/(_xpts[j]-_xpts[j-1]));
				break;
				}
			}
		
		}
	}

  }
  
  }


return;
  

}

