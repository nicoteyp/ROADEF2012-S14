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

#ifndef REALLOCSERVICE_H_
#define REALLOCSERVICE_H_
#include "reallocDefinitions.h"
#include "reallocProcess.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <list>
#include <set>

typedef std::vector<reallocService*> vector_reallocServicePtr;
typedef std::vector<reallocService*>::iterator vectorIterator_reallocServicePtr;
typedef std::vector<reallocService*>::const_iterator vectorConstIterator_reallocServicePtr;
typedef std::vector<reallocProcess*> vector_reallocProcessPtr;
typedef std::vector<reallocProcess*>::iterator vectorIterator_reallocProcessPtr;

class reallocService
{
  friend class lpSolveReallocInstance;
public:
  reallocService(const int nb_services, const unsigned int in_spreadMin = 0);
  ~reallocService();
  void
  load(std::istream &fi, vector_reallocService& vector_services,
      size_t nb_machine, size_t nb_neighbourhoods, size_t nb_locations);
  void
  write(std::ostream & fo = std::cout);
  void
  addProcess(reallocProcess * in_process);
  bool
  neighbourhoodsConstraint();
  bool
  locationsConstraint();
  bool
  conflictConstraint(const reallocID id);
  bool
  locationsConstraint(const reallocProcess& in_process,
      const reallocMachine& in_machine);
  bool
  neighbourhoodsConstraint(const reallocProcess& in_process,
      const reallocMachine& in_machine) const;
  bool
  neighbourhoodsConstraint(const reallocProcess& in_process,
      const reallocID new_neighbourhood) const;

  inline reallocID
  get_id() const
  {
    return _id;
  }
  ;
  void
  set_id();
  unsigned int _spreadMin;
  vector_reallocServicePtr _services_depends;
  vector_reallocServicePtr _services_superiors;
  vector_reallocID _machine_process_id;
  vector_reallocProcessPtr _processes;
  std::vector<std::list<reallocProcess *> > _locations;
  std::list<reallocID> _used_locations;
  std::vector<std::list<reallocProcess *> > _neighbourhoods;
private:
  static reallocID _id_count;
  reallocID _id;

};

#endif
