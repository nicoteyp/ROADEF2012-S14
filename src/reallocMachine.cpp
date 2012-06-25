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
#include "reallocMachine.h"
#include "reallocProcess.h"

reallocID reallocMachine::_id_count = 0;
reallocMachine::reallocMachine(int in_resources, int in_machines) :
    _machine_resources(in_resources, reallocResourceMachine()),
    _processes_allocation(), _moving_cost(in_machines), _load_cost(0),
    _balance_cost(0), _under_used_ressources(0), _total_cost(0)
{

}
reallocMachine::~reallocMachine()
{

}
void
reallocMachine::write(std::ostream & fo)
{
  fo << "machine" << std::endl;
  fo << _id << " id L : " << _location_id << " id N : " << _neighborhood_id
      << std::endl;
  for (vectorIterator_reallocResourceMachine iter = _machine_resources.begin();
      iter != _machine_resources.end(); ++iter)
    fo << (*iter).get_resource_id() << " Cap " << (*iter)._capacity
        << " S Cap : " << (*iter)._safety_capacity << " Used Cap "
        << (*iter)._used_capacity << " " << (*iter)._transient_used_capacity
        << std::endl;

  for (vectorIterator_int iter = _moving_cost.begin();
      iter != _moving_cost.end(); ++iter)
    fo << *iter << "\t";
  fo << std::endl;

}

bool
reallocMachine::capacitiesConstraint() const
{
  for (vectorConstIterator_reallocResourceMachine iter =
      _machine_resources.begin(); iter != _machine_resources.end(); ++iter)
    if ((*iter).capacityConstraint())
      return false;
  return true;
}

bool
reallocMachine::capacitiesConstraint(const reallocProcess& in_process) const
{
  vectorConstIterator_reallocResourceMachine iter;
  vectorConstIterator_int iter_r = in_process._requirement.begin();
  if (this == in_process.get_initial_allocation())
    {
      for (iter = _machine_resources.begin(); iter != _machine_resources.end();
          ++iter, ++iter_r)
        {
          if ((*iter).get_transient())
            continue;
          if ((*iter).capacityConstraint(*iter_r))
            return false;
        }
    }
  else
    {
      for (iter = _machine_resources.begin(); iter != _machine_resources.end();
          ++iter, ++iter_r)
        {
          if ((*iter).capacityConstraint(*iter_r))
            return false;
        }
    }
  return true;
}

bool
reallocMachine::capacitiesConstraint(const reallocProcess& in_process,
    int64 & deltas) const
{
  if (this == in_process.get_initial_allocation())
    {
      deltas = 0;
      vectorConstIterator_reallocResourceMachine iter;
      vectorConstIterator_int iter_r = in_process._requirement.begin();
      for (iter = _machine_resources.begin(); iter != _machine_resources.end();
          ++iter, ++iter_r)
        {

          if ((*iter).get_transient())
            {
              //deltas += (*iter).deltaLoadCost((*iter_r));

            }
          else if ((*iter).capacityConstraint(*iter_r))
            {
              return false;
            }
//        else
//        {
//          deltas += (*iter).deltaLoadCost((*iter_r));
//        }
        }
      for (iter = _machine_resources.begin(), iter_r =
          in_process._requirement.begin(); iter != _machine_resources.end();
          ++iter, ++iter_r)
        {
          deltas += (*iter).deltaLoadCost((*iter_r));
        }

    }
  else
    {

      deltas = 0;
      vectorConstIterator_reallocResourceMachine iter;
      vectorConstIterator_int iter_r = in_process._requirement.begin();
      for (iter = _machine_resources.begin(); iter != _machine_resources.end();
          ++iter, ++iter_r)
        {
          if ((*iter).capacityConstraint(*iter_r))
            {
              return false;
            }
        }

      for (iter = _machine_resources.begin(), iter_r =
          in_process._requirement.begin(); iter != _machine_resources.end();
          ++iter, ++iter_r)
        {
          deltas += (*iter).deltaLoadCost((*iter_r));
        }

    }
  return true;
}

bool
reallocMachine::capacitiesSwapConstraint(const reallocProcess& in_process1,
    const reallocProcess& in_process2, int64 & deltas) const
{
  if (this == in_process2.get_initial_allocation())
    {
      deltas = 0;
      vectorConstIterator_reallocResourceMachine iter;
      vectorConstIterator_int iter_r1 = in_process1._requirement.begin();
      vectorConstIterator_int iter_r2 = in_process2._requirement.begin();
      for (iter = _machine_resources.begin(); iter != _machine_resources.end();
          ++iter, ++iter_r1, ++iter_r2)
        {

          if ((*iter).get_transient())
            {
              deltas += (*iter).deltaLoadCost((*iter_r2) - (*iter_r1));
              continue;

            }
          if ((*iter).capacityConstraint((*iter_r2) - (*iter_r1)))
            {
              return false;
            }
          else
            {
              deltas += (*iter).deltaLoadCost((*iter_r2) - (*iter_r1));
            }
        }
    }
  else
    {
      if (this == in_process1.get_initial_allocation())
        {
          deltas = 0;
          vectorConstIterator_reallocResourceMachine iter;
          vectorConstIterator_int iter_r1 = in_process1._requirement.begin();
          vectorConstIterator_int iter_r2 = in_process2._requirement.begin();
          for (iter = _machine_resources.begin();
              iter != _machine_resources.end(); ++iter, ++iter_r1, ++iter_r2)
            {
              if ((*iter).get_transient())
                {
                  if ((*iter).capacityConstraint((*iter_r2)))
                    {
                      return false;
                    }
                }
              else if ((*iter).capacityConstraint((*iter_r2) - (*iter_r1)))
                {
                  return false;
                }
              deltas += (*iter).deltaLoadCost((*iter_r2) - (*iter_r1));
            }

        }
      else
        {
          deltas = 0;
          vectorConstIterator_reallocResourceMachine iter;
          vectorConstIterator_int iter_r1 = in_process1._requirement.begin();
          vectorConstIterator_int iter_r2 = in_process2._requirement.begin();
          for (iter = _machine_resources.begin();
              iter != _machine_resources.end(); ++iter, ++iter_r1, ++iter_r2)
            {
              if ((*iter).capacityConstraint((*iter_r2) - (*iter_r1)))
                {
                  return false;
                }
              deltas += (*iter).deltaLoadCost((*iter_r2) - (*iter_r1));
            }
        }
    }
  return true;
}

int64
reallocMachine::loadCost()
{
  _load_cost = 0;
  _under_used_ressources = 0;
  vectorConstIterator_reallocResourceMachine iter_machine_resource;
  for (iter_machine_resource = _machine_resources.begin();
      iter_machine_resource != _machine_resources.end();
      ++iter_machine_resource/*, ++iter_max*/)
    {
      _load_cost += (*iter_machine_resource).loadCost();
      _under_used_ressources += (*iter_machine_resource).get_capacity()
          < (*iter_machine_resource)._safety_capacity;

    }

  return _load_cost;
}

int64
reallocMachine::balanceCost(const vector_reallocBalanceCost& in_balance_costs)
{
  _balance_cost = 0;
  vectorConstIterator_reallocBalanceCost iter_balance_cost;
  for (iter_balance_cost = in_balance_costs.begin();
      iter_balance_cost != in_balance_costs.end(); ++iter_balance_cost)
    _balance_cost += balanceCost((*iter_balance_cost));
  return _balance_cost;
}

void
reallocMachine::update_capacities(const reallocProcess& in_process, int coeff,
    bool initial_machine)
{
  vectorIterator_reallocResourceMachine iter_machine_resource;
  vectorConstIterator_int iter_requirement;
  _under_used_ressources = 0;

  for (iter_machine_resource = _machine_resources.begin(), iter_requirement =
      in_process._requirement.begin();
      iter_machine_resource != _machine_resources.end();
      ++iter_machine_resource, ++iter_requirement/*, ++iter_max*/)
    {
      (*iter_machine_resource).add_used_capacity(coeff * (*iter_requirement),
          initial_machine);
      _under_used_ressources += ((*iter_machine_resource).get_capacity()
          < (*iter_machine_resource)._safety_capacity);
    }

}

int64
reallocMachine::balanceCost(const reallocBalanceCost & in_balance_cost) const
{
  return balanceCost(in_balance_cost, 0, 0);
}

int64
reallocMachine::balanceCost(const reallocBalanceCost & in_balance_cost,
    const int64 add_r1, const int64 add_r2) const
{
  int64 _cost =
      in_balance_cost._target
          * (_machine_resources[in_balance_cost._id_r1]._capacity
              - (add_r1
                  + _machine_resources[in_balance_cost._id_r1]._used_capacity))
          - (_machine_resources[in_balance_cost._id_r2]._capacity
              - (add_r2
                  + _machine_resources[in_balance_cost._id_r2]._used_capacity));
  return (_cost > 0) ? in_balance_cost._cost * _cost : 0;

}

void
reallocMachine::set_id()
{
  _id = reallocMachine::_id_count++;
}

void
reallocMachine::load(std::istream &fi, vector_reallocResource& in_resources

)
{
  set_id();
  fi >> _neighborhood_id;
  fi >> _location_id;

  vectorIterator_reallocResource iter_resource;
  vectorIterator_reallocResourceMachine iter_machine_resource;
  for (iter_machine_resource = _machine_resources.begin(), iter_resource =
      in_resources.begin(); iter_machine_resource != _machine_resources.end();
      ++iter_machine_resource, ++iter_resource)
    (*iter_machine_resource).set_resource(&(*iter_resource));

  for (iter_machine_resource = _machine_resources.begin();
      iter_machine_resource != _machine_resources.end();
      ++iter_machine_resource)
    fi >> (*iter_machine_resource)._capacity;

  for (iter_machine_resource = _machine_resources.begin();
      iter_machine_resource != _machine_resources.end();
      ++iter_machine_resource)
    fi >> (*iter_machine_resource)._safety_capacity;

  for (vectorIterator_int iter = _moving_cost.begin();
      iter != _moving_cost.end(); ++iter)
    fi >> (*iter);
}
