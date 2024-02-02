#include "motis/paxforecast/paxforecast.h"

#include <memory>

#include "utl/read_file.h"

#include "motis/core/common/logging.h"
#include "motis/module/message.h"

#include "motis/paxmon/paxmon_data.h"

#include "motis/paxforecast/api/apply_measures.h"
#include "motis/paxforecast/api/metrics.h"
#include "motis/paxforecast/behavior/default_behavior.h"
#include "motis/paxforecast/behavior/parser.h"
#include "motis/paxforecast/monitoring_update.h"

#include "motis/paxforecast/universe_data.h"

using namespace motis::module;
using namespace motis::logging;
using namespace motis::paxmon;

namespace motis::paxforecast {

paxforecast::paxforecast() : module("Passenger Forecast", "paxforecast") {
  param(forecast_filename_, "forecast_results",
        "output file for forecast messages");
  param(behavior_stats_filename_, "behavior_stats",
        "output file for behavior statistics");
  param(routing_cache_filename_, "routing_cache",
        "optional cache file for routing queries");
  param(behavior_file_, "behavior", "behavior model configuration file");
  param(stats_file_, "stats", "statistics file");
  param(min_delay_improvement_, "min_delay_improvement",
        "minimum required arrival time improvement for major delay "
        "alternatives (minutes)");
  param(revert_forecasts_, "revert_forecasts",
        "revert forecasts if broken transfers become valid again");
  param(probability_threshold_, "probability_threshold",
        "minimum allowed route probability (routes with lower probability are "
        "dropped)");
  param(uninformed_pax_, "uninformed_pax",
        "percentage of passengers that ignore forecasts and announcements");
  param(major_delay_switch_, "major_delay_switch",
        "percentage of passengers that may switch to alternatives in case of "
        "an expected major delay");
  param(
      allow_start_metas_, "allow_start_metas",
      "allow using equivalent stations as start station in alternative routes");
  param(allow_dest_metas_, "allow_destination_metas",
        "allow using equivalent stations as destination station in alternative "
        "routes");
}

paxforecast::~paxforecast() = default;

void paxforecast::init(motis::module::registry& reg) {
  if (!behavior_file_.empty()) {
    auto const config = utl::read_file(behavior_file_.c_str());
    if (config) {
      behavior_ = std::make_unique<behavior::probabilistic::passenger_behavior>(
          behavior::parse_passenger_behavior(config.value()));
    } else {
      LOG(log_level::error)
          << "could not read passenger behavior model configuration from "
          << behavior_file_;
      throw std::runtime_error{
          "could not read passenger behavior model configuration"};
    }
  } else {
    LOG(info) << "no passenger behavior model configuration specified, using "
                 "defaults";
    behavior_ = std::make_unique<behavior::probabilistic::passenger_behavior>(
        behavior::get_default_behavior());
  }

  stats_writer_ = std::make_unique<stats_writer>(stats_file_);

  if (!forecast_filename_.empty()) {
    forecast_file_.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    forecast_file_.open(forecast_filename_);
  }

  if (!behavior_stats_filename_.empty()) {
    behavior_stats_file_.exceptions(std::ios_base::failbit |
                                    std::ios_base::badbit);
    behavior_stats_file_.open(behavior_stats_filename_);
    behavior_stats_file_
        << "system_time,event_type,group_route_count,cpg_count,"
        << "found_alt_count_avg,picked_alt_count_avg,"
        << "best_alt_prob_avg,second_alt_prob_avg\n";
  }

  if (!routing_cache_filename_.empty()) {
    routing_cache_.open(routing_cache_filename_);
  }

  reg.subscribe("/paxmon/monitoring_update",
                [&, this](msg_ptr const& msg) {
                  auto& data = *get_shared_data<paxmon_data*>(
                      to_res_id(global_res_id::PAX_DATA));
                  on_monitoring_update(*this, data, msg);
                  return nullptr;
                },
                {});

  reg.subscribe("/paxmon/universe_forked",
                [&](msg_ptr const& msg) {
                  auto const ev = motis_content(PaxMonUniverseForked, msg);
                  LOG(info)
                      << "paxforecast: /paxmon/universe_forked: new="
                      << ev->new_universe() << ", base=" << ev->base_universe();
                  universe_storage_.universe_created(ev->new_universe());
                  return nullptr;
                },
                {});

  reg.subscribe("/paxmon/universe_destroyed",
                [&](msg_ptr const& msg) {
                  auto const ev = motis_content(PaxMonUniverseDestroyed, msg);
                  LOG(info) << "paxforecast: /paxmon/universe_destroyed: "
                            << ev->universe();
                  universe_storage_.universe_destroyed(ev->universe());
                  return nullptr;
                },
                {});

  reg.register_op("/paxforecast/apply_measures",
                  [&, this](msg_ptr const& msg) -> msg_ptr {
                    auto& data = *get_shared_data<paxmon_data*>(
                        to_res_id(global_res_id::PAX_DATA));
                    return api::apply_measures(*this, data, msg);
                  },
                  {});

  reg.register_op("/paxforecast/metrics",
                  [&, this](msg_ptr const& msg) -> msg_ptr {
                    return api::metrics(*this, msg);
                  },
                  {});
}

}  // namespace motis::paxforecast
