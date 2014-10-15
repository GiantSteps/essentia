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

#include <fileref.h>
#include <tpropertymap.h>
#include <tag.h>

#include "metadatareader.h"
#include "metadatautils.h"
#include "essentiautil.h"


using namespace std;
//using TagLib::FileRef;
//using TagLib::PropertyMap;
//using TagLib::String;
//using TagLib::StringList;

string fixInvalidUTF8(const string& str) {
  // a big fat hack to try to fix invalid utf-8 characters
  // see http://www.utf8-chartable.de/
  // http://stackoverflow.com/questions/6555015/check-for-invalid-utf8
  // http://stackoverflow.com/questions/17316506/strip-invalid-utf8-from-string-in-c-c
  string fixed;
  fixed.reserve(str.size());
  unsigned char c, c2=0, c3=0, c4=0;

  for(int i=0; i<(int)str.size(); i++) {
    c = (unsigned char)str[i];

    if (c < 127) { // normal ascii
      if (c==9 || c==10 || c==13 || c >= 32) { // normal char or \t \n \r
        fixed += c;
      }
    }
    else if (c < 160) { // control character
      if (c2 == 128) { // fix microsoft mess, add euro
        fixed += 226;
        fixed += 130;
        fixed += 172;
      }
      if (c2 == 133) { // fix IBM mess, add NEL = \n\r
        fixed += 10;
        fixed += 13;
      }
    }
    else if (c<192) { // invalid for utf8, converting ascii
      fixed += (unsigned char)194;
      fixed += c;
    }
    else if (c<194) { // invalid for utf8, converting ascii
      fixed += (unsigned char)195;
      fixed += c-64;
    }
    else if(c < 224) { // possibly two-byte utf8
      c2=(unsigned char)str[i+1];
      if (c2>127 && c2<192) { // valid two-byte utf8
        if (c==194 && c2<160) { // control char, skipping
          ;
        }
        else {
          fixed += c;
          fixed += c2;
        }
        i++;
      }
      else { // invalid utf8, converting ascii
        fixed += (unsigned char)195;
        fixed += c-64;
      }
    } else if (c < 240) { // possibly three-byte utf8
      c2=(unsigned char)str[i+1];
      c3=(unsigned char)str[i+2];
      if (c2>127 && c2<192 && c3>127 && c3<192) { // valid three-byte utf8
        fixed += c;
        fixed += c2;
        fixed += c3;
        i += 2;
      }
      else { // invalid utf8, converting ascii
        fixed += (unsigned char)195;
        fixed += c-64;
      }
    } else if (c<245) { // possibly four-byte utf8
      c2=(unsigned char)str[i+1];
      c3=(unsigned char)str[i+2];
      c4=(unsigned char)str[i+3];
      if (c2>127 && c2<192 && c3>127 && c3<192 && c4>127 && c4<192) {
        // valid four-byte utf8
        fixed += c;
        fixed += c2;
        fixed += c3;
        fixed += c4;
        i += 3;
      } else { // invalid utf8, converting ascii
        fixed += (unsigned char)195;
        fixed += c-64;
      }
    }
    else { // invalid utf8, converting ascii
      fixed += (unsigned char)195;
      fixed += c-64;
    }
  }
  return fixed;
}


bool containsControlChars(const string& str) {
  for (int i=0; i<(int)str.size(); i++) {
    int c = (unsigned char)str[i];
    if ((c >= 0x00 && c <= 0x1F &&
         c != 0x09 && c != 0x0A && c != 0x0D) || // C0 control code set minus newlines & tabs
        (c >= 0x80 && c <= 0x9F)) { // C1 control code set
      return true;
    }
  }
  return false;
}

#if TAGLIB_MAJOR_VERSION == 1 && TAGLIB_MINOR_VERSION >= 5

bool isLatin1(const TagLib::String& str) {
  return str.isLatin1();
}

#else // TAGLIB_VERSION < 1.5

bool isLatin1(const TagLib::String& str) {
  // adapted from taglib 1.5
  for (int i=0; i<(int)str.size(); i++) {
    if (str[i] >= 256) return false;
  }
  return true;
}

#endif // TAGLIB_VERSION >=? 1.5


// Utility function to format tags so that they can be correctly parsed back
string formatString(const TagLib::StringList& strList) {
  TagLib::String str = strList.toString(";");
  if (str.isEmpty()) return "";

  string result = str.to8Bit(true);

  // heuristic to detect wrongly encoded tags (ie: twice latin-1 to utf-8, mostly)
  // we should encode everything ourselves to utf-8, but sometimes it might happen
  // that someone already did that, but told us the string was in latin-1.
  // A way to detect that is if the string contains only latin-1 chars, when
  // converting it to latin-1 it contains code chars, this probably means it was
  // previously encoded in utf-8
  if (isLatin1(str) &&
      containsControlChars(str.to8Bit(false))) {
    result = str.to8Bit(false);
  }

  // fix invalid utf-8 characters
  result = fixInvalidUTF8(result);

  return result;
}

namespace essentia {
namespace standard {

const char* MetadataReader::name = "MetadataReader";
const char* MetadataReader::description = DOC("This algorithm outputs the metadata tags associated with audio files, as well as their audio properties (e.g. bitrate, length, etc.). Supported audio file types are:\n"
"  - mp3\n"
"  - flac\n"
"  - ogg\n"
"An exception is thrown if unsupported filetype is given or if the file does not exist.\n"
"Please observe that the .wav format is not supported. Also note that this algorithm incorrectly calculates the number of channels for a file in mp3 format only for versions less than 1.5 of taglib in Linux and less or equal to 1.5 in Mac OS X\n"
"This algorithm also contains some heuristic to try to deal with encoding errors in the tags and tries to do the appropriate conversion if a problem was found (mostly twice latin1->utf8 conversion).\n"
);


void MetadataReader::configure() {
  if (parameter("filename").isConfigured()) {
    _filename = parameter("filename").toString();
  }
  _tagPoolName = parameter("tagPoolName").toString(); 
}

void MetadataReader::compute() {
  if (!parameter("filename").isConfigured()) {
    throw EssentiaException("MetadataReader: 'filename' parameter has not been configured");
  }

  TagLib::FileRef f(_filename.c_str());

  Pool tagPool;

  if (f.isNull()) {
    // in case TagLib can't get metadata out of this file, try some basic PCM approach
    int pcmSampleRate = 0;
    int pcmChannels = 0;
    int pcmBitrate = 0;

    try {
      pcmMetadata(_filename, pcmSampleRate, pcmChannels, pcmBitrate);
    }
    catch (EssentiaException& e) {
      if (parameter("failOnError").toBool())
        throw EssentiaException("MetadataReader: File does not exist or does not seem to be of a supported filetype. ", e.what());
    }

    _title.get()   = "";
    _artist.get()  = "";
    _album.get()   = "";
    _comment.get() = "";
    _genre.get()   = "";
    _track.get()   = "";
    _date.get()    = "";

    _tagPool.get()  = tagPool;

    _duration.get()   = 0;
    _bitrate.get()    = pcmBitrate;
    _sampleRate.get() = pcmSampleRate;
    _channels.get()   = pcmChannels;

    return;
  }
  
  /*
  TagLib::Tag *tag = f.tag();

  cout << "-- TAG (basic) --" << endl;
  cout << "title   - \"" << tag->title()   << "\"" << endl;
  cout << "artist  - \"" << tag->artist()  << "\"" << endl;
  cout << "album   - \"" << tag->album()   << "\"" << endl;
  cout << "year    - \"" << tag->year()    << "\"" << endl;
  cout << "comment - \"" << tag->comment() << "\"" << endl;
  cout << "track   - \"" << tag->track()   << "\"" << endl;
  cout << "genre   - \"" << tag->genre()   << "\"" << endl; 
  */

  TagLib::PropertyMap tags = f.file()->properties();

  _title.get()   = formatString(tags["TITLE"]);
  _artist.get()  = formatString(tags["ARTIST"]);
  _album.get()   = formatString(tags["ALBUM"]);
  _comment.get() = formatString(tags["COMMENT"]);
  _genre.get()   = formatString(tags["GENRE"]);
  _track.get()   = formatString(tags["TRACKNUMBER"]);  
  _date.get()    = formatString(tags["DATE"]); 

  // populate tag pool
  for(TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end(); ++i) {
    string key = i->first.to8Bit(true);
    // remove '.' chars which are used in Pool descriptor names as a separator
    // convert to lowercase
    std::replace(key.begin(), key.end(), '.', '_'); 
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    key = _tagPoolName + "." + key;

    for(TagLib::StringList::ConstIterator str = i->second.begin(); str != i->second.end(); ++str) {
      tagPool.add(key, str->to8Bit(true));
    }
  }

  _tagPool.get()  = tagPool;

  _duration.get()     = f.audioProperties()->length();
  _bitrate.get()    = f.audioProperties()->bitrate();
  _sampleRate.get() = f.audioProperties()->sampleRate();
  _channels.get()   = f.audioProperties()->channels();

  // fix for taglib incorrectly returning the bitrate for wave files
  string ext = toLower(_filename.substr(_filename.size()-3));
  if (ext == "wav") {
    _bitrate.get() = _bitrate.get() * 1024 / 1000;
  }
}

} // namespace standard
} // namespace essentia

namespace essentia {
namespace streaming {


const char* MetadataReader::name = standard::MetadataReader::name;
const char* MetadataReader::description = standard::MetadataReader::description;


void MetadataReader::configure() {
  _filename = parameter("filename").toString();
  _newlyConfigured = true;
}

AlgorithmStatus MetadataReader::process() {
  if (_filename == "" || !_newlyConfigured) return PASS;

  TagLib::FileRef f(_filename.c_str());

  //Pool tagPool;

  if (f.isNull()) {
    // in case TagLib can't get metadata out of this file, try some basic PCM approach
    int pcmSampleRate = 0;
    int pcmChannels = 0;
    int pcmBitrate = 0;

    try {
      pcmMetadata(_filename, pcmSampleRate, pcmChannels, pcmBitrate);
    }
    catch (EssentiaException& e) {
      if (parameter("failOnError").toBool())
        throw EssentiaException("MetadataReader: File does not exist or does not seem to be of a supported filetype. ", e.what());
    }
    string ns = "";
    _title.push(ns);
    _artist.push(ns);
    _album.push(ns);
    _comment.push(ns);
    _genre.push(ns);
    _track.push(ns);
    _date.push(ns);
    //_tagPool.push(tagPool);
    _duration.push(0);
    _bitrate.push(pcmBitrate);
    _sampleRate.push(pcmSampleRate);
    _channels.push(pcmChannels);
  }
  else {
    TagLib::PropertyMap tags = f.file()->properties();

    _title.push(formatString(tags["TITLE"]));
    _artist.push(formatString(tags["ARTIST"]));
    _album.push(formatString(tags["ALBUM"]));
    _comment.push(formatString(tags["COMMENT"]));
    _genre.push(formatString(tags["GENRE"]));
    _track.push(formatString(tags["TRACKNUMBER"]));
    _date.push(formatString(tags["DATE"]));

    // populate tag pool
    /*
    for(PropertyMap::Iterator it = tags.begin(); it != tags.end(); ++it) {
      for(StringList::Iterator str = it->second.begin(); str != it->second.end(); ++str) {
        tagPool.add(it->first.to8Bit(true), str->to8Bit(true));
      }
    }
    */


    /*
    cout << "musicbrainz_recordingid = MUSICBRAINZ_TRACKID = " << formatString(tags["MUSICBRAINZ_TRACKID"]) << endl;
    cout << "musicbrainz_albumid = MUSICBRAINZ_ALBUMID = " << formatString(tags["MUSICBRAINZ_ALBUMID"]) << endl;
    cout << "musicbrainz_artistid = MUSICBRAINZ_ARTISTID = " << formatString(tags["MUSICBRAINZ_ARTISTID"]) << endl;
    cout << "musicbrainz_albumartistid = MUSICBRAINZ_ALBUMARTISTID = " << formatString(tags["MUSICBRAINZ_ALBUMARTISTID"]) << endl;
    cout << "musicbrainz_releasegroupid = MUSICBRAINZ_RELEASEGROUPID = " << formatString(tags["MUSICBRAINZ_RELEASEGROUPID"]) << endl;
    cout << "musicbrainz_workid = MUSICBRAINZ_WORKID = " << formatString(tags["MUSICBRAINZ_WORKID"]) << endl;
    cout << "ACOUSTID_ID = " << formatString(tags["ACOUSTID_ID"]) << endl;
    cout << "ACOUSTID_FINGERPRINT = " << formatString(tags["ACOUSTID_FINGERPRINT"]) << endl;
    */

    // TODO: missing in taglib?
    // musicbrainz_trackid = MUSICBRAINZ_RELEASETRACKID
    // musicbrainz_trmid = MUSICBRAINZ_TRMID
    // musicbrainz_discid = MUSICBRAINZ_DISCID

    //cout << "PropertyMap = " << formatString(tags.toString()) << endl;

    //_tagPool.push(tagPool);


    _duration.push((int)f.audioProperties()->length());

    int bitrate = f.audioProperties()->bitrate();
    // fix for taglib incorrectly returning the bitrate for wave files
    string ext = toLower(_filename.substr(_filename.size()-3));
    if (ext == "wav") {
      bitrate = bitrate * 1024 / 1000;
    }

    _bitrate.push((int)bitrate);
    _sampleRate.push((int)f.audioProperties()->sampleRate());
    _channels.push((int)f.audioProperties()->channels());
  }

  _newlyConfigured = false;
  shouldStop(true);
  return OK;
}

} // namespace streaming
} // namespace essentia
