#include "Module.h"

#include "Task.h"

#include <ostream>
#include <cassert>

namespace {
template <typename T>
const typename T::value_type &maxP(typename T::const_iterator Begin,
                                   const typename T::const_iterator &End) {
  assert(Begin != End);
  const typename T::value_type *Min = &(*Begin);
  for (auto I = ++Begin; I != End; ++I) {
    Min = &(std::max(*I, *Min));
  }
  return *Min;
}
}

namespace perf {

void Module::start(Task &Task, const time_point &Time) {
  if (!Started) {
    Started = true;
    StartTime = Time;
  }
  auto &TF = Frame.local();
  TF.emplace_back(Task, Time);
  TaskDuration.insert(std::make_pair(&Task, duration()));
}

void Module::stop() {
  auto Now = clock::now();

  StopTime.local() = Now;

  auto &TF = Frame.local();
  assert(TF.back().Task != nullptr);
  {
    tbb::concurrent_hash_map<const Task *, duration>::accessor A;
    bool found = TaskDuration.find(A, TF.back().Task);
    assert(found); // It's inserted in start()
    if (!found) {
      std::stringstream sstr;
      sstr << "perf::stop - ***ERROR*** thread id: "
           << tbb::this_tbb_thread::get_id() << "\n";
      throw std::runtime_error("Frame.local() cannot be found.");
    }
    A->second += Now - TF.back().StartTime;
  }
  TF.pop_back();
}

void Module::report(std::ostream &Out) const {
  if (StopTime.empty()) {
    Out << "Perf: No data collected.\n";
    return;
  }
  time_point LastStopTime =
      maxP<decltype(StopTime)>(std::begin(StopTime), std::end(StopTime));
  Out << "\tModule " << Name << ": "
      << std::chrono::duration<double>(LastStopTime - StartTime).count()
      << " seconds.\n";
  for (const auto &TD : TaskDuration) {
    assert(TD.first != nullptr);
    Out << "\t\tTask " << TD.first->getName() << " : "
        << std::chrono::duration<double>(TD.second).count() << " seconds.\n";
  }
}

void Module::reset() {
  Started = false;
  Frame.clear();
  TaskDuration.clear();
  StopTime.clear();
}
}
