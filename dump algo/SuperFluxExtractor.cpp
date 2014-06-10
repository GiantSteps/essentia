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




namespace essentia {
namespace streaming {


const char* SuperFluxExtractor::name = "SuperFluxExtractor";
const char* SuperFluxExtractor::description = DOC("Superflux algorithm : Use of superFluxNovelty and superFluxPeaks for extracting onsets with set of parameters advised by [1] \n"
"[1] : \"Maximum Filter Vibrato Suppression for Onset Detection\" by Sebastian Böck and Gerhard Widmer in Proceedings of the 16th International Conference on Digital Audio Effects (DAFx-13), Maynooth, Ireland, September 2013");


void SuperFluxExtractor::configure() {
 	
 	



}


void SuperFluxExtractor::compute() {



  
}







void SuperFluxExtractor::reset() {
  Algorithm::reset();

}










// 



namespace essentia {
namespace streaming {

const char* SuperFluxExtractor::name = standard::SuperFluxExtractor::name;
const char* SuperFluxExtractor::description = standard::SuperFluxExtractor::description;



AlgorithmStatus SuperFluxExtractor::process() {
  bool producedData = false;


    AlgorithmStatus status = acquireData();
    if (status != OK) {
      // acquireData() returns SYNC_OK if we could reserve both inputs and outputs
      // being here means that there is either not enough input to process,
      // or that the output buffer is full, in which cases we need to return from here
      return status;
    }

    _algo->input("bands").set(_bands.tokens());
    _algo->output("Differences").set(_diffs.tokens());

    _algo->compute();

    // give back the tokens that were reserved
    releaseData();

    return OK;
  
}


} // namespace streaming
} // namespace essentia
