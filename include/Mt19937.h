/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   3. The names of its contributors may not be used to endorse or promote 
   products derived from this software without specific prior written 
   permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#ifndef Mt19937_h
#define Mt19937_h



/* Period parameters */  
#define MT_N 624
#define MT_M 397
#define MT_MATRIX_A 0x9908b0dfU   /* constant vector a */
#define MT_UPPER_MASK 0x80000000U /* most significant w-r bits */
#define MT_LOWER_MASK 0x7fffffffU /* least significant r bits */

class Mt19937
{

private:
  unsigned int mt_[MT_N]; /* the array for the state vector  */
  int mti_; /* mti==MT_N+1 means mt[MT_N] is not initialized */
  unsigned int mag01_[2];
  
private:
  Mt19937(const Mt19937&); // NI
  Mt19937 &operator=(const Mt19937&); // NI

public:
  static Mt19937 generator_;

public:

  inline Mt19937()
    : mti_(MT_N+1)
  {
    mag01_[0]= 0x0U;
    mag01_[1]= MT_MATRIX_A;

    init_genrand(5489U);
  }

  /* initializes mt[MT_N] with a seed */
  inline void init_genrand(unsigned int s)
  {
    mt_[0]= s & 0xffffffffU;
    for (mti_=1; mti_<MT_N; ++mti_) {
      mt_[mti_] = 
        (1812433253U * (mt_[mti_-1] ^ (mt_[mti_-1] >> 30)) + mti_); 
      /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
      /* In the previous versions, MSBs of the seed affect   */
      /* only MSBs of the array mt[].                        */
      /* 2002/01/09 modified by Makoto Matsumoto             */
      mt_[mti_] &= 0xffffffffU;
      /* for >32 bit machines */
    }
  }

  /* initialize by an array with array-length */
  /* init_key is the array for initializing keys */
  /* key_length is its length */
  /* slight change for C++, 2004/2/26 */
  inline void init_by_array(unsigned int init_key[], int key_length)
  {
    int i, j, k;
    init_genrand(19650218U);
    i=1; j=0;
    k = (MT_N>key_length ? MT_N : key_length);
    for (; k; k--) {
      mt_[i] = (mt_[i] ^ ((mt_[i-1] ^ (mt_[i-1] >> 30)) * 1664525U))
        + init_key[j] + j; /* non linear */
      mt_[i] &= 0xffffffffU; /* for WORDSIZE > 32 machines */
      ++i; ++j;
      if (i>=MT_N) { mt_[0] = mt_[MT_N-1]; i=1; }
      if (j>=key_length) j=0;
    }
    for (k=MT_N-1; k; k--) {
      mt_[i] = (mt_[i] ^ ((mt_[i-1] ^ (mt_[i-1] >> 30)) * 1566083941U))
        - i; /* non linear */
      mt_[i] &= 0xffffffffU; /* for WORDSIZE > 32 machines */
      i++;
      if (i>=MT_N) { mt_[0] = mt_[MT_N-1]; i=1; }
    }

    mt_[0] = 0x80000000U; /* MSB is 1; assuring non-zero initial array */ 
  }

  /* generates a random number on [0,0xffffffff]-interval */
  inline unsigned int genrand_int32(void)
  {
    unsigned int y;
    
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti_ >= MT_N) { /* generate MT_N words at one time */

      int kk;

      //if (mti_ == MT_N+1)   /* if init_genrand() has not been called, */
      //init_genrand(5489U); /* a default initial seed is used */

      for (kk=0;kk<MT_N-MT_M;++kk) {
        y = (mt_[kk]&MT_UPPER_MASK)|(mt_[kk+1]&MT_LOWER_MASK);
        mt_[kk] = mt_[kk+MT_M] ^ (y >> 1) ^ mag01_[y & 0x1U];
      }
      for (;kk<MT_N-1;++kk) {
        y = (mt_[kk]&MT_UPPER_MASK)|(mt_[kk+1]&MT_LOWER_MASK);
        mt_[kk] = mt_[kk+(MT_M-MT_N)] ^ (y >> 1) ^ mag01_[y & 0x1U];
      }
      y = (mt_[MT_N-1]&MT_UPPER_MASK)|(mt_[0]&MT_LOWER_MASK);
      mt_[MT_N-1] = mt_[MT_M-1] ^ (y >> 1) ^ mag01_[y & 0x1U];

      mti_ = 0;
    }
  
    y = mt_[mti_++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680U;
    y ^= (y << 15) & 0xefc60000U;
    y ^= (y >> 18);

    return y;
  }

  /* generates a random number on [0,0x7fffffff]-interval */
  inline int genrand_int31(void)
  {
    return (int)(genrand_int32()>>1);
  }

  /* generates a random number on [0,1]-real-interval */
  inline double genrand_real1(void)
  {
    return genrand_int32()*(1.0/4294967295.0); 
    /* divided by 2^32-1 */ 
  }

  /* generates a random number on [0,1)-real-interval */
  inline double genrand_real2(void)
  {
    return genrand_int32()*(1.0/4294967296.0); 
    /* divided by 2^32 */
  }

  /* generates a random number on (0,1)-real-interval */
  inline double genrand_real3(void)
  {
    return (((double)genrand_int32()) + 0.5)*(1.0/4294967296.0); 
    /* divided by 2^32 */
  }

  /* generates a random number on [0,1) with 53-bit resolution*/
  inline double genrand_res53(void) 
  { 
    unsigned int a=genrand_int32()>>5, b=genrand_int32()>>6; 
    return(a*67108864.0+b)*(1.0/9007199254740992.0); 
  } 
  /* These real versions are due to Isaku Wada, 2002/01/09 added */


};

#endif

// Local Variables:
// mode: c++
// indent-tabs-mode: nil
// c-basic-offset: 2
// fill-column: 79
// End:
