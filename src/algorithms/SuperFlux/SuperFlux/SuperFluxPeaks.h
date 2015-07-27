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

#ifndef ESSENTIA_SuperFluxPeaks_H
#define ESSENTIA_SuperFluxPeaks_H

#include "algorithmfactory.h"
using namespace std;
namespace essentia {
namespace standard {
        
class SuperFluxPeaks : public Algorithm {
    
private:
    Input<std::vector<Real> > _signal;
    Output<std::vector<Real> > _peaks;
    Output<std::vector<Real> > _strengths;
    standard::Algorithm* _movAvg;
    standard::Algorithm* _maxf;
    
    int _pre_avg;
    int _pre_max;
    Real _combine;
    Real _threshold;
    Real _ratioThreshold;
    
    Real peakTime;
    Real lastVal;
    bool wasMax;
    bool activation_slope;
    bool wasActivated;
    
    int hopSize;
    Real frameRate;

    
public:
    SuperFluxPeaks() {
        declareInput(_signal, "novelty", "the input novelty");
        declareOutput(_peaks, "peaks", "the output instants [s]");
        declareOutput(_strengths, "strengths", "the output instants [s]");
        _movAvg = AlgorithmFactory::create("MovingAverage");
        _maxf = AlgorithmFactory::create("MaxFilter");

        
    }
    
    ~SuperFluxPeaks(){
        delete _movAvg;
        delete _maxf;
    }
    
    
    
    void declareParameters() {
        declareParameter("frameRate", "frameRate", "(0,inf)", 172.);
        declareParameter("threshold","threshold for peak peaking compared to the difference of novelty_signal and average_signal ,  for peaking onsets in ambien noise","[0,inf)",.05);
        declareParameter("ratioThreshold", "ratio threshold for peak-picking compared to novelty_signal/novelty_average, 0  disable it ,  for low energy onsets", "[0,inf)", 16.);
        declareParameter("combine", "ms for onset combination", "(0,inf)", 30.);
        declareParameter("pre_avg", "use N miliseconds past information for moving average", "(0,inf)", 100.);
        declareParameter("pre_max", "use N miliseconds past information for moving maximum", "(0,inf)", 30.);
        declareParameter("activation_slope", "add the time where thresholds have been activated (different than actual maximum),corresponding strengths values will be set to zero", "{false,true}", false);
        
    }
    
    void reset() {
        Algorithm::reset();_maxf->reset();_movAvg->reset();peakTime=0;lastVal=0;wasMax = false;
    };
    void configure();
    void compute();
    
    
    static const char* name;
    static const char* version;
    static const char* description;
};
    
} // namespace standard
} // namespace essentia


#include "accumulatoralgorithm.h"

namespace essentia {
namespace streaming {

class SuperFluxPeaks : public AccumulatorAlgorithm {
            
protected:
    Sink<Real> _signal;
    Source<std::vector<Real> > _peaks;
    Source<std::vector<Real> > _strengths;
    
    standard::Algorithm * _algo;
    

    float current_t;
    
    float framerate,_combine;
    std::vector<Real> onsTime,onsStrength;

    
    
public:
    SuperFluxPeaks(){
        
        _algo = standard::AlgorithmFactory::create("SuperFluxPeaks");
        declareInputStream(_signal, "novelty", "the input novelty");
        declareOutputResult(_peaks, "peaks", "peaks instants [s]");
        declareOutputResult(_strengths, "strengths", "peaks values [novelty unit]");
    };
    ~SuperFluxPeaks(){
        delete _algo;
    }
    
    
    void declareParameters() {
        declareParameter("frameRate", "frameRate", "(0,inf)", 172.);
        declareParameter("threshold","threshold for peak peaking compared to the difference of novelty_signal and average_signal ,  for peaking onsets in ambien noise","[0,inf)",.05);
        declareParameter("ratioThreshold", "ratio threshold for peak-picking compared to novelty_signal/novelty_average, 0  disable it ,  for low energy onsets", "[0,inf)", 16.);
        declareParameter("combine", "ms for onset combination", "(0,inf)", 30.0);
        declareParameter("pre_avg", "use N miliseconds past information for moving average", "(0,inf)", 100.);
        declareParameter("pre_max", "use N miliseconds past information for moving maximum", "(0,inf)", 30.);
        declareParameter("activation_slope", "add the time where thresholds have been activated (different than actual maximum),corresponding strengths values will be set to zero", "{false,true}", false);
    };
    
    
    // link algo parameter with streaming burffer options
    
    void configure(){
        _algo->configure(this->_params);
        framerate = _algo->parameter("frameRate").toReal();
        _combine = parameter("combine").toReal()/1000.;
        current_t=0;
   
    };
    void consume();
    void finalProduce();
    void reset();

    
    
    static const char* name;
    static const char* description;
    
};
} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_SuperFluxPeaks_H
