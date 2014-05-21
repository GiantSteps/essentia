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

#include "RingBufferRt.h"

#include "sourcebase.h"
#include "atomic.h"

using namespace std;

namespace essentia {
namespace streaming {

const char* RingBufferRt::name = "RingBufferRt";
const char* RingBufferRt::description = DOC(
"This algorithm gets data from an input ringbuffer of type Real that is fed into the essentia streaming mode."
);

RingBufferRt::RingBufferRt():_impl(0)
{
  declareOutput(_output, 1024, "signal", "data source of what's coming from the ringbuffer");
  _output.setBufferType(BufferUsage::forAudioStream);
}

RingBufferRt::~RingBufferRt()
{
	delete _impl;
}

void RingBufferRt::configure()
{
	delete _impl;
	_impl = new RingBufferImpl(RingBufferImpl::kAvailable,parameter("bufferSize").toInt());
}




void RingBufferRt::reset() {
  Algorithm::reset();
  _impl->reset();
}

} // namespace streaming
} // namespace essentia
