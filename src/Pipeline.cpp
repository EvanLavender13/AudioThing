#include "Pipeline.h"

FilterPipeline::FilterPipeline() {}

FilterPipeline::~FilterPipeline() { clearFilters(); }

std::vector<double> FilterPipeline::process(const std::vector<double> &input) {
  if (filters.empty()) {
    return input;
  }

  std::vector<double> output = input;
  for (size_t i = 0; i < filters.size(); ++i) {
    output = filters[i]->process(output);
  }

  return output;
}

void FilterPipeline::reset() {
  for (size_t i = 0; i < filters.size(); ++i) {
    filters[i]->reset();
  }
}

void FilterPipeline::clearFilters() {
  for (size_t i = 0; i < filters.size(); ++i) {
    delete filters[i];
  }
  filters.clear();
}

bool FilterPipeline::isEmpty() const { return filters.empty(); }