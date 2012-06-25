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

#include "reallocBalanceCost.h"

reallocBalanceCost::reallocBalanceCost(const reallocID in_id_r1,
    const reallocID in_id_r2, const float in_target, const float in_cost) :
    _id_r1(in_id_r1), _id_r2(in_id_r2), _target(in_target), _cost(in_cost)
{

}
reallocBalanceCost::~reallocBalanceCost()
{

}

void
reallocBalanceCost::load(std::istream &fi)
{
  fi >> _id_r1;
  fi >> _id_r2;
  fi >> _target;
  fi >> _cost;
#ifdef REALLOC_DEBUG
  write();
#endif

}

void
reallocBalanceCost::write(std::ostream & fo)
{
  fo << "R1 : " << _id_r1 << " R2 : " << _id_r2 << " target : " << _target
      << " cost : " << _cost << std::endl;

}

