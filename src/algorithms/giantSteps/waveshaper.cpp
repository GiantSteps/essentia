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
#include "essentiamath.h"

using namespace std;
using namespace essentia;
using namespace standard;





const char* waveshaper::name = "waveshaper";
const char* waveshaper::description = DOC(
"shapes the signal to a waveshaping model made of 3 continuous linear part\n"
"(p1,p2),(p2,p3),(p3,p4) "
"normalize option stands for pre-normalizing before waveshaping process\n"
"symmetric option waveshapes the absolute value of the signal\n"
"References:\n"
" \n");


void waveshaper::configure() {

_symmetric = parameter("symmetric").toBool();
_normalize = parameter("normalize").toBool();

  _p1x =  parameter("p1x").toReal();
  _p2x =  parameter("p2x").toReal();
  _p3x =  parameter("p3x").toReal();
  _p4x =  parameter("p4x").toReal();
  _p1y =  parameter("p1y").toReal();
  _p2y =  parameter("p2y").toReal();
  _p3y =  parameter("p3y").toReal();
  _p4y =  parameter("p4y").toReal();
if(_p1x>=_p2x||_p2x>=_p3x||_p3x>=_p4x) throw EssentiaException("wrong Params or badly sorted p1x<p2x<p3x<p4x") ;
  _c1 = (_p2y-_p1y)*1.0/(_p2x-_p1x);
  _c2 = (_p3y-_p2y)*1.0/(_p3x-_p2x);
  _c3 = (_p4y-_p3y)*1.0/(_p4x-_p3x);
  
  

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
	  if(_symmetric){neg =cur<0; cur = abs(cur);} 
		
		
		if(cur>_p1x&&cur<=_p2x)  
			{signalout[i] = (Real)(_p1y+(cur-_p1x)*_c1);}
		else if(cur>_p2x&&cur<=_p3x) 
			{signalout[i] = (Real)(_p2y+(cur-_p2x)*_c2);}
		else if(cur>_p3x&&cur<=_p4x) 
			{signalout[i] = (Real)(_p3y+(cur-_p3x)*_c3);}
		else if(cur<=_p1x) 
			{signalout[i] = (Real)_p1y;}
		else 
			{signalout[i] = (Real)_p4y;}
   
   if(_symmetric && neg){signalout[i] *=-1;}
  }
  
  }



  

}

