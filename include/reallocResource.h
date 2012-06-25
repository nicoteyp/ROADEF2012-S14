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

#ifndef REALLOCRESOURCE_H_
#define REALLOCRESOURCE_H_
#include "reallocDefinitions.h"

class reallocResource
{
  friend class reallocInstance;
public:
  reallocResource(const bool in_transient = true, const float in_weight = 0);
  ~reallocResource();
  void
  load(std::istream &fi);
  void
  write(std::ostream & fo = std::cout);
  inline reallocID
  get_id() const
  {
    return _id;
  }
  ;
  void
  set_id();
  bool _transient;
  int _weight_low_cost;

private:
  static reallocID _id_count;
  reallocID _id;

};

typedef std::vector<reallocResource> vector_reallocResource;
typedef std::vector<reallocResource>::iterator vectorIterator_reallocResource;

#endif
