#pragma once

#include <cinttypes>
#include <memory>
#include <vector>

#include "geo/latlng.h"

#include "cista/reflection/comparable.h"
#include "cista/strong.h"

#include "nigiri/common/interval.h"
#include "nigiri/types.h"

#include "icc/elevators/get_state_changes.h"

namespace nigiri {
struct rt_timetable;
}

namespace icc {

template <typename K, typename V>
using vector_map = nigiri::vector_map<K, V>;

template <typename T>
using hash_set = nigiri::hash_set<T>;

template <typename K, typename V>
using hash_map = nigiri::hash_map<K, V>;

enum class status : bool { kActive, kInactive };

inline status status_from_str(std::string_view s) {
  return s == "INACTIVE" ? status::kInactive : status::kActive;
}

using elevator_idx_t = cista::strong<std::uint32_t, struct elevator_idx_>;

struct elevator {
  friend bool operator==(elevator const&, elevator const&) = default;

  std::int64_t id_;
  geo::latlng pos_;
  status status_;
  std::string desc_;
  std::vector<nigiri::interval<nigiri::unixtime_t>> out_of_service_;
  std::vector<state_change<nigiri::unixtime_t>> state_changes_{
      intervals_to_state_changes(out_of_service_)};
};

using rtt_ptr_t = std::shared_ptr<nigiri::rt_timetable>;

}  // namespace icc