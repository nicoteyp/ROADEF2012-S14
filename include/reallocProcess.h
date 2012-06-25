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

#ifndef REALLOCPROCESS_H_
#define REALLOCPROCESS_H_
#include "reallocDefinitions.h"
#include "reallocMachine.h"

class reallocService;
typedef std::vector<reallocService> vector_reallocService;
typedef std::vector<reallocService>::iterator vectorIterator_reallocService;

class reallocProcess
{
  friend class reallocInstance;
public:
  inline reallocID
  get_id_initial_allocation() const
  {
    return _initial_allocation->get_id();
  }
  ;
  inline reallocID
  get_id_allocation() const
  {
    return _allocation->get_id();
  }
  ;
  inline reallocID
  get_id() const
  {
    return _id;
  }
  ;
  inline int64
  get_iteration() const
  {
    return _iteration;
  }
  ;
  inline reallocService*
  get_service() const
  {
    return _service;
  }
  ;
  inline reallocMachine*
  get_allocation() const

  {
    return _allocation;
  }
  ;
  inline reallocMachine*
  get_initial_allocation() const
  {
    return _initial_allocation;
  }
  ;
  inline int
  get_move_cost() const
  {
    return _move_cost;
  }
  ;
  inline int64
  get_load_cost() const
  {
    return _load_cost;
  }
  ;
  inline int64
  get_delta_cost() const
  {
    return _delta_cost;
  }
  ;
public:
  reallocProcess(const int nb_resources, const float in_move_cost = 0);
  ~reallocProcess();
  void
  write(std::ostream & fo = std::cout);
  void
  load(std::istream &fi, vector_reallocService& vector_services);
  void
  allocation(reallocMachine* in_machine);

  reallocID
  get_id_service() const;

  void
  set_iteration(const int64 in_iteration);

  void
  set_initial_allocation(reallocMachine* in_allocation);
  void
  set_service(reallocService* in_service);
  vector_int _requirement;

private:
  int _move_cost;
  int64 _load_cost;
  int64 _delta_cost;
  void
  set_id();
  reallocID _id;
  static reallocID _id_count;
  reallocMachine* _initial_allocation;
  reallocMachine* _allocation;
  reallocService* _service;
  int64 _iteration;

};

inline bool
compareProcess(const reallocProcess* first, const reallocProcess* second)
{
  return first->get_load_cost() > second->get_load_cost();
}

typedef std::vector<reallocProcess> vector_reallocProcess;
typedef std::vector<reallocProcess>::iterator vectorIterator_reallocProcess;
typedef std::vector<reallocProcess>::const_iterator vectorConstIterator_reallocProcess;
#endif
