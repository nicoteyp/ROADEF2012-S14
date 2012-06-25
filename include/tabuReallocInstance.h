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

#ifndef TABUREALLOCINSTANCE_H_
#define TABUREALLOCINSTANCE_H_
#include "reallocInstance.h"
#include "reallocTimer.h"
#include <climits>
#include <cmath>
#include <cstdlib>

struct reallocMove
{
  reallocMove(int64 _cost = LONG_MAX) :
      _delta(_cost), _process(NULL), _process_2(NULL), _machine(NULL)
  {
  }
  ;
  int64 _delta;
  reallocProcess * _process;
  reallocProcess * _process_2;
  reallocMachine * _machine;
};

typedef std::vector<reallocMachine *> set_ReallocMachine;
typedef std::vector<reallocMachine *>::iterator setIterator_ReallocMachine;

typedef std::vector<set_ReallocMachine> vector_setReallocMachine;
typedef std::vector<set_ReallocMachine>::iterator vectorIterator_setReallocMachine;

typedef std::vector<set_ReallocProcess> vector_setReallocProcess;
typedef std::vector<set_ReallocProcess>::iterator vectorIterator_setReallocProcess;

#define TABU 10
#define MAX_MACHINE_SELECTION 100
#define MAX_PROCESS_SELECTION 20
#define DIVERSIFICATION_DELTA 0.001
#define INTENSIFICATION_DELTA 0.00001
#define MAX_PROCESSES 1000
//#define REALLOC_TEST

class tabuReallocInstance : public reallocInstance
{
  friend class matchingReallocGraph;
public:
  tabuReallocInstance(reallocTimer & in_timer);
  virtual void
  loadInitialAffectation(std::istream &fi);
  virtual void
  saveSolution(std::ostream &fo = std::cout);
  virtual void
  search(std::ostream &fo = std::cout);
  inline int
  get_tabu_length() const
  {
    return _tabu_length;
  }
  ;
  inline int64
  get_iteration() const
  {
    return _iteration;
  }
  ;
  inline int64
  get_min_cost() const
  {
    return _min_cost;
  }
  ;
  inline int64
  get_initial_cost() const
  {
    return _initial_cost;
  }
  ;
  inline int64
  get_best_iteration() const
  {
    return _best_iteration;
  }
  ;
  inline int
  get_max_uur() const
  {
    return _max_uur;
  }
  ;
  inline int
  get_empty_matching_iteration() const
  {
    return _empty_matching_iteration;
  }
  ;
protected:
  void
  _overusedMachinesSelection(std::vector<reallocMachine*> & in_selection,
      unsigned int _nb);
  void
  _underusedMachinesSelection(std::vector<reallocMachine*> & in_selection,
      unsigned int _nb);
  bool
  _iterationSearch(double percent);
  void
  _drawGraph(double percent = 0.0);
  void
  _applyMoveGraph();

  bool
  _exit_predicate() const;
  int64
  deltaLoadCost(const reallocProcess& in_process,
      const reallocMachine& in_machine) const;
  int64
  deltaSwapLoadCost(const reallocProcess& in_process1,
      const reallocProcess& in_process2) const;

  void
  _updateDeltaNeighborhoodConstraints(const reallocProcess & process,
      const reallocID new_neighbourhood, const reallocID old_neighbourhood);
  void
  bestSolution();
  void
  constraintForNeighborhood(const reallocID in_neighbourhood);
  bool
  get_constraintNeighborhood(const reallocID in_neighbourhood,
      const reallocID in_process) const;
  void
  set_constraintNeighborhood(const reallocID in_neighbourhood,
      const reallocProcess & in_process);
  std::vector<reallocMove> _current_moves;
  std::vector<std::list<reallocMachine*> > _machine_group;
  vector_reallocID _best_solution;
  std::vector<reallocMachine*> _selection_min;
  std::vector<reallocMachine*> _selection_max;
private:
  int64 _min_cost;
  int64 _initial_cost;
  int64 _iteration;
  int64 _best_iteration;
  double _time;
  int _tabu_length;
  vector_bool _deltaNeighborhoodConstraints;
  reallocTimer _timer;
  int _empty_matching_iteration;
  int _max_uur;
  int _selection_size;
  double _max_delta;

};

#endif
