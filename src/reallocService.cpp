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

#include "reallocService.h"

reallocID reallocService::_id_count = 0;

reallocService::reallocService(const int nb_services,
    const unsigned int in_spreadMin) :
    _spreadMin(in_spreadMin), _services_depends(), _services_superiors(),
    _machine_process_id(), _processes(), _locations(), _used_locations(),
    _neighbourhoods()
{
  _services_depends.reserve(nb_services);
  _services_superiors.reserve(nb_services);
  _processes.reserve(REALLOC_PROCESSES);
}
reallocService::~reallocService()
{

}

void
reallocService::write(std::ostream & fo)
{
  fo << _id << " Spread Min : " << _spreadMin << std::endl;
  fo << "Depends : ";
  for (vectorIterator_reallocServicePtr iter = _services_depends.begin();
      iter != _services_depends.end(); ++iter)
    fo << (*iter)->get_id() << "\t";
  fo << std::endl;
  fo << "Process : ";
  for (vectorIterator_reallocProcessPtr iter = _processes.begin();
      iter != _processes.end(); ++iter)
    fo << (*iter)->get_id() << "\t";

  fo << std::endl;
}
void
reallocService::addProcess(reallocProcess * in_process)
{
  _processes.push_back(in_process);
  in_process->set_service(this);
}

bool
reallocService::neighbourhoodsConstraint()
{
  vectorIterator_reallocServicePtr iter_services;
  for (iter_services = _services_depends.begin();
      iter_services != _services_depends.end(); ++iter_services)
    {
      for (unsigned int j = 0; j < _neighbourhoods.size(); ++j)
        if (_neighbourhoods[j].size() > 0
            && (*iter_services)->_neighbourhoods[j].size() == 0)
          return false;

    }

  return true;
}

bool
reallocService::locationsConstraint()
{
  return _used_locations.size() >= _spreadMin;
}

bool
reallocService::locationsConstraint(const reallocProcess& in_process,
    const reallocMachine& in_machine)
{
  if (_used_locations.size() > _spreadMin || _spreadMin == 0)
    {
      return true;
    }
  else
    {
      if (_locations[in_process.get_allocation()->get_location_id()].size() > 1)
        return true;
      else
        {
          if (_locations[in_machine.get_location_id()].size() == 0)
            return true;
          else
            return false;
        }
    }
}

bool
reallocService::neighbourhoodsConstraint(const reallocProcess& in_process,
    const reallocMachine& in_machine) const
{
  return neighbourhoodsConstraint(in_process, in_machine.get_neighborhood_id());
}

bool
reallocService::neighbourhoodsConstraint(const reallocProcess& in_process,
    const reallocID new_neighbourhood) const
{
  reallocID old_neighbourhood =
      in_process.get_allocation()->get_neighborhood_id();
  if (_neighbourhoods[old_neighbourhood].size() == 1)
    {
      for (vectorConstIterator_reallocServicePtr iter =
          _services_superiors.begin(); iter != _services_superiors.end();
          ++iter)
        if ((*iter)->_neighbourhoods[old_neighbourhood].size() > 0)
          return false;

    }
  if (_neighbourhoods[new_neighbourhood].size() == 0)
    {
      for (vectorConstIterator_reallocServicePtr iter =
          _services_depends.begin(); iter != _services_depends.end(); ++iter)
        if ((*iter)->_neighbourhoods[new_neighbourhood].size() == 0)
          return false;
    }

  return true;

}

bool
reallocService::conflictConstraint(const reallocID id)
{
  return _machine_process_id[id] == -1;
}

void
reallocService::set_id()
{
  _id = reallocService::_id_count++;
}

void
reallocService::load(std::istream &fi, vector_reallocService& vector_services,
    size_t nb_machine, size_t nb_neighbourhoods, size_t nb_locations)
{
  set_id();
  fi >> _spreadMin;
  int size_depends;
  fi >> size_depends;
  _services_depends.resize(size_depends);
  _machine_process_id.resize(nb_machine, -1);
  _neighbourhoods.resize(nb_neighbourhoods);
  _locations.resize(nb_locations);
  vectorIterator_reallocServicePtr iter_services;
  reallocID id_depends;
  for (iter_services = _services_depends.begin();
      iter_services != _services_depends.end(); ++iter_services)
    {
      fi >> id_depends;
      (*iter_services) = &vector_services[id_depends];
      vector_services[id_depends]._services_superiors.push_back(this);
    }
}
