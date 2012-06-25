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
#ifndef REALLOCINSTANCE_H_
#define REALLOCINSTANCE_H_
#include "reallocDefinitions.h"
#include "reallocResource.h"
#include "reallocMachine.h"
#include "reallocService.h"
#include "reallocProcess.h"
#include "reallocBalanceCost.h"

//#define REALLOC_DEBUG

class reallocInstance
{
  friend class matchingReallocGraph;
public:
  reallocInstance();
  ~reallocInstance();
  void
  loadFile(std::istream &fi);
  virtual void
  loadInitialAffectation(std::istream &fi);
  virtual void
  saveSolution(std::ostream &fo = std::cout);
  inline int64
  getCost()
  {
    return _cost;
  }
private:
  void
  _load_resources(std::istream &fi);
  void
  _load_machines(std::istream &fi);
  void
  _load_services(std::istream &fi);
  void
  _load_processes(std::istream &fi);
  void
  _load_balance_costs(std::istream &fi);

protected:
  bool
  _serviceConstraint(const reallocProcess& in_process,
      const reallocMachine& in_machine) const;
  void
  constraintDelta(const reallocProcess& in_process,
      const reallocMachine& in_machine, bool & _move, int64 & _deltas) const;
  int64
  _deltaSwapBalanceCost(const reallocProcess& in_process1,
      const reallocProcess& in_process2) const;

  void
  constraintSwapDelta(const reallocMachine& in_machine,
      const reallocProcess& in_process1, const reallocProcess& in_process2,
      bool & _move, int64 & _deltas) const;
  bool
  constraintSwapDelta(const reallocProcess& in_process1,
      const reallocProcess& in_process2, int64 & _deltas) const;
  int64
  _loadCost();
  int64
  _balanceCost();
  int64
  _deltaServiveCost(const reallocProcess& in_process,
      const reallocMachine& in_machine) const;
  int64
  _deltaReallocationCost(const reallocProcess& in_process,
      const reallocMachine& in_machine) const;

  int64
  _deltaRemoveLoadCost(const reallocProcess& in_process,
      const reallocMachine& in_machine) const;
  int64
  _deltaRemoveBalanceCost(const reallocProcess& in_process,
      const reallocMachine& in_machine) const;
  int64
  _deltaAddBalanceCost(const reallocProcess& in_process,
      const reallocMachine& in_machine) const;
  int64
  _deltaSwapBalanceCost(const reallocProcess& in_process1,
      const reallocProcess& in_process2,
      const reallocMachine& in_machine) const;
  void
  _deltaCost(reallocMachine& in_machine);

  virtual void
  _moveSolution(reallocProcess& in_process, reallocMachine& in_machine,
      const int64 in_deltaCost);
  virtual void
  _moveSwapSolution(reallocProcess& in_process, reallocMachine& in_machine);
  int64 _cost;
  int _max_move_service;
  vector_int _max_service;
  int _nb_resources;
  vector_reallocResource _resources;
  int _nb_machines;
  vector_reallocMachine _machines;
  int _nb_services;
  vector_reallocService _services;
  int _nb_processes;
  vector_reallocProcess _processes;
  int _nb_balance_costs;
  vector_reallocBalanceCost _balance_costs;
  int _nb_locations;
  int _nb_neighbourhoods;
  int _weight_process_move;
  int _weight_service_move;
  int _weight_machine_move;
  vector_reallocID _solution;

};

#endif
