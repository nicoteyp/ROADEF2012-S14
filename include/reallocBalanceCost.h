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

#ifndef REALLOCBALANCECOST_H_
#define REALLOCBALANCECOST_H_
#include "reallocDefinitions.h"

class reallocBalanceCost
{
public:
  reallocBalanceCost(const reallocID in_id_r1 = -1, const reallocID in_id_r2 =
      -1, const float in_target = 0, const float in_cost = 0);
  ~reallocBalanceCost();
  void
  load(std::istream &fi);
  void
  write(std::ostream & fo = std::cout);
  reallocID _id_r1;
  reallocID _id_r2;
  int _target;
  int _cost;

};

typedef std::vector<reallocBalanceCost> vector_reallocBalanceCost;
typedef std::vector<reallocBalanceCost>::iterator vectorIterator_reallocBalanceCost;
typedef std::vector<reallocBalanceCost>::const_iterator vectorConstIterator_reallocBalanceCost;
#endif
