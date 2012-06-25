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
#include "reallocProcess.h"
#include "reallocService.h"

reallocID reallocProcess::_id_count = 0;

reallocProcess::reallocProcess(const int nb_resources, const float in_move_cost) :
    _requirement(nb_resources), _move_cost(in_move_cost), _load_cost(0),
    _delta_cost(0), _initial_allocation(NULL), _allocation(NULL),
    _service(NULL), _iteration(-REALLOC_PROCESSES)

{

}

reallocProcess::~reallocProcess()
{
}
void
reallocProcess::write(std::ostream & fo)
{
  fo << "service" << std::endl;
  fo << _id << " id S : " << get_id_service() << " move cost : " << _move_cost
      << std::endl;
  fo << " id_initial_allocation : " << get_id_initial_allocation() << std::endl;
  for (vectorIterator_int iter = _requirement.begin();
      iter != _requirement.end(); ++iter)
    fo << *iter << "\t";
  fo << std::endl;

}

void
reallocProcess::allocation(reallocMachine* in_machine)
{
  if (_allocation != NULL)
    {
      _service->_locations[_allocation->get_location_id()].remove(this);
      _service->_neighbourhoods[_allocation->get_neighborhood_id()].remove(
          this);
      if (_service->_locations[_allocation->get_location_id()].size() == 0)
        _service->_used_locations.remove(_allocation->get_location_id());
      _service->_machine_process_id[_allocation->get_id()] = -1;
    }

  _allocation = in_machine;
  _service->_locations[_allocation->get_location_id()].push_back(this);
  _service->_neighbourhoods[_allocation->get_neighborhood_id()].push_back(this);
  _service->_machine_process_id[_allocation->get_id()] = _id;

  if (_service->_locations[_allocation->get_location_id()].size() == 1)
    _service->_used_locations.push_back(_allocation->get_location_id());

}

void
reallocProcess::set_initial_allocation(reallocMachine* in_allocation)
{
  _initial_allocation = in_allocation;
}
void
reallocProcess::set_service(reallocService* in_service)
{
  _service = in_service;
}

void
reallocProcess::set_id()
{
  _id = reallocProcess::_id_count++;
}

void
reallocProcess::load(std::istream &fi, vector_reallocService& vector_services)
{
  set_id();
  reallocID id_service;
  fi >> id_service;
  vector_services[id_service].addProcess(this);
  for (vectorIterator_int iter = _requirement.begin();
      iter != _requirement.end(); ++iter)
    fi >> *iter;
  fi >> _move_cost;
}

void
reallocProcess::set_iteration(const int64 in_iteration)
{
  _iteration = in_iteration;
}

reallocID
reallocProcess::get_id_service() const
{
  return _service->get_id();
}
