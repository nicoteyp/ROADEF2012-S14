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

#include "matchingReallocGraph.h"
#ifdef REALLOC_OPENMP
#include <omp.h>
#endif

#include "Random.h"
void
randomizedProcessesSelection(const set_ReallocProcess & in_base,
    unsigned int in_nb, std::vector<reallocProcess*> & in_selection,
    const tabuReallocInstance& instance)
{
  if (in_nb > in_base.size())
    {
      in_selection.insert(in_selection.end(), in_base.begin(), in_base.end());
    }
  else
    {
      std::vector<reallocProcess*> temp;
      temp.insert(temp.begin(), in_base.begin(), in_base.end());

      for (unsigned int i = 0; i < in_nb; ++i)
        {
          int elt = RandomInt(temp.size());

          if (temp[elt]->get_iteration() + instance.get_tabu_length()
              < instance.get_iteration())
            in_selection.push_back(temp[elt]);
          else
            --i;
          temp.erase(temp.begin() + elt);
          if (temp.size() == 0)
            return;
        }
    }
}

void
weightedRandomizedProcessesSelection(const set_ReallocProcess & in_base,
    unsigned int in_nb, std::vector<reallocProcess*> & in_selection,
    const tabuReallocInstance& instance)
{
  if (in_nb > in_base.size())
    {
      in_selection.insert(in_selection.end(), in_base.begin(), in_base.end());
    }
  else
    {
      std::vector<reallocProcess*> temp;
      temp.insert(temp.begin(), in_base.begin(), in_base.end());
      int64 weight = temp.front()->get_allocation()->get_total_cost();

      for (unsigned int i = 0; i < in_nb; ++i)
        {
          if (temp.size() == 0 || weight == 0)
            break;
          int64 r = RandomInt(weight);
          int64 temp2 = 0;
          unsigned int elt = 0;
          while (r > temp2 + temp[elt]->get_load_cost() && elt < temp.size())
            {
              temp2 += temp[elt]->get_load_cost();
              elt++;
            }
          weight -= temp[elt]->get_load_cost();
          if (temp[elt]->get_iteration() + instance.get_tabu_length()
              < instance.get_iteration())
            in_selection.push_back(temp[elt]);
          else
            --i;
          temp.erase(temp.begin() + elt);

        }

      sort(in_selection.begin(), in_selection.end(), compareProcess);

    }
}
std::vector<reallocProcess*> matchingReallocGraph::_selection_swap(0);
std::vector<std::vector<reallocProcess*> > matchingReallocGraph::_selections_swap(
    0);
vector_bool matchingReallocGraph::_constraint(0);
#ifdef REALLOC_OPENMP
std::vector<std::vector<reallocProcess*> >
matchingReallocGraph::_selections_swap2(0);
#endif
void
matchingReallocGraph::init_selection(int nb_machines)
{
  _selections_swap.resize(nb_machines);
  for (int i = 0; i < nb_machines; ++i)
    {
      _selections_swap[i].reserve(MAX_PROCESS_SELECTION);
    }
  _constraint.resize(MAX_PROCESS_SELECTION, false);
  _selection_swap.reserve(MAX_PROCESS_SELECTION);
#ifdef REALLOC_OPENMP
  _selections_swap2.resize(nb_machines);
#endif
}

matchingReallocGraph::matchingReallocGraph(int nb_machines) :
    G(), edges(nb_machines * nb_machines, -1), weights(G),
    edge_move(G, reallocMove(LONG_MAX))
{
  G.reserveNode(nb_machines);
  G.reserveEdge(nb_machines * nb_machines);
  for (int i = 0; i < nb_machines; ++i)
    {
      G.addNode();
    }
}

matchingReallocGraph::~matchingReallocGraph()
{
  edges.clear();
}

void
matchingReallocGraph::defineAddEdges(const tabuReallocInstance& instance,
    double percent)
{
  std::vector<reallocMachine*>::const_iterator iter_machine1 =
      instance._selection_min.begin();
  for (; iter_machine1 != instance._selection_min.end(); ++iter_machine1)
    {
      _selections_swap[(*iter_machine1)->get_id()].clear();
      _selections_swap[(*iter_machine1)->get_id()].reserve(
          MAX_PROCESS_SELECTION);
      weightedRandomizedProcessesSelection(
          (*iter_machine1)->_processes_allocation, MAX_PROCESS_SELECTION,
          _selections_swap[(*iter_machine1)->get_id()], instance);
    }

  std::vector<reallocMachine*>::const_iterator iter_machine =
      instance._selection_max.begin();
#ifdef REALLOC_OPENMP

  std::vector<reallocMachine*>::const_iterator iter_machine2;
#pragma omp parallel for private (iter_machine2)
  for (unsigned int i = 0; i < instance._selection_max.size(); ++i)
    {
      if (instance._selection_max[i]->get_under_used_ressources()
          < instance.get_max_uur() - instance.get_empty_matching_iteration())
      continue;
      for (iter_machine2 = instance._selection_min.begin(); iter_machine2
          != instance._selection_min.end(); ++iter_machine2)
        {
          if (instance._selection_max[i]->get_id() == (*iter_machine2)->get_id())
          continue;
          defineAddEdge(instance, percent, *(instance._selection_max[i]),
              *(*iter_machine2));

        }
    }

#else
  for (;
      iter_machine != instance._selection_max.end()
          && (*iter_machine)->get_under_used_ressources()
              >= instance.get_max_uur()
                  - instance.get_empty_matching_iteration(); ++iter_machine)
    {
      for (std::vector<reallocMachine*>::const_iterator iter_machine2 =
          instance._selection_min.begin();
          iter_machine2 != instance._selection_min.end(); ++iter_machine2)
        {
          if ((*iter_machine)->get_id() == (*iter_machine2)->get_id())
            continue;
          defineAddEdge(instance, percent, *(*iter_machine), *(*iter_machine2));
        }
    }
#endif
}
void
matchingReallocGraph::defineAddEdgeValue(const reallocGraph::Edge &in_edge,
    const int64& in_cost, const int64& in_delta, reallocProcess * in_process,
    reallocMachine * in_machine)
{
#ifdef REALLOC_OPENMP
#pragma omp critical
#endif
    {
      weights[in_edge] = in_cost;
      edge_move[in_edge]._process = in_process;
      edge_move[in_edge]._machine = in_machine;
      edge_move[in_edge]._delta = in_delta;
      edge_move[in_edge]._process_2 = NULL;

    }
}

void
matchingReallocGraph::defineSwapEdgeValue(const reallocGraph::Edge &in_edge,
    const int64& in_cost, const int64& in_delta, reallocProcess * in_process,
    reallocProcess * in_process_2)
{
#ifdef REALLOC_OPENMP
#pragma omp critical
#endif
    {
      weights[in_edge] = in_cost;
      edge_move[in_edge]._process = in_process;
      edge_move[in_edge]._machine = NULL;
      edge_move[in_edge]._delta = in_delta;
      edge_move[in_edge]._process_2 = in_process_2;

    }
}

void
matchingReallocGraph::defineAddEdge(const tabuReallocInstance& instance,
    double percent, reallocMachine& machine1, const reallocMachine& machine2)
{
  int i, j;
  reallocGraph::Edge edge;
  if (machine1.get_id() < machine2.get_id())
    {
      i = machine1.get_id();
      j = machine2.get_id();
    }
  else
    {
      j = machine1.get_id();
      i = machine2.get_id();
    }
  int64 deltaEdge = percent * instance.get_initial_cost();
  for (std::vector<reallocProcess*>::iterator iter_process =
      _selections_swap[machine2.get_id()].begin();
      iter_process != _selections_swap[machine2.get_id()].end()
          && -(*iter_process)->get_load_cost() < deltaEdge; ++iter_process)
    {
      if ((*iter_process)->get_iteration() + instance.get_tabu_length()
          >= instance.get_iteration())
        continue;

      if (instance.get_constraintNeighborhood(machine1.get_neighborhood_id(),
          (*iter_process)->get_id())
          && (*iter_process)->get_service()->conflictConstraint(
              machine1.get_id())
          && instance._serviceConstraint(*(*iter_process), machine1))
        {
          int64 delta = instance.deltaLoadCost(*(*iter_process), machine1);
          if (delta >= percent * instance.get_initial_cost())
            continue;
          delta += instance._deltaReallocationCost(*(*iter_process), machine1);
          if (edges[i * instance._nb_machines + j] == -1)
            {
#ifdef REALLOC_OPENMP
#pragma omp critical
#endif
                {
                  edge = G.addEdge(G.nodeFromId(i), G.nodeFromId(j));
                  edges[i * instance._nb_machines + j] = G.id(edge);
                }
              defineAddEdgeValue(edge,
                  (-1 * (delta - percent * instance.get_initial_cost())), delta,
                  (*iter_process), &machine1);
              deltaEdge = delta;

            }
          else
            {
              edge = G.edgeFromId(edges[i * instance._nb_machines + j]);
              if (-1 * (delta - percent * instance.get_initial_cost())
                  > weights[edge])
                {
                  defineAddEdgeValue(edge,
                      (-1 * (delta - percent * instance.get_initial_cost())),
                      delta, (*iter_process), &machine1);
                  deltaEdge = delta;
                }

            }
        }
    }
}

void
matchingReallocGraph::defineAddSwapEdgesOptimised(
    const tabuReallocInstance& instance, double percent)
{

  std::vector<reallocMachine*>::const_iterator iter_machine =
      instance._selection_min.begin();
  for (; iter_machine != instance._selection_min.end(); ++iter_machine)
    {
      _selections_swap[(*iter_machine)->get_id()].clear();
      weightedRandomizedProcessesSelection(
          (*iter_machine)->_processes_allocation, MAX_PROCESS_SELECTION,
          _selections_swap[(*iter_machine)->get_id()], instance);
    }
#ifdef REALLOC_OPENMP
  iter_machine = instance._selection_max.begin();
  for (; iter_machine != instance._selection_max.end()
      && (*iter_machine)->get_under_used_ressources() >= instance.get_max_uur() - instance.get_empty_matching_iteration()
      ; ++iter_machine)
    {
      _selections_swap2[(*iter_machine)->get_id()].clear();
      _selections_swap2[(*iter_machine)->get_id()].reserve(MAX_ADD_PROCESS);
      randomizedProcessesSelection((*iter_machine)->_processes_allocation,
          MAX_PROCESS_SELECTION, _selections_swap2[(*iter_machine)->get_id()], instance);
    }
#pragma omp parallel sections
    {
#pragma omp section
        {
          std::vector<reallocMachine*>::const_iterator iter_machine2;
          for (unsigned int i = 0; i < instance._selection_min.size()/2; ++i)
            {
              for (iter_machine2 = instance._selection_max.begin(); iter_machine2
                  != instance._selection_max.end()
                  && (*iter_machine2)->get_under_used_ressources() >= instance.get_max_uur() - instance.get_empty_matching_iteration();
                  ++iter_machine2)
                {
                  if (instance._selection_min[i]->get_id() == (*iter_machine2)->get_id()
                      || instance._selection_min[i]->get_under_used_ressources()
                      > (*iter_machine2)->get_under_used_ressources())
                  continue;
                  defineAddSwapEdgeOptimised(instance, percent,
                      *(*iter_machine2), *(instance._selection_min[i]));

                }
            }
        }
#pragma omp section
        {
          std::vector<reallocMachine*>::const_iterator iter_machine2;
          for (unsigned int i = instance._selection_min.size()/2+1;
              i < instance._selection_min.size(); ++i)
            {
              for (iter_machine2 = instance._selection_max.begin(); iter_machine2
                  != instance._selection_max.end()
                  && (*iter_machine2)->get_under_used_ressources() >= instance.get_max_uur() - instance.get_empty_matching_iteration(); ++iter_machine2)
                {
                  if (instance._selection_min[i]->get_id() == (*iter_machine2)->get_id()
                      || instance._selection_min[i]->get_under_used_ressources()
                      > (*iter_machine2)->get_under_used_ressources())
                  continue;
                  defineAddSwapEdgeOptimised(instance, percent,
                      *(*iter_machine2), *(instance._selection_min[i]));

                }
            }
        }
    }

#else
  if (percent != 0)
    {
      for (iter_machine = instance._selection_max.begin();
          iter_machine != instance._selection_max.end()
              && (*iter_machine)->get_under_used_ressources()
                  >= instance.get_max_uur()
                      - instance.get_empty_matching_iteration(); ++iter_machine)
        {
          _selection_swap.clear();
          randomizedProcessesSelection((*iter_machine)->_processes_allocation,
              MAX_PROCESS_SELECTION, _selection_swap, instance);

          for (std::vector<reallocMachine*>::const_iterator iter_machine2 =
              instance._selection_min.begin();
              iter_machine2 != instance._selection_min.end(); ++iter_machine2)
            {
              if ((*iter_machine)->get_id() == (*iter_machine2)->get_id()
                  || (*iter_machine2)->get_under_used_ressources()
                      > (*iter_machine)->get_under_used_ressources())
                continue;
              defineAddSwapEdgeStage2(instance, percent, *(*iter_machine),
                  *(*iter_machine2));
            }
        }
    }
  else
    {
      for (iter_machine = instance._selection_max.begin();
          iter_machine != instance._selection_max.end()
              && (*iter_machine)->get_under_used_ressources()
                  >= instance.get_max_uur()
                      - instance.get_empty_matching_iteration(); ++iter_machine)
        {
          for (std::vector<reallocMachine*>::const_iterator iter_machine2 =
              instance._selection_min.begin();
              iter_machine2 != instance._selection_min.end(); ++iter_machine2)
            {
              if ((*iter_machine)->get_id() == (*iter_machine2)->get_id()
                  || (*iter_machine2)->get_under_used_ressources()
                      > (*iter_machine)->get_under_used_ressources())
                continue;
              defineAddSwapEdgeStage1(instance, percent, *(*iter_machine),
                  *(*iter_machine2));
            }
        }

    }

#endif

}

void
matchingReallocGraph::defineAddSwapEdgeStage2(
    const tabuReallocInstance& instance, double percent,
    reallocMachine& machine1, const reallocMachine& machine2)
{
  int i, j;
  reallocGraph::Edge edge;
  if (machine1.get_id() < machine2.get_id())
    {
      i = machine1.get_id();
      j = machine2.get_id();
    }
  else
    {
      j = machine1.get_id();
      i = machine2.get_id();
    }
  _constraint.resize(_selections_swap[machine2.get_id()].size(), false);
  int count = 0;
  for (std::vector<reallocProcess*>::iterator iter_process =
      _selections_swap[machine2.get_id()].begin();
      iter_process != _selections_swap[machine2.get_id()].end();
      ++iter_process, ++count)
    {
      _constraint[count] = ((*iter_process)->get_iteration()
          + instance.get_tabu_length() < instance.get_iteration())
          && (instance.get_constraintNeighborhood(
              machine1.get_neighborhood_id(), (*iter_process)->get_id())
              && (*iter_process)->get_service()->conflictConstraint(
                  machine1.get_id())
              && instance._serviceConstraint(*(*iter_process), machine1));
    }

  int64 deltaEdge = percent * instance.get_initial_cost();
  count = 0;
  for (std::vector<reallocProcess*>::iterator iter_process =
      _selections_swap[machine2.get_id()].begin();
      iter_process != _selections_swap[machine2.get_id()].end()
          && -(*iter_process)->get_load_cost() < deltaEdge;
      ++iter_process, ++count)
    {
      if (_constraint[count])
        {
          int64 delta = instance.deltaLoadCost(*(*iter_process), machine1);

          if (delta >= deltaEdge)
            continue;
          delta += instance._deltaReallocationCost(*(*iter_process), machine1);
          if (edges[i * instance._nb_machines + j] == -1)
            {
#ifdef REALLOC_OPENMP
#pragma omp critical
#endif
                {
                  edge = G.addEdge(G.nodeFromId(i), G.nodeFromId(j));
                  edges[i * instance._nb_machines + j] = G.id(edge);

                }
              defineAddEdgeValue(edge,
                  (-1 * (delta - percent * instance.get_initial_cost())), delta,
                  (*iter_process), &machine1);
              deltaEdge = delta;

            }
          else
            {
              edge = G.edgeFromId(edges[i * instance._nb_machines + j]);
              if (-1 * (delta - percent * instance.get_initial_cost())
                  > weights[edge])
                {
                  defineAddEdgeValue(edge,
                      (-1 * (delta - percent * instance.get_initial_cost())),
                      delta, (*iter_process), &machine1);
                  deltaEdge = delta;
                }
            }
        }
    }

  count = 0;
  for (std::vector<reallocProcess*>::iterator iter_process =
      _selections_swap[machine2.get_id()].begin();
      iter_process != _selections_swap[machine2.get_id()].end()
          && -(*iter_process)->get_load_cost() < deltaEdge;
      ++iter_process, ++count)
    {
      if (_constraint[count])
        {
#ifdef REALLOC_OPENMP
          std::vector<reallocProcess*>::iterator iter_process2 =
          _selections_swap2[machine1.get_id()].begin();
          for (; iter_process2 != _selections_swap2[machine1.get_id()].end(); ++iter_process2)
#else
          std::vector<reallocProcess*>::iterator iter_process2 =
              _selection_swap.begin();
          for (; iter_process2 != _selection_swap.end(); ++iter_process2)
#endif
            {
              if ((*iter_process2)->get_iteration() + instance.get_tabu_length()
                  >= instance.get_iteration())
                continue;
              if (instance.get_constraintNeighborhood(
                  machine2.get_neighborhood_id(), (*iter_process2)->get_id())
                  && (*iter_process2)->get_service()->conflictConstraint(
                      machine2.get_id())
                  && instance._serviceConstraint(*(*iter_process2), machine2))
                {
                  int64 delta = instance.deltaSwapLoadCost(*(*iter_process),
                      *(*iter_process2));
                  if (delta >= percent * instance.get_initial_cost())
                    continue;
                  delta += instance._deltaReallocationCost(*(*iter_process),
                      machine1);
                  delta += instance._deltaReallocationCost(*(*iter_process2),
                      machine2);
                  if (edges[i * instance._nb_machines + j] == -1)
                    {
#ifdef REALLOC_OPENMP
#pragma omp critical
#endif
                        {
                          edge = G.addEdge(G.nodeFromId(i), G.nodeFromId(j));
                          edges[i * instance._nb_machines + j] = G.id(edge);
                        }
                      defineSwapEdgeValue(edge,
                          -1 * (delta - percent * instance.get_initial_cost()),
                          delta, (*iter_process), (*iter_process2));
                      deltaEdge = delta;
                    }
                  else
                    {
                      edge = G.edgeFromId(edges[i * instance._nb_machines + j]);
                      if (-1 * (delta - percent * instance.get_initial_cost())
                          > weights[edge])
                        {
                          defineSwapEdgeValue(edge,
                              -1
                                  * (delta
                                      - percent * instance.get_initial_cost()),
                              delta, (*iter_process), (*iter_process2));
                          deltaEdge = delta;
                        }
                    }
                }
            }

        }
    }
}

void
matchingReallocGraph::defineAddSwapEdgeStage1(
    const tabuReallocInstance& instance, double percent,
    reallocMachine& machine1, const reallocMachine& machine2)
{
  int i, j;
  reallocGraph::Edge edge;
  if (machine1.get_id() < machine2.get_id())
    {
      i = machine1.get_id();
      j = machine2.get_id();
    }
  else
    {
      j = machine1.get_id();
      i = machine2.get_id();
    }
  int64 deltaEdge = percent * instance.get_initial_cost();
  for (std::vector<reallocProcess*>::iterator iter_process =
      _selections_swap[machine2.get_id()].begin();
      iter_process != _selections_swap[machine2.get_id()].end()
          && -(*iter_process)->get_load_cost() < deltaEdge; ++iter_process)
    {
      if (((*iter_process)->get_iteration() + instance.get_tabu_length()
          < instance.get_iteration())
          && (instance.get_constraintNeighborhood(
              machine1.get_neighborhood_id(), (*iter_process)->get_id())
              && (*iter_process)->get_service()->conflictConstraint(
                  machine1.get_id())
              && instance._serviceConstraint(*(*iter_process), machine1)))
        {
          int64 delta = instance.deltaLoadCost(*(*iter_process), machine1);

          if (delta >= deltaEdge)
            continue;
          delta += instance._deltaReallocationCost(*(*iter_process), machine1);
          if (edges[i * instance._nb_machines + j] == -1)
            {
#ifdef REALLOC_OPENMP
#pragma omp critical
#endif
                {
                  edge = G.addEdge(G.nodeFromId(i), G.nodeFromId(j));
                  edges[i * instance._nb_machines + j] = G.id(edge);

                }
              defineAddEdgeValue(edge,
                  (-1 * (delta - percent * instance.get_initial_cost())), delta,
                  (*iter_process), &machine1);
              deltaEdge = delta;

            }
          else
            {
              edge = G.edgeFromId(edges[i * instance._nb_machines + j]);
              if (-1 * (delta - percent * instance.get_initial_cost())
                  > weights[edge])
                {
                  defineAddEdgeValue(edge,
                      (-1 * (delta - percent * instance.get_initial_cost())),
                      delta, (*iter_process), &machine1);
                  deltaEdge = delta;
                }
            }
        }
    }
}

void
matchingReallocGraph::defineSwapEdge(const tabuReallocInstance& instance,
    double percent, reallocMachine& machine1, const reallocMachine& machine2)
{
  reallocGraph::Edge edge;
  int i, j;
  if (machine1.get_id() < machine2.get_id())
    {
      i = machine1.get_id();
      j = machine2.get_id();
    }
  else
    {
      j = machine1.get_id();
      i = machine2.get_id();
    }

  for (std::vector<reallocProcess*>::iterator iter_process1 =
      _selections_swap[machine1.get_id()].begin();
      iter_process1 != _selections_swap[machine1.get_id()].end();
      ++iter_process1)
    {
      if ((*iter_process1)->get_iteration() + instance.get_tabu_length()
          >= instance.get_iteration())
        continue;
      if (instance.get_constraintNeighborhood(machine2.get_neighborhood_id(),
          (*iter_process1)->get_id())
          && (*iter_process1)->get_service()->conflictConstraint(
              machine2.get_id())
          && instance._serviceConstraint(*(*iter_process1), machine2))
        {
          std::vector<reallocProcess*>::iterator iter_process2 =
              _selection_swap.begin();
          for (; iter_process2 != _selection_swap.end(); ++iter_process2)
            {
              if ((*iter_process2)->get_iteration() + instance.get_tabu_length()
                  >= instance.get_iteration())
                continue;
              if (instance.get_constraintNeighborhood(
                  machine1.get_neighborhood_id(), (*iter_process2)->get_id())
                  && (*iter_process2)->get_service()->conflictConstraint(
                      machine1.get_id())
                  && instance._serviceConstraint(*(*iter_process2), machine1))
                {
                  int64 delta = instance.deltaSwapLoadCost(*(*iter_process1),
                      *(*iter_process2));
                  if (delta >= percent * instance.get_initial_cost())
                    continue;
                  delta += instance._deltaReallocationCost(*(*iter_process1),
                      machine2);
                  delta += instance._deltaReallocationCost(*(*iter_process2),
                      machine1);
                  if (edges[i * instance._nb_machines + j] == -1)
                    {
#ifdef REALLOC_OPENMP
#pragma omp critical
#endif
                        {
                          edge = G.addEdge(G.nodeFromId(i), G.nodeFromId(j));
                          edges[i * instance._nb_machines + j] = G.id(edge);
                        }
                      defineSwapEdgeValue(edge,
                          -1 * (delta - percent * instance._cost), delta,
                          (*iter_process1), (*iter_process2));

                    }
                  else
                    {
                      edge = G.edgeFromId(edges[i * instance._nb_machines + j]);
                      if (-1 * (delta - percent * instance._cost)
                          > weights[edge])
                        {
                          defineSwapEdgeValue(edge,
                              -1 * (delta - percent * instance._cost), delta,
                              (*iter_process1), (*iter_process2));
                        }
                    }
                }
            }
        }
    }
}

void
matchingReallocGraph::defineSwapEdges(const tabuReallocInstance& instance,
    double percent)
{

#ifdef REALLOC_OPENMP
  unsigned int j;
#pragma omp parallel for private (j)
  for (unsigned int i = 0; i < instance._selection_max.size(); ++i)
    {
      if (in_selection[i]->_under_used_ressources < instance.get_max_uur()
          - instance.get_empty_matching_iteration())
      continue;
      for (j = i + 1; j < instance._selection_max.size(); ++j)
        {
          if (in_selection[j]->_under_used_ressources < instance.get_max_uur()
              - instance.get_empty_matching_iteration())
          continue;
          defineSwapEdge(instance, percent, *(instance._selection_max[i]),
              *(instance._selection_max[j]));
        }
    }
#else
  for (std::vector<reallocMachine*>::const_iterator iter_machine1 =
      instance._selection_max.begin();
      iter_machine1 != instance._selection_max.end()
          && (*iter_machine1)->get_under_used_ressources()
              >= instance.get_max_uur()
                  - instance.get_empty_matching_iteration(); ++iter_machine1)
    {
      _selection_swap.clear();
      randomizedProcessesSelection((*iter_machine1)->_processes_allocation,
          MAX_PROCESS_SELECTION, _selection_swap, instance);
      for (std::vector<reallocMachine*>::const_iterator iter_machine2 =
          instance._selection_min.begin();
          iter_machine2 != instance._selection_min.end(); ++iter_machine2)
        {
          if ((*iter_machine1)->get_id() == (*iter_machine2)->get_id()
              || (*iter_machine2)->get_under_used_ressources()
                  > (*iter_machine1)->get_under_used_ressources())
            continue;
          defineSwapEdge(instance, percent, *(*iter_machine2),
              *(*iter_machine1));
        }
    }
#endif
}

void
matchingReallocGraph::matching(tabuReallocInstance& instance)
{
  MaxWeightedMatching<reallocGraph, reallocGraph::EdgeMap<int64> > matching(G,
      weights);

  matching.run();
  instance._current_moves.resize(matching.matchingSize(),
      reallocMove(LONG_MAX));
  std::vector<reallocMove>::iterator iter = instance._current_moves.begin();

  for (int i = 0; i < instance._nb_machines; ++i)
    {
      reallocGraph::Node node = matching.mate(G.nodeFromId(i));
      if (node != INVALID)
        {
          int j = reallocGraph::id(node);
          if (i < j)
            {
              (*iter) = edge_move[G.edgeFromId(
                  edges[i * instance._nb_machines + j])];
              ++iter;
            }
        }
    }
}

