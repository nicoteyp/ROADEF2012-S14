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

#ifndef REALLOCMACHINE_H_
#define REALLOCMACHINE_H_
#include "reallocDefinitions.h"
#include "reallocResourceMachine.h"
#include "reallocBalanceCost.h"
#include <list>
class reallocProcess;

typedef std::vector<reallocResourceMachine> vector_reallocResourceMachine;
typedef std::vector<reallocResourceMachine>::iterator vectorIterator_reallocResourceMachine;
typedef std::vector<reallocResourceMachine>::const_iterator vectorConstIterator_reallocResourceMachine;

typedef std::list<reallocProcess *> set_ReallocProcess;
typedef std::list<reallocProcess *>::iterator setIterator_ReallocProcess;
typedef std::list<reallocProcess *>::const_iterator setConstIterator_ReallocProcess;

class reallocMachine
{
  friend class reallocInstance;
  friend class lpReallocInstance;
  friend class tabuReallocInstance;
public:
  inline reallocID
  get_id() const
  {
    return _id;
  }
  ;
  inline reallocID
  get_location_id() const
  {
    return _location_id;
  }
  ;
  inline reallocID
  get_neighborhood_id() const
  {
    return _neighborhood_id;
  }
  ;
  inline int
  get_under_used_ressources() const
  {
    return _under_used_ressources;
  }
  ;
  inline int64
  get_total_cost() const
  {
    return _total_cost;
  }
  ;
public:
  reallocMachine(int in_resources = 0, int in_machines = 0);
  ~reallocMachine();
  bool
  capacitiesConstraint() const;
  bool
  capacitiesConstraint(const reallocProcess& in_process) const;
  void
  update_capacities(const reallocProcess& in_process, int coeff,
      bool initial_machine);
  void
  load(std::istream &fi, vector_reallocResource& in_resources);
  bool
  capacitiesConstraint(const reallocProcess& in_process, int64 & deltas) const;
  bool
  capacitiesSwapConstraint(const reallocProcess& in_process1,
      const reallocProcess& in_process2, int64 & deltas) const;
  void
  write(std::ostream & fo = std::cout);

  int64
  loadCost();
  int64
  balanceCost(const vector_reallocBalanceCost& in_balance_costs);
  int64
  balanceCost(const reallocBalanceCost & in_balance_cost) const;
  int64
  balanceCost(const reallocBalanceCost & in_balance_cost, const int64 add_r1,
      const int64 add_r2) const;
  void
  set_id();
  vector_reallocResourceMachine _machine_resources;
  set_ReallocProcess _processes_allocation;
  vector_int _moving_cost;

private:
  int64 _load_cost;
  int64 _balance_cost;
  int _under_used_ressources;
  int64 _total_cost;

  static reallocID _id_count;
  reallocID _id;
  reallocID _location_id;
  reallocID _neighborhood_id;
};

typedef std::vector<reallocMachine> vector_reallocMachine;
typedef std::vector<reallocMachine>::iterator vectorIterator_reallocMachine;
typedef std::vector<reallocMachine>::const_iterator vectorConstIterator_reallocMachine;
#endif
