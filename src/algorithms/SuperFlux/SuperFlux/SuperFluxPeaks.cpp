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
        const char* SuperFluxPeaks::description = DOC("Peak peaking from Superflux algorithm (see SuperFluxExtractor for references)");
        
        
        void SuperFluxPeaks::configure() {
            
            
            
            frameRate = parameter("frameRate").toReal();
            
            // convert to framenumber
            _pre_avg = int(frameRate* parameter("pre_avg").toReal() / 1000.);
            _pre_max = int(frameRate * parameter("pre_max").toReal() / 1000.);
            
            
            if(_pre_avg<=1)
                throw EssentiaException("SuperFluxPeaks: too small _pre_averaging filter size");
            if(_pre_max<=1)
                throw EssentiaException("SuperFluxPeaks: too small _pre_maximum filter size");
            // convert to seconds
            _combine = parameter("combine").toReal()/1000.;
            
            _movAvg->configure("size",_pre_avg);
            _maxf->configure("width",_pre_max,"causal",true);
            
            _threshold = parameter("threshold").toReal();
            _ratioThreshold = parameter("ratioThreshold").toReal();
            
            activation_slope = parameter("activation_slope").toBool();
            
            
            peakTime = 0;
            lastVal = 0;
            wasMax = false;
            wasActivated = false;
            
            
        }
        
        
        void SuperFluxPeaks::compute() {
            
            const vector<Real>& signal = _signal.get();
            vector<Real>& peaks = _peaks.get();
            vector<Real>& strengths = _strengths.get();
            if (signal.empty()) {
                peaks.resize(0);
                strengths.resize(0);
                return;
            }
            
            int size = signal.size();
            
            
            
            
            vector<Real> avg(size,0);
            _movAvg->input("signal").set(signal);
            _movAvg->output("signal").set(avg);
            _movAvg->compute();
            
            
            vector<Real> maxs(size,0);
            _maxf->input("signal").set(signal);
            _maxf->output("signal").set(maxs);
            _maxf->compute();
            
            
            int nDetec=0;
            int nActivations=0;
            
            for( int i =0; i <size;i++){
                
                
                if( wasMax && signal[i] < lastVal)
                {
                    
                    //               cout << "found : lin" << isOverLinearThreshold << "ratio: " << isOverratioThreshold << endl;
                    cout << lastVal << " : " <<signal[i] << " : " << maxs[i] << " : " <<  avg[i] << endl;
                    peakTime = i*1.0/frameRate;
                    if((nDetec>0 && peakTime-peaks[nDetec-1]>_combine)  ||  nDetec ==0) {
                        peaks[nDetec] = peakTime;
                        strengths[nDetec] = lastVal;
                        nDetec++;
                        wasActivated = false;
                    }
                    

                        wasMax = false;
                }
                
                
                // we want to avoid ratioThreshold noisy activation in really low flux parts so we set noise floor
                // set by default to 10-7 (REALLY LOW for a flux)
                else if(signal[i]==maxs[i]){//&& signal[i]>1e-8){
                    
                    bool isOverLinearThreshold = _threshold>0 &&  signal[i]>avg[i]+_threshold ;
                    bool isOverratioThreshold = _ratioThreshold>0 &&avg[i]>0 && signal[i]*1.0/avg[i]>_ratioThreshold;
                    bool isMax = (_threshold == 0 || isOverLinearThreshold) && (_ratioThreshold == 0 || isOverratioThreshold);
                    
                    // we only want the first activation
                    if(activation_slope &&!wasActivated && isMax&& !wasMax){
                        peaks[nActivations] = peakTime;
                        strengths[nActivations] = 0;
                        nActivations++;
                        wasActivated = true;
                    }
                    wasMax |= isMax;
                    
                    
                }
                if(wasMax){
                    cout << "w" << endl;
                }
                
                lastVal = signal[i];
            }
            strengths.resize(nDetec+nActivations);
            peaks.resize(nDetec+nActivations);
            return;
            
        }
        
        
        
        
        
        
    } // namespace standard
} // namespace essentia



#include "algorithmfactory.h"

namespace essentia {
    namespace streaming {
        
        const char* SuperFluxPeaks::name = standard::SuperFluxPeaks::name;
        const char* SuperFluxPeaks::description = standard::SuperFluxPeaks::description;
        
        
        void SuperFluxPeaks::consume() {
            
            
            
            int _aquireSize = _signal.acquireSize();
            
            std::vector<Real> out = std::vector<Real>(_aquireSize);
            std::vector<Real> outStrengths = std::vector<Real>(_aquireSize);
            _algo->input("novelty").set(_signal.tokens());
            _algo->output("peaks").set(out);
            _algo->output("strengths").set(outStrengths);
            _algo->compute();
            
            
            
            if(out.size()>0){
                
                //trim firstpart  if needed
                bool trimBeg = false;
                if(onsTime.size()>0 && (current_t+ out[0] - onsTime.back()<_combine)){
                    trimBeg = true;
                }
                
                // copy if there is something to copy
                if(!trimBeg || out.size()>1){
                    onsTime.insert(onsTime.end(), out.begin(),out.end() - (trimBeg?1:0));
                    onsStrength.insert(onsStrength.end(), outStrengths.begin(),outStrengths.end() - (trimBeg?1:0));
                }
                
            }
            
            
            current_t+=_aquireSize/framerate;
            
        }
        
        void SuperFluxPeaks::finalProduce() {
            
            _peaks.push((std::vector<Real>) onsTime);
            //    if(onsTime.size()){
            //        cout << nProcess << endl;
            //        cout << onsTime  << endl;
            //    }
            _strengths.push((std::vector<Real>)onsStrength);
            
            
            reset();
            
            
        }
        
        
        void SuperFluxPeaks::reset(){
            current_t=0;
            onsTime.clear();
            onsStrength.clear();
            
        }
        
    } // namespace streaming
} // namespace essentia
