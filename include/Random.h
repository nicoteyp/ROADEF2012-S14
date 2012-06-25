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

#ifndef Random_h
#define Random_h
#define USE_MT19937

#ifdef __cplusplus
extern "C"
{
#endif

  /*=============================================================================*/
  /** Draw uniformly an integer random value in the range [0,max-1] */
  unsigned int
  RandomInt(unsigned int max);

  /*=============================================================================*/
  /** Draw uniformly a floating number random value in the range [0,max[ */
  double
  RandomFloat(double max);

  /*=============================================================================*/
  /** Seed the random generator */
  void
  SRandom(int seed);

#ifdef __cplusplus
}
#endif

#endif
