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

#ifndef REALLOCDEFINITIONS_H_
#define REALLOCDEFINITIONS_H_

#include <float.h>
// for math.h under visual studio for finding M_PI
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>

typedef int reallocID;
typedef long long int int64;

#define REALLOC_MAX_REAL     FLT_MAX
#define REALLOC_MIN_REAL     FLT_MIN

#define REALLOC_MACHINES	5000
#define REALLOC_RESOURCES	20
#define REALLOC_PROCESSES	50000
#define REALLOC_SERVICES	5000
#define REALLOC_LOCATIONS	1000
#define	REALLOC_NEIGHBORHOODS	1000
#define REALLOC_DEPENDENCIES	5000
#define REALLOC_BALANCE_COSTS	10

typedef std::vector<int> vector_int;
typedef std::vector<int>::iterator vectorIterator_int;
typedef std::vector<int>::const_iterator vectorConstIterator_int;
typedef std::vector<int64> vector_int64;
typedef std::vector<int64>::iterator vectorIterator_int64;
typedef std::vector<int64>::const_iterator vectorConstIterator_int64;
typedef std::vector<reallocID> vector_reallocID;
typedef std::vector<reallocID>::iterator vectorIterator_reallocID;
typedef std::vector<bool> vector_bool;
typedef std::vector<bool>::iterator vectorIterator_bool;
typedef std::vector<double> vector_double;
typedef std::vector<double>::iterator vectorIterator_double;
typedef std::vector<double>::const_iterator vectorConstIterator_double;

#endif /*REALLOCDEFINITIONS_H_*/
