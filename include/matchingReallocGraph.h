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

#ifndef MATCHINGREALLOCGRAPH_H_
#define MATCHINGREALLOCGRAPH_H_
#include "tabuReallocInstance.h"
#include <lemon/matching.h>
using namespace lemon;
//#define REALLOC_LISTGRAPH
#ifdef REALLOC_LISTGRAPH
#include <lemon/list_graph.h>
typedef ListGraph reallocGraph;
#else
#include <lemon/smart_graph.h>
typedef SmartGraph reallocGraph;
#endif

class matchingReallocGraph
{
private:
  reallocGraph G;
  std::vector<int> edges;
  reallocGraph::EdgeMap<int64> weights;
  reallocGraph::EdgeMap<reallocMove> edge_move;
  static std::vector<reallocProcess*> _selection_swap;
  static std::vector<std::vector<reallocProcess*> > _selections_swap;
  static vector_bool _constraint;
#ifdef REALLOC_OPENMP
  static std::vector< std::vector<reallocProcess*> > _selections_swap2;
#endif
public:
  matchingReallocGraph(int nb_machines);
  ~matchingReallocGraph();
  static void
  init_selection(int nb_machines);

  void
  matching(tabuReallocInstance& instance);

  void
  defineAddSwapEdgesOptimised(const tabuReallocInstance& instance,
      double percent);
  //Obsolete method
  void
  defineAddEdges(const tabuReallocInstance& instance, double percent);
  //Obsolete method
  void
  defineSwapEdges(const tabuReallocInstance& instance, double percent);

protected:
  void
  defineAddEdgeValue(const reallocGraph::Edge &in_edge, const int64& in_cost,
      const int64& in_delta, reallocProcess * in_process,
      reallocMachine * in_machine);
  void
  defineSwapEdgeValue(const reallocGraph::Edge &in_edge, const int64& in_cost,
      const int64& in_delta, reallocProcess * in_process,
      reallocProcess * in_process_2);
  //A Speed Steepest Descent Stage Graph
  void
  defineAddSwapEdgeStage1(const tabuReallocInstance& instance, double percent,
      reallocMachine& machine1, const reallocMachine& machine2);
  //A Hill Climbing Stage Graph
  void
  defineAddSwapEdgeStage2(const tabuReallocInstance& instance, double percent,
      reallocMachine& machine1, const reallocMachine& machine2);

  void
  defineAddEdge(const tabuReallocInstance& instance, double percent,
      reallocMachine& machine1, const reallocMachine& machine2);

  void
  defineSwapEdge(const tabuReallocInstance& instance, double percent,
      reallocMachine& machine1, const reallocMachine& machine2);

};

#endif
