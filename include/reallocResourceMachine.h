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

#ifndef REALLOCRESOURCEMACHINE_H_
#define REALLOCRESOURCEMACHINE_H_
#include "reallocDefinitions.h"
#include "reallocResource.h"

class reallocResourceMachine
{
public:
  reallocResourceMachine();
  ~reallocResourceMachine();
  inline bool
  capacityConstraint(int64 sup_cap = 0) const
  {
    return _used_capacity + _transient_used_capacity + sup_cap > _capacity;
  }
  ;
  int64
  loadCost() const;
  inline int64
  deltaLoadCost(int64 sup_cap) const
  {

    if (_used_capacity + sup_cap > _safety_capacity)
      {
        if (_used_capacity > _safety_capacity)
          return _resource->_weight_low_cost * sup_cap;
        else
          return _resource->_weight_low_cost
              * (_used_capacity + sup_cap - _safety_capacity);
      }
    else
      {
        if (_used_capacity > _safety_capacity)
          return -_resource->_weight_low_cost
              * (_used_capacity - _safety_capacity);
        else
          return 0;
      }

  }
  reallocID
  get_resource_id() const;
  inline bool
  get_transient() const
  {
    return _resource->_transient;
  }
  ;
  int64
  get_capacity() const;
  void
  set_resource(reallocResource * in_resource);
  void
  add_used_capacity(int64 sup_cap, bool in_initial);
  int64 _capacity;
  int64 _safety_capacity;
  int64 _used_capacity;
  int64 _transient_used_capacity;
private:

  reallocResource * _resource;
};

#endif
