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

#include "reallocInstance.h"

reallocInstance::reallocInstance() :
    _cost(0), _max_move_service(0), _max_service(), _resources(), _machines(),
    _services(), _processes(), _balance_costs(), _nb_locations(0),
    _nb_neighbourhoods(0), _solution()

{
  _resources.reserve(REALLOC_RESOURCES);
  _machines.reserve(REALLOC_MACHINES);
  _services.reserve(REALLOC_SERVICES);
  _max_service.reserve(REALLOC_SERVICES);
  _processes.reserve(REALLOC_PROCESSES);
  _balance_costs.reserve(REALLOC_BALANCE_COSTS);
  _solution.reserve(REALLOC_PROCESSES);

}

reallocInstance::~reallocInstance()
{
}

void
reallocInstance::loadFile(std::istream &fi)
{
  _load_resources(fi);
  _load_machines(fi);
  _load_services(fi);
  _load_processes(fi);
  _load_balance_costs(fi);
  fi >> _weight_process_move;
  fi >> _weight_service_move;
  fi >> _weight_machine_move;
#ifdef REALLOC_DEBUG
  std::cout << "weight_process_move : " << _weight_process_move << std::endl;
  std::cout << "weight_service_move : " << _weight_service_move << std::endl;
  std::cout << "weight_machine_move : " << _weight_machine_move << std::endl;
#endif
}

void
reallocInstance::_load_resources(std::istream &fi)
{
  fi >> _nb_resources;
#ifdef REALLOC_DEBUG
  std::cout << "nb_resources : " << _nb_resources << std::endl;
#endif
  _resources.resize(_nb_resources, reallocResource());
  for (vectorIterator_reallocResource iter = _resources.begin();
      iter != _resources.end(); ++iter)
    {
      (*iter).load(fi);
    }
}
void
reallocInstance::_load_machines(std::istream &fi)
{
  fi >> _nb_machines;
#ifdef REALLOC_DEBUG
  std::cout << "nb_machines : " << _nb_machines << std::endl;
#endif
  _machines.resize(_nb_machines, reallocMachine(_nb_resources, _nb_machines));

  for (vectorIterator_reallocMachine iter_machine = _machines.begin();
      iter_machine != _machines.end(); ++iter_machine)
    {
      (*iter_machine).load(fi, _resources);
      _nb_locations = std::max(_nb_locations, (*iter_machine)._location_id + 1);
      _nb_neighbourhoods = std::max(_nb_neighbourhoods,
          (*iter_machine)._neighborhood_id + 1);
    }
}

void
reallocInstance::_load_services(std::istream &fi)
{
  fi >> _nb_services;
#ifdef REALLOC_DEBUG
  std::cout << "nb_services : " << _nb_services << std::endl;
#endif
  _services.resize(_nb_services, reallocService(_nb_services));
  _max_service.resize(_nb_services, 0);
  for (vectorIterator_reallocService iter = _services.begin();
      iter != _services.end(); ++iter)
    {
      (*iter).load(fi, _services, _nb_machines, _nb_neighbourhoods,
          _nb_locations);
    }
}
void
reallocInstance::_load_processes(std::istream &fi)
{
  fi >> _nb_processes;
#ifdef REALLOC_DEBUG
  std::cout << "nb_processes : " << _nb_processes << std::endl;
#endif
  _processes.resize(_nb_processes, reallocProcess(_nb_resources));
  for (vectorIterator_reallocProcess iter = _processes.begin();
      iter != _processes.end(); ++iter)
    {
      (*iter).load(fi, _services);
    }
  _solution.resize(_nb_processes);

  for (vectorIterator_reallocProcess iter_p = _processes.begin();
      iter_p != _processes.end(); ++iter_p)
    {
      vectorIterator_reallocResource iter_r = _resources.begin();
      for (vectorIterator_int iter = (*iter_p)._requirement.begin();
          iter != (*iter_p)._requirement.end(); ++iter, ++iter_r)
        {
          (*iter_p)._load_cost += (*iter_r)._weight_low_cost * (*iter);
        }
    }

}

void
reallocInstance::_load_balance_costs(std::istream &fi)
{
  fi >> _nb_balance_costs;
#ifdef REALLOC_DEBUG
  std::cout << "nb_balance_costs : " << _nb_balance_costs << std::endl;
#endif
  _balance_costs.resize(_nb_balance_costs);

  for (vectorIterator_reallocBalanceCost iter = _balance_costs.begin();
      iter != _balance_costs.end(); ++iter)
    {
      (*iter).load(fi);
    }
}
void
reallocInstance::loadInitialAffectation(std::istream &fi)
{
  reallocID id;
  vectorIterator_reallocProcess iter;
  vectorIterator_reallocID iter_solution;
  for (iter = _processes.begin(), iter_solution = _solution.begin();
      iter != _processes.end(); ++iter, ++iter_solution)
    {
      fi >> id;
      reallocMachine * machine = &_machines[id];
      (*iter).set_initial_allocation(machine);
      (*iter).allocation(machine);
      machine->_processes_allocation.push_back(&(*iter));
      (*iter_solution) = id;

    }

  vectorIterator_reallocResourceMachine iter_machine_resource;
  vectorIterator_int iter_requirement;
  for (iter = _processes.begin(); iter != _processes.end(); ++iter)
    {
      for (iter_machine_resource =
          (*iter).get_initial_allocation()->_machine_resources.begin(), iter_requirement =
          (*iter)._requirement.begin();
          iter_machine_resource
              != (*iter).get_initial_allocation()->_machine_resources.end();
          ++iter_machine_resource, ++iter_requirement)
        (*iter_machine_resource)._used_capacity += (*iter_requirement);

    }
  _cost = _loadCost() + _balanceCost();
#ifdef REALLOC_DEBUG
  std::cout << "Processes : " << std::endl;
  for (int i = 0; i < _nb_processes; i++)
  _processes[i].write();
  std::cout << "Machines : " << std::endl;
  for (int i = 0; i < _nb_machines; i++)
  _machines[i].write();
  std::cout << "Services : " << std::endl;
  for (int i = 0; i < _nb_services; i++)
  _services[i].write();
  std::cout << "load cost: " << _loadCost() << std::endl;
  std::cout << "balance cost: " << _balanceCost() << std::endl;
  std::cout << "initial cost: " << _cost << std::endl;
#endif

}

void
reallocInstance::saveSolution(std::ostream &fo)
{
  for (vectorIterator_int iter = _solution.begin(); iter != _solution.end();
      ++iter)
    fo << *iter << " ";
}

int64
reallocInstance::_loadCost()
{
  int64 sum = 0;
  vectorIterator_reallocMachine iter_machine;
  vectorIterator_reallocResourceMachine iter_machine_resource;
  for (iter_machine = _machines.begin(); iter_machine != _machines.end();
      ++iter_machine)
    sum += (*iter_machine).loadCost();
  return sum;
}

int64
reallocInstance::_balanceCost()
{
  int64 sum = 0;
  vectorIterator_reallocMachine iter_machine;
  vectorIterator_reallocBalanceCost iter_balance_cost;
  for (iter_machine = _machines.begin(); iter_machine != _machines.end();
      ++iter_machine)
    sum += (*iter_machine).balanceCost(_balance_costs);
  return sum;
}

int64
reallocInstance::_deltaReallocationCost(const reallocProcess& in_process,
    const reallocMachine& in_machine) const
{
  const reallocMachine* current_machine = in_process.get_allocation();
  const reallocMachine* initial_machine = in_process.get_initial_allocation();
  const reallocMachine* new_machine = &in_machine;
  int64 delta = 0;
  delta += _weight_machine_move
      * (initial_machine->_moving_cost[in_machine.get_id()]
          - initial_machine->_moving_cost[current_machine->get_id()]);
#ifdef REALLOC_DEBUG
  std::cout << "machine_move :"
  << _weight_machine_move
  * (initial_machine->_moving_cost[in_machine.get_id()]
      - initial_machine->_moving_cost[current_machine->get_id()])
  << std::endl;
#endif
  if (initial_machine != new_machine && initial_machine == current_machine)
    {
#ifdef REALLOC_DEBUG
      std::cout << "process_move : " << _weight_process_move
      * in_process._move_cost << std::endl;
#endif
      delta += _weight_process_move * in_process.get_move_cost();
    }
  else if (initial_machine == new_machine && initial_machine != current_machine)
    {
#ifdef REALLOC_DEBUG
      std::cout << "process_move : " << -_weight_process_move
      * in_process._move_cost << std::endl;
#endif
      delta -= _weight_process_move * in_process.get_move_cost();
    }
  return delta;
}

int64
reallocInstance::_deltaServiveCost(const reallocProcess& in_process,
    const reallocMachine& in_machine) const
{
  const reallocMachine* current_machine = in_process.get_allocation();
  const reallocMachine* initial_machine = in_process.get_initial_allocation();
  const reallocMachine* new_machine = &in_machine;
  int64 delta = 0;
  if (initial_machine != new_machine && initial_machine == current_machine)
    {
#ifdef REALLOC_DEBUG
      std::cout << "service_move : "
      << ((_max_service[in_process.get_id_service()]
              == _max_move_service) ? _weight_service_move : 0)
      << std::endl;
#endif
      delta +=
          (_max_service[in_process.get_id_service()] == _max_move_service) ?
              _weight_service_move : 0;
    }
  else if (initial_machine == new_machine && initial_machine != current_machine)
    {
      vectorConstIterator_int iter_process_service = _max_service.begin()
          + in_process.get_id_service();
      int new_max = (*iter_process_service) - 1;
      for (vectorConstIterator_int iter = _max_service.begin();
          iter != _max_service.end(); ++iter)
        if (iter != iter_process_service && (*iter) > new_max)
          new_max = (*iter);

#ifdef REALLOC_DEBUG
      std::cout << "service_move : " << -_weight_service_move
      * (_max_move_service - new_max) << std::endl;
#endif
      delta -= _weight_service_move * (_max_move_service - new_max);
    }
  return delta;
}

int64
reallocInstance::_deltaAddBalanceCost(const reallocProcess& in_process,
    const reallocMachine& in_machine) const
{
  int64 sum = 0;
  for (vectorConstIterator_reallocBalanceCost iter = _balance_costs.begin();
      iter != _balance_costs.end(); ++iter)
    {
      sum += in_machine.balanceCost(*iter,
          in_process._requirement[(*iter)._id_r1],
          in_process._requirement[(*iter)._id_r2]);
    }
  return sum - in_machine._balance_cost;
}

int64
reallocInstance::_deltaRemoveBalanceCost(const reallocProcess& in_process,
    const reallocMachine& in_machine) const
{
  int64 sum = 0;
  for (vectorConstIterator_reallocBalanceCost iter = _balance_costs.begin();
      iter != _balance_costs.end(); ++iter)
    {
      sum += in_machine.balanceCost(*iter,
          -1 * in_process._requirement[(*iter)._id_r1],
          -1 * in_process._requirement[(*iter)._id_r2]);
    }
  return sum - in_machine._balance_cost;
}

int64
reallocInstance::_deltaRemoveLoadCost(const reallocProcess& in_process,
    const reallocMachine& in_machine) const
{
  int64 sum = 0;
  vectorConstIterator_reallocResourceMachine iter_machine_resource;
  vectorConstIterator_int iter_requirement;
  for (iter_machine_resource = in_machine._machine_resources.begin(), iter_requirement =
      in_process._requirement.begin();
      iter_machine_resource != in_machine._machine_resources.end();
      ++iter_machine_resource, ++iter_requirement)
    sum += (*iter_machine_resource).deltaLoadCost(-1 * (*iter_requirement));

  return sum;
}

void
reallocInstance::_moveSolution(reallocProcess& in_process,
    reallocMachine& in_machine, const int64 in_deltaCost)
{
  reallocMachine * current_machine = in_process.get_allocation();
  reallocMachine* initial_machine = in_process.get_initial_allocation();

  current_machine->update_capacities(in_process, -1,
      current_machine == initial_machine);

  in_machine.update_capacities(in_process, 1, (&in_machine == initial_machine));

  if (initial_machine != &in_machine && initial_machine == current_machine)
    {
      _max_service[in_process.get_id_service()]++;
      _max_move_service =
          (_max_move_service > _max_service[in_process.get_id_service()]) ?
              _max_move_service : _max_service[in_process.get_id_service()];
    }
  else if (initial_machine == &in_machine && initial_machine != current_machine)
    {
      _max_service[in_process.get_id_service()]--;
      _max_move_service = _max_service[in_process.get_id_service()];
      for (vectorIterator_int iter = _max_service.begin();
          iter != _max_service.end(); ++iter)
        if ((*iter) > _max_move_service)
          _max_move_service = (*iter);
    }

  _cost += in_deltaCost;
  in_process.allocation(&in_machine);
  _solution[in_process.get_id()] = in_machine.get_id();

}

void
reallocInstance::_moveSwapSolution(reallocProcess& in_process,
    reallocMachine& in_machine)
{
  reallocMachine * current_machine = in_process.get_allocation();
  reallocMachine* initial_machine = in_process.get_initial_allocation();

  current_machine->update_capacities(in_process, -1,
      current_machine == initial_machine);

  in_machine.update_capacities(in_process, 1, (&in_machine == initial_machine));

  if (initial_machine != &in_machine && initial_machine == current_machine)
    {
      _max_service[in_process.get_id_service()]++;
      _max_move_service =
          (_max_move_service > _max_service[in_process.get_id_service()]) ?
              _max_move_service : _max_service[in_process.get_id_service()];
    }
  else if (initial_machine == &in_machine && initial_machine != current_machine)
    {
      _max_service[in_process.get_id_service()]--;
      _max_move_service = _max_service[in_process.get_id_service()];
      for (vectorIterator_int iter = _max_service.begin();
          iter != _max_service.end(); ++iter)
        if ((*iter) > _max_move_service)
          _max_move_service = (*iter);
    }
  in_process.allocation(&in_machine);
  _solution[in_process.get_id()] = in_machine.get_id();

}

void
reallocInstance::constraintDelta(const reallocProcess& in_process,
    const reallocMachine& in_machine, bool & _move, int64 & _deltas) const
{
  _move = in_machine.capacitiesConstraint(in_process, _deltas);
  if (_move)
    _deltas += _deltaAddBalanceCost(in_process, in_machine);
}

bool
reallocInstance::constraintSwapDelta(const reallocProcess& in_process1,
    const reallocProcess& in_process2, int64 & deltas) const
{
  const reallocMachine* in_machine1 = in_process1.get_allocation();
  const reallocMachine* in_machine2 = in_process2.get_allocation();
  vectorConstIterator_reallocResourceMachine iter1 =
      in_machine1->_machine_resources.begin();
  vectorConstIterator_reallocResourceMachine iter2 =
      in_machine2->_machine_resources.begin();
  vectorConstIterator_int iter_r1 = in_process1._requirement.begin();
  vectorConstIterator_int iter_r2 = in_process2._requirement.begin();
  for (; iter1 != in_machine1->_machine_resources.end();
      ++iter1, ++iter2, ++iter_r1, ++iter_r2)
    {

      if ((*iter1).get_transient())
        {
          if (in_machine1 == in_process2.get_initial_allocation())
            {
              //nothing
            }
          else if (in_machine1 == in_process1.get_initial_allocation()
              && (*iter1).capacityConstraint((*iter_r2)))
            {
              return false;
            }
          else if ((*iter1).capacityConstraint((*iter_r2) - (*iter_r1)))
            {
              return false;
            }

          if (in_machine2 == in_process1.get_initial_allocation())
            {
              //nothing
            }
          else if (in_machine2 == in_process2.get_initial_allocation()
              && (*iter2).capacityConstraint((*iter_r1)))
            {
              return false;
            }
          else if ((*iter2).capacityConstraint((*iter_r1) - (*iter_r2)))
            {
              return false;
            }
        }
      else
        {
          if ((*iter1).capacityConstraint((*iter_r2) - (*iter_r1)))
            {
              return false;
            }
          else if ((*iter2).capacityConstraint((*iter_r1) - (*iter_r2)))
            {
              return false;
            }
        }
    }

  iter1 = in_machine1->_machine_resources.begin();
  iter2 = in_machine2->_machine_resources.begin();
  iter_r1 = in_process1._requirement.begin();
  iter_r2 = in_process2._requirement.begin();
  for (; iter1 != in_machine1->_machine_resources.end();
      ++iter1, ++iter2, ++iter_r1, ++iter_r2)
    {
      deltas += (*iter1).deltaLoadCost((*iter_r2) - (*iter_r1));
      deltas += (*iter2).deltaLoadCost((*iter_r1) - (*iter_r2));
    }
  deltas += _deltaSwapBalanceCost(in_process1, in_process2);
  return true;

}

int64
reallocInstance::_deltaSwapBalanceCost(const reallocProcess& in_process1,
    const reallocProcess& in_process2) const
{
  int64 sum = 0;
  for (vectorConstIterator_reallocBalanceCost iter = _balance_costs.begin();
      iter != _balance_costs.end(); ++iter)
    {
      sum += in_process1.get_allocation()->balanceCost(*iter,
          in_process2._requirement[(*iter)._id_r1]
              - in_process1._requirement[(*iter)._id_r1],
          in_process2._requirement[(*iter)._id_r2]
              - in_process1._requirement[(*iter)._id_r2]);
      sum += in_process2.get_allocation()->balanceCost(*iter,
          in_process1._requirement[(*iter)._id_r1]
              - in_process2._requirement[(*iter)._id_r1],
          in_process1._requirement[(*iter)._id_r2]
              - in_process2._requirement[(*iter)._id_r2]);
    }
  return sum - in_process1.get_allocation()->_balance_cost
      - in_process2.get_allocation()->_balance_cost;
}

void
reallocInstance::constraintSwapDelta(const reallocMachine& in_machine,
    const reallocProcess& in_process1, const reallocProcess& in_process2,
    bool & _move, int64 & _deltas) const
{
  int64 delta1;
  _move = in_machine.capacitiesSwapConstraint(in_process1, in_process2, delta1);
  _deltas += delta1;
  if (_move)
    _deltas += _deltaSwapBalanceCost(in_process1, in_process2, in_machine);

}
int64
reallocInstance::_deltaSwapBalanceCost(const reallocProcess& in_process1,
    const reallocProcess& in_process2, const reallocMachine& in_machine) const
{
  int64 sum = 0;
  for (vectorConstIterator_reallocBalanceCost iter = _balance_costs.begin();
      iter != _balance_costs.end(); ++iter)
    {
      sum += in_machine.balanceCost(*iter,
          in_process2._requirement[(*iter)._id_r1]
              - in_process1._requirement[(*iter)._id_r1],
          in_process2._requirement[(*iter)._id_r2]
              - in_process1._requirement[(*iter)._id_r2]);
    }
  return sum - in_machine._balance_cost;
}

void
reallocInstance::_deltaCost(reallocMachine& in_machine)
{
  in_machine.balanceCost(_balance_costs);
  for (setIterator_ReallocProcess iter_process =
      in_machine._processes_allocation.begin();
      iter_process != in_machine._processes_allocation.end(); ++iter_process)
    {
      (*iter_process)->_delta_cost = _deltaRemoveLoadCost(*(*iter_process),
          in_machine) + _deltaRemoveBalanceCost(*(*iter_process), in_machine);
    }

}

bool
reallocInstance::_serviceConstraint(const reallocProcess& in_process,
    const reallocMachine& in_machine) const
{
  return (in_process.get_service()->locationsConstraint(in_process, in_machine));
}
