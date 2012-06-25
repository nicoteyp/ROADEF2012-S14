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

#include "tabuReallocInstance.h"
#include "matchingReallocGraph.h"
#include "Random.h"
#include <algorithm>
#ifdef REALLOC_OPENMP
#include <omp.h>
#endif
bool
compareMove(const reallocMove & a, const reallocMove & b)
{
  return a._delta < b._delta;
}

tabuReallocInstance::tabuReallocInstance(reallocTimer & in_timer) :
    reallocInstance(), _current_moves(), _machine_group(), _best_solution(),
    _selection_min(), _selection_max(), _min_cost(0), _initial_cost(0),
    _iteration(0), _best_iteration(0), _time(0), _tabu_length(TABU),
    _deltaNeighborhoodConstraints(), _timer(in_timer),
    _empty_matching_iteration(0), _max_uur(0)
{

  _selection_min.reserve(MAX_MACHINE_SELECTION);
  _selection_max.reserve(MAX_MACHINE_SELECTION);
  _deltaNeighborhoodConstraints.reserve(REALLOC_MACHINES * REALLOC_PROCESSES);
  SRandom(0);
}

bool
tabuReallocInstance::_exit_predicate() const
{
  return _timer.expired(_iteration);
}
bool
tabuReallocInstance::_iterationSearch(double percent)
{
  _drawGraph(percent);
  _applyMoveGraph();
  _iteration++;
  return !(get_max_uur() - get_empty_matching_iteration() <= 0
      && _current_moves.size() == 0);
}

void
tabuReallocInstance::search(std::ostream &fo)
{
  bool temp = true;
  double percent = 0;
  _initial_cost = _cost;
  double max_delta = 0;
  if (_nb_processes <= MAX_PROCESSES)
    _max_delta = DIVERSIFICATION_DELTA;
  else
    _max_delta = INTENSIFICATION_DELTA;

  _selection_size = std::min(_nb_machines / 2, MAX_MACHINE_SELECTION);
  matchingReallocGraph::init_selection(_nb_machines);
  do
    {
      if (!_iterationSearch(percent))
        {
          _empty_matching_iteration = 0;
#ifdef REALLOC_DEBUG
          std::cout << "nb iterations\t" << _iteration << std::endl;
          std::cout << "cost\t" << _min_cost << std::endl;
          std::cout << "time\t" << _time << std::endl;
#endif
          percent += _max_delta;
        }
    }
  while (!_exit_predicate());
  saveSolution(fo);

#ifdef REALLOC_DEBUG
  std::cout << "nb iterations\t" << _iteration << std::endl;
  std::cout << "cost\t" << _min_cost << std::endl;
  std::cout << "time\t" << _time << std::endl;
#endif

}

void
tabuReallocInstance::_updateDeltaNeighborhoodConstraints(
    const reallocProcess & process, const reallocID new_neighbourhood,
    const reallocID old_neighbourhood)

{
  reallocService* service = process.get_service();
#ifdef REALLOC_OPENMP
#pragma omp parallel sections
    {
#pragma omp section
        {
#endif
  for (vectorIterator_reallocProcessPtr iter = service->_processes.begin();
      iter != service->_processes.end(); ++iter)
    {
      if ((*iter)->get_allocation()->get_neighborhood_id() == old_neighbourhood
          || (*iter)->get_allocation()->get_neighborhood_id()
              == new_neighbourhood)
        {
          for (int i = 0; i < _nb_neighbourhoods; ++i)
            set_constraintNeighborhood(i, *(*iter));
        }
    }
#ifdef REALLOC_OPENMP
}

#pragma omp section
{
#endif
  //depend des services
  for (vectorConstIterator_reallocServicePtr iter_ser =
      service->_services_depends.begin();
      iter_ser != service->_services_depends.end(); ++iter_ser)
    {
      for (vectorIterator_reallocProcessPtr iter =
          (*iter_ser)->_processes.begin();
          iter != (*iter_ser)->_processes.end(); ++iter)
        {
          if ((*iter)->get_allocation()->get_neighborhood_id()
              == old_neighbourhood
              || (*iter)->get_allocation()->get_neighborhood_id()
                  == new_neighbourhood)
            {
              for (int i = 0; i < _nb_neighbourhoods; ++i)
                set_constraintNeighborhood(i, *(*iter));
            }
        }
    }
  //services qui dépendent
  for (vectorConstIterator_reallocServicePtr iter_ser =
      service->_services_superiors.begin();
      iter_ser != service->_services_superiors.end(); ++iter_ser)
    {
      for (vectorIterator_reallocProcessPtr iter =
          (*iter_ser)->_processes.begin();
          iter != (*iter_ser)->_processes.end(); ++iter)
        {
          set_constraintNeighborhood(new_neighbourhood, *(*iter));
          set_constraintNeighborhood(old_neighbourhood, *(*iter));
        }
    }
#ifdef REALLOC_OPENMP
}
}
#endif
}

bool
tabuReallocInstance::get_constraintNeighborhood(
    const reallocID in_neighbourhood, const reallocID in_process) const
{
  return _deltaNeighborhoodConstraints[in_neighbourhood
      + _nb_neighbourhoods * in_process];
}

void
tabuReallocInstance::set_constraintNeighborhood(
    const reallocID in_neighbourhood, const reallocProcess & in_process)
{
#ifdef REALLOC_OPENMP
  bool temp = in_process.get_service()->neighbourhoodsConstraint(in_process,
      in_neighbourhood);

#pragma omp critical
    {
      _deltaNeighborhoodConstraints[in_neighbourhood + _nb_neighbourhoods
      * in_process.get_id()] = temp;
    }
#else
  _deltaNeighborhoodConstraints[in_neighbourhood
      + _nb_neighbourhoods * in_process.get_id()] =
      in_process.get_service()->neighbourhoodsConstraint(in_process,
          in_neighbourhood);
#endif
}

void
tabuReallocInstance::constraintForNeighborhood(const reallocID in_neighbourhood)
{
  for (vectorConstIterator_reallocProcess iter_process = _processes.begin();
      iter_process != _processes.end(); ++iter_process)
    {
      set_constraintNeighborhood(in_neighbourhood, (*iter_process));
    }

}

void
tabuReallocInstance::loadInitialAffectation(std::istream &fi)
{
  reallocInstance::loadInitialAffectation(fi);
  _best_solution = _solution;
  _min_cost = _cost;
  _deltaNeighborhoodConstraints.resize(_nb_neighbourhoods * _nb_processes,
      true);
#ifdef REALLOC_OPENMP
#pragma omp parallel for
  for (int i = 0; i < _nb_neighbourhoods; ++i)
  this->constraintForNeighborhood(i);

  for (vectorIterator_reallocProcess iter_process = _processes.begin(); iter_process
      != _processes.end(); ++iter_process)
    {
      (*iter_process).get_allocation()->_total_cost += (*iter_process).get_load_cost();
    }

#else
  for (vectorIterator_reallocProcess iter_process = _processes.begin();
      iter_process != _processes.end(); ++iter_process)
    {
      for (int i = 0; i < _nb_neighbourhoods; ++i)
        {
          set_constraintNeighborhood(i, (*iter_process));
        }
      (*iter_process).get_allocation()->_total_cost +=
          (*iter_process).get_load_cost();
    }
#endif

#ifdef REALLOC_OPENMP
#pragma omp parallel for
  for (int i = 0; i < _nb_machines; ++i)
  _deltaCost(_machines[i]);
#pragma omp taskwait
#else
  for (vectorIterator_reallocMachine iter_machine = _machines.begin();
      iter_machine != _machines.end(); ++iter_machine)
    _deltaCost(*iter_machine);
#endif

  _current_moves.reserve(_nb_machines);
  _machine_group.resize(_nb_resources + 1);
  for (vectorIterator_reallocMachine iter_machine = _machines.begin();
      iter_machine != _machines.end(); ++iter_machine)
    {
      _machine_group[(*iter_machine).get_under_used_ressources()].push_back(
          &(*iter_machine));
      (*iter_machine)._processes_allocation.sort(compareProcess);
    }
}

int64
tabuReallocInstance::deltaLoadCost(const reallocProcess& in_process,
    const reallocMachine& in_machine) const
{
  int64 delta = 0;
  bool temp;
  constraintDelta(in_process, in_machine, temp, delta);
  if (!temp)
    {
      return LONG_MAX;
    }
  delta += in_process.get_delta_cost();
  return delta;

}

int64
tabuReallocInstance::deltaSwapLoadCost(const reallocProcess& in_process1,
    const reallocProcess& in_process2) const
{
  int64 delta = 0;
  bool temp = constraintSwapDelta(in_process1, in_process2, delta);
  if (!temp)
    {
      return LONG_MAX;
    }
  return delta;
}

void
tabuReallocInstance::saveSolution(std::ostream &fo)
{
  fo.clear();
  for (vectorIterator_reallocID iter = _best_solution.begin();
      iter != _best_solution.end(); ++iter)
    fo << *iter << " ";
}

void
tabuReallocInstance::_drawGraph(double percent)
{
  matchingReallocGraph G(this->_nb_machines);
  _overusedMachinesSelection(_selection_min, _selection_size);
  _underusedMachinesSelection(_selection_max, _selection_size);
  _max_uur = _selection_max.front()->_under_used_ressources;
  G.defineAddSwapEdgesOptimised(*this, percent);
  G.matching(*this);
}

void
tabuReallocInstance::bestSolution()
{
  if (_min_cost > _cost)
    {
      _best_solution = _solution;
      _min_cost = _cost;
#ifdef REALLOC_DEBUG
      _time = _timer.get_passed_time_sec();
#endif
    }
}

void
tabuReallocInstance::_applyMoveGraph()
{
  sort(_current_moves.begin(), _current_moves.end(), compareMove);
  std::vector<reallocMove>::iterator iter = _current_moves.begin();
  int64 delta_cost = 0;

  for (;
      iter != _current_moves.end()
          && delta_cost + (*iter)._delta < _max_delta * this->_initial_cost;
      ++iter)
    {
      if ((*iter)._machine)
        {
          reallocMachine * old = (*iter)._process->get_allocation();
          if (_serviceConstraint(*((*iter)._process), *((*iter)._machine))
              && get_constraintNeighborhood(
                  (*iter)._machine->get_neighborhood_id(),
                  (*iter)._process->get_id()))
            {
              int64 delta = (*iter)._delta
                  + this->_deltaServiveCost(*(*iter)._process,
                      *(*iter)._machine);

              delta_cost += delta;
              _machine_group[old->get_under_used_ressources()].remove(old);
              _machine_group[(*iter)._machine->get_under_used_ressources()].remove(
                  (*iter)._machine);

              _moveSolution(*(*iter)._process, *(*iter)._machine, delta);
              bestSolution();
              (*iter)._process->set_iteration(_iteration);
#ifdef REALLOC_OPENMP
#pragma omp parallel sections
                {

#pragma omp section
                    {
#endif
              (*iter)._machine->_processes_allocation.push_back(
                  (*iter)._process);
              (*iter)._machine->_processes_allocation.sort(compareProcess);
              (*iter)._machine->_total_cost +=
                  (*iter)._process->get_load_cost();
              this->_deltaCost(*(*iter)._machine);
#ifdef REALLOC_OPENMP
            }
#pragma omp section
            {
#endif
              old->_processes_allocation.remove((*iter)._process);
              old->_processes_allocation.sort(compareProcess);
              old->_total_cost -= (*iter)._process->get_load_cost();
              this->_deltaCost(*old);
#ifdef REALLOC_OPENMP
            }
        }
#endif
              _updateDeltaNeighborhoodConstraints(*(*iter)._process,
                  (*iter)._machine->get_neighborhood_id(),
                  old->get_neighborhood_id());
              _machine_group[old->get_under_used_ressources()].push_back(old);
              _machine_group[(*iter)._machine->get_under_used_ressources()].push_back(
                  (*iter)._machine);
#ifdef REALLOC_OPENMP
#pragma omp taskwait
#endif
            }
        }
      else
        {
          //swap move
          reallocMachine * old1 = (*iter)._process->get_allocation();
          reallocMachine * old2 = (*iter)._process_2->get_allocation();

          if (_serviceConstraint(*((*iter)._process), *old2)
              && get_constraintNeighborhood(old2->get_neighborhood_id(),
                  (*iter)._process->get_id())

              && _serviceConstraint(*((*iter)._process_2), *old1)
              && get_constraintNeighborhood(old1->get_neighborhood_id(),
                  (*iter)._process_2->get_id())

                  )
            {
              _machine_group[old1->get_under_used_ressources()].remove(old1);
              _machine_group[old2->get_under_used_ressources()].remove(old2);

              int64 delta1 = _deltaServiveCost(*(*iter)._process, *old2);
              _moveSwapSolution(*(*iter)._process, *old2);
              int64 delta2 = _deltaServiveCost(*(*iter)._process_2, *old1);
              _moveSwapSolution(*(*iter)._process_2, *old1);
              delta_cost += (*iter)._delta + delta1 + delta2;

              _cost += (*iter)._delta + delta1 + delta2;
              bestSolution();
#ifdef REALLOC_OPENMP
#pragma omp parallel sections
                {
#pragma omp section
                    {
#endif
              (*iter)._process_2->set_iteration(_iteration);
              old1->_processes_allocation.remove((*iter)._process);
              old1->_processes_allocation.push_back((*iter)._process_2);
              old1->_total_cost += (*iter)._process_2->get_load_cost();
              old1->_total_cost -= (*iter)._process->get_load_cost();
              old1->_processes_allocation.sort(compareProcess);
              this->_deltaCost(*old1);
#ifdef REALLOC_OPENMP
            }
#pragma omp section
            {
#endif
              (*iter)._process->set_iteration(_iteration);
              old2->_processes_allocation.remove((*iter)._process_2);
              old2->_processes_allocation.push_back((*iter)._process);
              old2->_total_cost -= (*iter)._process_2->get_load_cost();
              old2->_total_cost += (*iter)._process->get_load_cost();
              old2->_processes_allocation.sort(compareProcess);
              this->_deltaCost(*old2);
#ifdef REALLOC_OPENMP
            }
        }
#endif
              _updateDeltaNeighborhoodConstraints(*(*iter)._process,
                  old2->get_neighborhood_id(), old1->get_neighborhood_id());

              _updateDeltaNeighborhoodConstraints(*(*iter)._process_2,
                  old1->get_neighborhood_id(), old2->get_neighborhood_id());
#ifdef REALLOC_OPENMP
#pragma omp taskwait
#endif
              _machine_group[old1->get_under_used_ressources()].push_back(old1);
              _machine_group[old2->get_under_used_ressources()].push_back(old2);

            }
        }
    }
  if (_current_moves.size() == 0)
    {
      _empty_matching_iteration++;
    }
}

void
randomizedUnderusedMachinesSelection(std::vector<reallocMachine*> & in_base,
    unsigned int in_nb, std::vector<reallocMachine*> & in_selection)
{
  for (unsigned int i = 0; i < in_nb; ++i)
    {
      int elt = RandomInt(in_base.size());
      in_selection.push_back(in_base[elt]);
      in_base.erase(in_base.begin() + elt);

    }
}

void
randomizedOverusedMachinesSelection(std::vector<reallocMachine*> & in_base,
    unsigned int in_nb, std::vector<reallocMachine*> & in_selection)
{
  for (unsigned int i = 0; i < in_nb; ++i)
    {
      int elt = RandomInt(in_base.size());
      if (in_base[elt]->_processes_allocation.size() > 0)
        in_selection.push_back(in_base[elt]);
      else
        --i;
      in_base.erase(in_base.begin() + elt);
      if (in_base.size() == 0)
        return;
    }
}

void
tabuReallocInstance::_underusedMachinesSelection(
    std::vector<reallocMachine*> & in_selection, unsigned int _nb)
{
  in_selection.clear();
  for (unsigned int i = 0; i < _machine_group.size(); ++i)
    {
      unsigned index = _machine_group.size() - i - 1;
      if (_nb - in_selection.size() > _machine_group[index].size())
        {
          in_selection.insert(in_selection.end(), _machine_group[index].begin(),
              _machine_group[index].end());
        }
      else
        {
          std::vector<reallocMachine*> selection;
          selection.insert(selection.end(), _machine_group[index].begin(),
              _machine_group[index].end());
          randomizedUnderusedMachinesSelection(selection,
              _nb - in_selection.size(), in_selection);
        }
      if (in_selection.size() == _nb)
        return;
    }
}
void
tabuReallocInstance::_overusedMachinesSelection(
    std::vector<reallocMachine*> & in_selection, unsigned int _nb)
{
  in_selection.clear();
  for (unsigned int i = 0; i < _machine_group.size(); ++i)
    {
      if (_nb - in_selection.size() > _machine_group[i].size())
        {
          in_selection.insert(in_selection.end(), _machine_group[i].begin(),
              _machine_group[i].end());
        }
      else
        {
          std::vector<reallocMachine*> selection;
          selection.insert(selection.end(), _machine_group[i].begin(),
              _machine_group[i].end());
          randomizedOverusedMachinesSelection(selection,
              _nb - in_selection.size(), in_selection);
        }

      if (in_selection.size() == _nb)
        return;

    }
}
