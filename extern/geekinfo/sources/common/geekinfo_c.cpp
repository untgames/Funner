/*
	geekinfo_c.cpp

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

#include <cstdlib>
#include <string>
#include <cstring>
#include "geekinfo.h"

char *systemMetric_C(SystemMetricType metric)
{
    char *meow;
    std::string moof;
    moof = systemMetric(metric);
    meow = (char *)malloc(1 + moof.size());
    strcpy(meow, moof.c_str());
    return meow;
}

uint64 systemMetricInteger_C(SystemMetricType metric)
{
    return systemMetricInteger(metric);
}

char *systemMetricName_C(SystemMetricType metric)
{
    char *meow;
    std::string moof;
    moof = systemMetricName(metric);
    meow = (char *)malloc(1 + moof.size());
    strcpy(meow, moof.c_str());
    return meow;
}
