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

#include "reallocResourceMachine.h"

reallocResourceMachine::reallocResourceMachine() :
    _capacity(0), _safety_capacity(0), _used_capacity(0),
    _transient_used_capacity(0), _resource(0)
{

}
reallocID
reallocResourceMachine::get_resource_id() const
{
  return _resource->get_id();
}

void
reallocResourceMachine::set_resource(reallocResource * in_resource)
{
  _resource = in_resource;
}

reallocResourceMachine::~reallocResourceMachine()
{

}


int64
reallocResourceMachine::loadCost() const
{
  return
      (_used_capacity > _safety_capacity) ?
          _resource->_weight_low_cost * (_used_capacity - _safety_capacity) : 0;
}

void
reallocResourceMachine::add_used_capacity(int64 sup_cap, bool in_initial)
{
  _used_capacity += sup_cap;
  if (_resource->_transient && in_initial)
    _transient_used_capacity -= sup_cap;
}

int64
reallocResourceMachine::get_capacity() const
{
  return _used_capacity + _transient_used_capacity;
}
