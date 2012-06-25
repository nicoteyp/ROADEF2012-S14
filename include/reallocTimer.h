/******************************************************************************
 Copyright 2012 Nicolas Teypaz, Probayes SAS, http://probayes.com/

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU  General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU  General Public License for more details.

 You should have received a copy of the GNU  General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef REALLOCTIMER_H_
#define REALLOCTIMER_H_

#define REALLOC_USE_SYSTIME

//#if defined(WIN32) || defined(_WIN32)
#if defined(_MSC_VER) // MS VC++
#undef REALLOC_USE_SYSTIME
#endif

#include <ctime>
#ifdef REALLOC_USE_SYSTIME
#include <sys/time.h>
#endif

#include <cstdlib>

class reallocTimer
{
protected:
  double tstart_;
  double tstop_;
#ifdef REALLOC_USE_SYSTIME
  struct timeval tv_;
#endif
  bool activated_;
public:
  inline
  reallocTimer();
  inline void
  start();
  inline void
  stop();
  inline double
  get_time_sec() const;
  inline double
  get_time_msec() const;
  inline void
  activate_for_time_sec(double time_in_sec);
  inline void
  activate_for_time_msec(double time_in_sec);
  inline bool
  is_activated() const;
  inline bool
  expired() const;
  inline bool
  expired(int iteration) const;
  inline void
  reset();
  inline double
  get_passed_time_sec() const;

  static reallocTimer timer;

};

//=============================================================================
inline
reallocTimer::reallocTimer() :
    tstart_(0.0), tstop_(0.0),
#ifdef REALLOC_USE_SYSTIME
        tv_(),
#endif
        activated_(false)
{
}

//=============================================================================
inline void
reallocTimer::reset()
{
  tstart_ = tstop_ = 0.0;
  activated_ = false;
}

//=============================================================================
inline bool
reallocTimer::is_activated() const
{
  return activated_;
}

#ifdef REALLOC_USE_SYSTIME

static inline double
reallocSeconds(timeval const & tv)
{
  return ((double) tv.tv_sec) + ((double) tv.tv_usec) * 1e-6;
}

//=============================================================================
inline void
reallocTimer::start()
{
  gettimeofday(&tv_, 0);
  tstart_ = reallocSeconds(tv_);
}

//=============================================================================
inline void
reallocTimer::stop()
{
  gettimeofday(&tv_, 0);
  tstop_ = reallocSeconds(tv_);
}

//=============================================================================
inline double
reallocTimer::get_time_sec() const
{
  return tstop_ - tstart_;
}

//=============================================================================
inline double
reallocTimer::get_time_msec() const
{
  return (tstop_ - tstart_) * 1000.0;
}

//=============================================================================
inline void
reallocTimer::activate_for_time_sec(double time_in_sec)
{
  gettimeofday(&tv_, 0);
  tstart_ = reallocSeconds(tv_);

  tstop_ = tstart_ + time_in_sec;
  activated_ = true;
}

//=============================================================================
inline void
reallocTimer::activate_for_time_msec(double time_in_msec)
{
  gettimeofday(&tv_, 0);
  tstart_ = reallocSeconds(tv_);

  tstop_ = tstart_ + time_in_msec * 1e-3;
  activated_ = true;
}

//=============================================================================
inline bool
reallocTimer::expired() const
{
  if (!activated_)
    return false;

  struct timeval ltv;
  gettimeofday(&ltv, 0);
  double current_time = reallocSeconds(ltv);

  return (current_time >= tstop_);
}
//=============================================================================
inline bool
reallocTimer::expired(int iteration) const
{
  if (!activated_)
    return false;

  struct timeval ltv;
  gettimeofday(&ltv, 0);
  double current_time = reallocSeconds(ltv);

  return (tstart_ + (iteration + 1) * ((current_time - tstart_) / iteration)
      >= tstop_ || current_time >= tstop_);
}

//=============================================================================
inline double
reallocTimer::get_passed_time_sec() const
{
  struct timeval ltv;
  gettimeofday(&ltv, 0);
  double current_time = reallocSeconds(ltv);
  return (current_time - tstart_);
}

#else
//=============================================================================
inline void reallocTimer::start()
  {
    tstart_ = clock();
  }

//=============================================================================
inline void reallocTimer::stop()
  {
    tstop_ = clock();
  }

//=============================================================================
inline double reallocTimer::get_time_sec() const
  {
    return (tstop_ - tstart_)/CLOCKS_PER_SEC;
  }

//=============================================================================
inline double reallocTimer::get_time_msec() const
  {
    return (tstop_ - tstart_)*1000.0/CLOCKS_PER_SEC;
  }

//=============================================================================
inline void reallocTimer::activate_for_time_sec(double time_in_sec)
  {
    tstart_ = clock();
    tstop_ = tstart_ + time_in_sec*CLOCKS_PER_SEC;
    activated_ = true;
  }

//=============================================================================
inline void reallocTimer::activate_for_time_msec(double time_in_msec)
  {
    tstart_ = clock();
    tstop_ = tstart_ + time_in_msec*1e-3*CLOCKS_PER_SEC;
    activated_ = true;
  }

//=============================================================================
inline bool reallocTimer::expired() const
  {
    if(!activated_) return false;

    return (clock() >= tstop_);
  }

//=============================================================================
inline double reallocTimer::get_passed_time_sec() const
  {
    return (clock() - tstart_);
  }

#endif

#endif
