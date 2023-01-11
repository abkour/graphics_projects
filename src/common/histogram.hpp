#ifndef _HISTOGRAM_HPP_
#define _HISTOGRAM_HPP_

#include <algorithm>
#include <iostream>
#include <vector>

class HistogramEntry {
public:
    HistogramEntry(float frame_time)
        : frame_time(frame_time) 
    {}
    float frame_time;

    friend bool operator<(HistogramEntry h0, HistogramEntry h1) {
        return h0.frame_time < h1.frame_time;
    }
};

class HistogramRecord {
public:
    HistogramRecord() 
        : low_one_percent(0)
        , low_ten_percent(0)
        , avg(0)
        , high_one_percent(0)
        , high_ten_percent(0)
    {}
    
    float low_one_percent;
    float low_ten_percent;
    float avg;
    float high_one_percent;
    float high_ten_percent;
};

extern std::ostream& operator<<(std::ostream& os, const HistogramRecord& hist_rec) {
    return os << "Printing histogram record(in ms)...\n" 
    << "Average: " << hist_rec.avg * 1000.f << '\n'
    << "Low 1%: " << hist_rec.low_one_percent * 1000.f << '\n'
    << "Low 10%: " << hist_rec.low_ten_percent * 1000.f << '\n'
    << "High 1%: " << hist_rec.high_one_percent * 1000.f << '\n'
    << "High 10%: " << hist_rec.high_ten_percent * 1000.f << '\n';
}


class Histogram {

public:

    Histogram() {}

    Histogram(const std::size_t expected_sample_count) {
        histogram_entries.reserve(expected_sample_count);
    }

    void supply_frametime_ms(float frametime_ms) {
        histogram_entries.emplace_back(frametime_ms);
    }

    HistogramRecord get_histogram_record() {
        analyse_entries();
        return histogram_record;
    }

private:

    std::vector<HistogramEntry> histogram_entries;
    HistogramRecord histogram_record;

private:

    // Implementation details
    void analyse_entries() {
        std::sort(histogram_entries.begin(), histogram_entries.end());
        
        for(int i = 0; i < histogram_entries.size(); ++i) {
            histogram_record.avg += histogram_entries[i].frame_time;
        }
        histogram_record.avg /= histogram_entries.size();
    
        // Low 1%
        for(int i = 0; i < histogram_entries.size() / 100; ++i) {
            histogram_record.low_one_percent += histogram_entries[i].frame_time;
        }
        histogram_record.low_one_percent /= histogram_entries.size() / 100;

        // Low 10%
        for(int i = 0; i < histogram_entries.size() / 10; ++i) {
            histogram_record.low_ten_percent += histogram_entries[i].frame_time;
        }
        histogram_record.low_ten_percent /= histogram_entries.size() / 10;
    
        for(int i = histogram_entries.size() - histogram_entries.size() / 100;
            i < histogram_entries.size(); ++i) {
            histogram_record.high_one_percent += histogram_entries[i].frame_time;
        }
        histogram_record.high_one_percent /= histogram_entries.size() / 100;

        for(int i = histogram_entries.size() - histogram_entries.size() / 10;
            i < histogram_entries.size(); ++i) {
            histogram_record.high_ten_percent += histogram_entries[i].frame_time;
        }
        histogram_record.high_ten_percent /= histogram_entries.size() / 10;
    }
};

#endif