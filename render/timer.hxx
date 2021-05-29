////////////////////////////////////////////////////////////////////
//
// $Id: timer.hxx,v 1.1.1.1 2005/04/01 17:12:53 kanai Exp $
//
// timer class
//
// Copyright (c) 2002 by Keio Research Institute at SFC
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _TIMER_H
#define _TIMER_H 1

#include "envDep.h"

#ifndef WIN32 // linux
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#else
#include <cstdio>
#include <iostream>
using namespace std;
#endif

#ifdef USE_RDTSC
#define TickPerMSec 366000.0
#else
#define TickPerMSec 1000000.0L
#endif

class Timer {

  double seconds_;

#ifdef WIN32
  LARGE_INTEGER nFreq_;
  LARGE_INTEGER nBefore_;
  LARGE_INTEGER nAfter_;
  float dwTime_;
#endif
  
public:

  Timer(){
#ifdef WIN32
    memset(&nFreq_,   0x00, sizeof nFreq_);
    memset(&nBefore_, 0x00, sizeof nBefore_);
    memset(&nAfter_,  0x00, sizeof nAfter_);
    dwTime_ = 0;
#endif
  };
  ~Timer(){};

#ifndef WIN32
#ifdef USE_RDTSC
  unsigned long long GetTick(void)
  {
    unsigned int h,l;
    /* read Pentium cycle counter */
    __asm__(".byte 0x0f,0x31" : "=a" (l),"=d" (h));
    return ((unsigned long long int)h<<32)|l;
  }
#else
  unsigned long long GetTick(void)
  {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (unsigned long long)tv.tv_sec*1000000+tv.tv_usec;
  }
#endif
#endif

#ifdef WIN32
  float time() const { return dwTime_; };
#else
  float time() const { return .0f; };
#endif

  void start() {
#ifdef WIN32
    QueryPerformanceFrequency(&nFreq_);
    QueryPerformanceCounter(&nBefore_);    
#endif
  };

  void stop() { stop( false ); };

  void stop( bool flag ) {
#ifdef WIN32
    QueryPerformanceCounter(&nAfter_);
    //dwTime_ = (DWORD)((nAfter_.QuadPart - nBefore_.QuadPart) * 1000 / nFreq_.QuadPart); milisec.
    dwTime_ = ((float) nAfter_.QuadPart - (float) nBefore_.QuadPart) / (float) nFreq_.QuadPart; // sec.
    if ( flag )
      cout << "Ellapsed time: " << dwTime_ << " sec. " << endl;
#endif
  };

  double get_seconds() {
    
#ifdef WIN32
    LARGE_INTEGER freq, timer;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&timer);	

    return (double)timer.QuadPart / (double)freq.QuadPart;
#else	/* linux */

#if 0
    struct timeval t;

    gettimeofday(&t, NULL);

    seconds_ = (double)t.tv_sec + (double)t.tv_usec / (1000 * 1000);
#endif
    
    unsigned long long time0 = GetTick();
    seconds_ = (double) time0 / (TickPerMSec);
#endif

    return seconds_;
  };

  void show_fps() {

    static char	buf[32];
    static double	last_time        = 0.0;
    static double	elapsed_time     = 0.0;
    double		curr_time        = 0.0;
    const double	display_interval = 10.0;
    static int	count            = 0;

    curr_time     = get_seconds();
    elapsed_time += (curr_time - last_time);
    last_time     = curr_time;
    
    count++;

    if (count >= display_interval) {
      sprintf(buf, "FPS: %f", display_interval / elapsed_time);
      printf("FPS = %f\n", display_interval / elapsed_time);
	  
      count        = 0;
      elapsed_time = 0.0;
    }
    //console_output(100, 100, buf);
  };
  
};

#endif // _TIMER_H

    
