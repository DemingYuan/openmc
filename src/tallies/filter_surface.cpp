#include "openmc/tallies/filter_surface.h"

#include <sstream>

#include "openmc/error.h"
#include "openmc/surface.h"

namespace openmc {

void
SurfaceFilter::from_xml(pugi::xml_node node)
{
  surfaces_ = get_node_array<int32_t>(node, "bins");
  n_bins_ = surfaces_.size();
}

void
SurfaceFilter::initialize()
{
  for (auto& s : surfaces_) {
    auto search = surface_map.find(s);
    if (search != surface_map.end()) {
      s = search->second;
    } else {
      std::stringstream err_msg;
      err_msg << "Could not find surface " << s
              << " specified on tally filter.";
      fatal_error(err_msg);
    }
  }

  for (int i = 0; i < surfaces_.size(); i++) {
    map_[surfaces_[i]] = i;
  }
}

void
SurfaceFilter::get_all_bins(Particle* p, int estimator, FilterMatch& match)
const
{
  auto search = map_.find(std::abs(p->surface)-1);
  if (search != map_.end()) {
    match.bins_.push_back(search->second + 1);
    if (p->surface < 0) {
      match.weights_.push_back(-1);
    } else {
      match.weights_.push_back(1);
    }
  }
}

void
SurfaceFilter::to_statepoint(hid_t filter_group) const
{
  Filter::to_statepoint(filter_group);
  std::vector<int32_t> surface_ids;
  for (auto c : surfaces_) surface_ids.push_back(surfaces[c]->id_);
  write_dataset(filter_group, "bins", surface_ids);
}

std::string
SurfaceFilter::text_label(int bin) const
{
  return "Surface " + std::to_string(surfaces[surfaces_[bin-1]]->id_);
}

} // namespace openmc
