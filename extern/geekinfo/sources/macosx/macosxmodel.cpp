/*
  macosxmodel.cpp
  
  Copyright (c) 2006-2009 Primate Labs

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <map>
#include <set>
#include <string>
#include <iostream>

namespace {

std::map<std::string, std::string> s_models;
std::map<std::string, std::set<std::string> > s_processors;
std::set<std::string> s_hackintoshes;

const int kModelStateStart = 0;
const int kModelStateName = 1;
const int kModelStateMajor = 2;
const int kModelStateComma = 3;
const int kModelStateMinor = 4;

static bool is_mac_model_string(const std::string & id)
{
  int state = kModelStateStart;
  
  for (unsigned i = 0; i < id.length(); i++) {
    char c = id[i];
    switch(state) {
      case kModelStateStart:
        if (isalpha(c)) {
          state = kModelStateName;
        } else {
          return false;
        }
        break;
        
      case kModelStateName:
        if (isdigit(c)) {
          state = kModelStateMajor;
        } else if (!isalpha(c)) {
          return false;
        } 
        break;
        
      case kModelStateMajor:
        if (c == ',') {
          state = kModelStateComma;
        } else if (!isdigit(c)) {
          return false;
        }
        break;
        
      case kModelStateComma:
        if (isdigit(c)) {
          state = kModelStateMinor;
        } else {
          return false;
        }
        break;
        
      case kModelStateMinor:
        if (!isdigit(c)) {
          return false;
        }
        break;
    }
  }
    
  if (state != kModelStateMinor) {
    return false;
  }

  return true;
}

void init_model_information() {
  static bool initialized = false;
  if (!initialized) {
    #include "macosxmodel.inc"
    initialized = true;
  }
}

} // namespace

std::string get_model_name(const std::string& id, const std::string& processor)
{
  init_model_information();
  
  if (id == "PowerMac") {
    return "Rosetta";
  }
  if (s_processors.count(id) && s_processors[id].count(processor)) {
    return s_models[id];
  }
  if (s_hackintoshes.count(id)) {
    return "Hackintosh";
  }
  if (!is_mac_model_string(id)) {
    return "Hackintosh";
  }
  return id;
}

