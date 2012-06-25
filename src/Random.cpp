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

#include <Random.h>
#include <Mt19937.h>
#include <cstdlib>			// standard C++ includes
#include <math.h>			// math routines
static bool NormalRandom_use_z2 = false;
#define RANDOM_ONE 1
static const float RAND_MAX_PLUS_ONE = float(RAND_MAX) + RANDOM_ONE;
//=============================================================================
void
SRandom(int seed)
{
  NormalRandom_use_z2 = false;
#ifdef USE_MT19937
  Mt19937::generator_.init_genrand(seed);
#else // not USE_MT19937 (Default compiler random generator rand() )
  srand( seed );
#endif
}

/*=============================================================================*/
/** Draw uniformly an integer random value in the range [0,max-1] */
unsigned int
RandomInt(unsigned int max)
{
#ifdef USE_MT19937
  const unsigned int res = Mt19937::generator_.genrand_int32() % max;
  return res;
#else
  return (unsigned int)(max* (rand()/RAND_MAX_PLUS_ONE));
#endif

}

/*=============================================================================*/
/** Draw uniformly a floating number random value in the range [0,max[ */
double
RandomFloat(double max)
{
#ifdef USE_MT19937
  const double res = max * Mt19937::generator_.genrand_res53();
  return res;
#else
  return max*rand()/RAND_MAX_PLUS_ONE;
#endif
}

// Local Variables:
// mode: c++
// indent-tabs-mode: nil
// c-basic-offset: 2
// fill-column: 79
// End:
