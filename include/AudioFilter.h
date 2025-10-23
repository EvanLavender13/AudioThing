#ifndef AUDIO_FILTER_H
#define AUDIO_FILTER_H

#include <vector>

// Base class for audio filters
class AudioFilter {
public:
    virtual ~AudioFilter() {}
    
    // Process audio buffer and return filtered result
    virtual std::vector<double> process(const std::vector<double>& input) = 0;
    
    // Reset filter state
    virtual void reset() {}
};

// Pass-through filter (no processing)
class PassThroughFilter : public AudioFilter {
public:
    std::vector<double> process(const std::vector<double>& input) override {
        return input;
    }
};

#endif // AUDIO_FILTER_H
