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

#include "reallocResource.h"

reallocID reallocResource::_id_count = 0;

reallocResource::reallocResource(const bool in_transient, const float in_weight) :
    _transient(in_transient), _weight_low_cost(in_weight)
{

}
reallocResource::~reallocResource()
{

}

void
reallocResource::write(std::ostream &fo)
{
  fo << _id << " TR : " << _transient << " cost : " << _weight_low_cost
      << std::endl;
}

void
reallocResource::set_id()
{
  _id = reallocResource::_id_count++;
}

void
reallocResource::load(std::istream &fi)
{
  set_id();
  fi >> _transient;
  fi >> _weight_low_cost;
#ifdef REALLOC_DEBUG
  write();
#endif
}
