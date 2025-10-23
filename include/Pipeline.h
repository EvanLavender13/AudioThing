#pragma once

#include "AudioFilter.h"
#include <vector>

// Pipeline that chains multiple audio filters
class FilterPipeline {
public:
    FilterPipeline();
    ~FilterPipeline();
    
    // Disable copy
    FilterPipeline(const FilterPipeline&) = delete;
    FilterPipeline& operator=(const FilterPipeline&) = delete;
  
    // Process audio through all filters in sequence
 std::vector<double> process(const std::vector<double>& input);
    
 // Reset all filters
    void reset();
    
    // Remove all filters
    void clearFilters();
    
// Check if pipeline has any filters
    bool isEmpty() const;

private:
    std::vector<AudioFilter*> filters;
};
