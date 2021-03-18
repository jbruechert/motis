#pragma once

#include <cstdint>
#include <optional>
#include <type_traits>
#include <vector>

#include "cista/reflection/comparable.h"

#include "motis/core/schedule/time.h"
#include "motis/core/journey/journey.h"

#include "motis/paxmon/compact_journey.h"

namespace motis::paxmon {

struct edge;

struct data_source {
  CISTA_COMPARABLE()

  std::uint32_t primary_ref_{};
  std::uint32_t secondary_ref_{};
};

enum class group_source_flags : std::uint8_t {
  NONE = 0,
  MATCH_INEXACT_TIME = 1U << 0U,
  MATCH_JOURNEY_SUBSET = 1U << 1U,
  MATCH_REROUTED = 1U << 2U,
  FORECAST = 1U << 3U
};

inline constexpr group_source_flags operator|(group_source_flags const& a,
                                              group_source_flags const& b) {
  return static_cast<group_source_flags>(
      static_cast<std::underlying_type_t<group_source_flags>>(a) |
      static_cast<std::underlying_type_t<group_source_flags>>(b));
}

inline constexpr group_source_flags operator&(group_source_flags const& a,
                                              group_source_flags const& b) {
  return static_cast<group_source_flags>(
      static_cast<std::underlying_type_t<group_source_flags>>(a) &
      static_cast<std::underlying_type_t<group_source_flags>>(b));
}

inline constexpr group_source_flags operator|=(group_source_flags& a,
                                               group_source_flags const& b) {
  a = a | b;
  return a;
}

inline constexpr group_source_flags operator&=(group_source_flags& a,
                                               group_source_flags const& b) {
  a = a & b;
  return a;
}

struct passenger_group {
  inline bool valid() const { return !edges_.empty(); }

  compact_journey compact_planned_journey_;
  std::uint64_t id_{};
  data_source source_{};
  std::uint16_t passengers_{1};
  motis::time planned_arrival_time_{INVALID_TIME};
  group_source_flags source_flags_{group_source_flags::NONE};
  bool ok_{true};
  motis::time added_time_{INVALID_TIME};
  float probability_{1.0F};
  std::uint8_t generation_{};
  std::uint64_t previous_version_{};
  std::vector<edge*> edges_{};
};

inline passenger_group make_passenger_group(
    compact_journey&& cj, data_source const source,
    std::uint16_t const passengers, motis::time const planned_arrival_time,
    group_source_flags const source_flags = group_source_flags::NONE,
    float const probability = 1.0F, motis::time added_time = INVALID_TIME,
    std::optional<std::uint64_t> previous_version = std::nullopt,
    std::uint8_t generation = 0, std::uint64_t id = 0ULL) {
  return passenger_group{std::move(cj),
                         id,
                         source,
                         passengers,
                         planned_arrival_time,
                         source_flags,
                         true,
                         added_time,
                         probability,
                         generation,
                         previous_version.value_or(id),
                         {}};
}

inline bool is_planned_group(passenger_group const* grp) {
  return ((grp->source_flags_ & group_source_flags::FORECAST) !=
          group_source_flags::FORECAST) &&
         grp->probability_ == 1.0F;
}

}  // namespace motis::paxmon
