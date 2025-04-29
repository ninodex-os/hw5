#ifndef RECCHECK
#include <iostream>
#include <fstream>
#include <vector>
#endif

#include "schedwork.h"

using namespace std;

// Helper function prototypes
static bool scheduleDay(
    size_t day,
    vector<size_t>& shifts,
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched
);

static bool generateCombinations(
    const vector<Worker_T>& available,
    size_t index,
    vector<Worker_T>& current,
    size_t workersNeeded,
    vector<size_t>& shifts,
    size_t day,
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched
);

static void collectAvailable(
    const AvailabilityMatrix& avail,
    size_t day,
    const vector<size_t>& shifts,
    size_t maxShifts,
    Worker_T currentWorker,
    vector<Worker_T>& available
);

static bool updateShifts(
    const vector<Worker_T>& workers,
    vector<size_t>& shifts,
    size_t index,
    int delta
);

// Main scheduling function
bool schedule(
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched
) {
    if (avail.empty() || dailyNeed == 0 || maxShifts == 0) {
        return false;
    }

    sched.clear();
    sched.resize(avail.size());
    vector<size_t> shifts(avail[0].size(), 0);
    
    return scheduleDay(0, shifts, avail, dailyNeed, maxShifts, sched);
}

// Recursive function to handle each day's scheduling
static bool scheduleDay(
    size_t day,
    vector<size_t>& shifts,
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched
) {
    if (day >= avail.size()) {
        return true;
    }

    vector<Worker_T> available;
    collectAvailable(avail, day, shifts, maxShifts, 0, available);

    if (available.size() < dailyNeed) {
        return false;
    }

    vector<Worker_T> current;
    return generateCombinations(available, 0, current, dailyNeed, shifts, day, 
                               avail, dailyNeed, maxShifts, sched);
}

// Recursively generate valid worker combinations for current day
static bool generateCombinations(
    const vector<Worker_T>& available,
    size_t index,
    vector<Worker_T>& current,
    size_t workersNeeded,
    vector<size_t>& shifts,
    size_t day,
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched
) {
    if (current.size() == dailyNeed) {
        if (!updateShifts(current, shifts, 0, 1)) return false;
        sched[day] = current;
        
        bool success = scheduleDay(day + 1, shifts, avail, dailyNeed, maxShifts, sched);
        
        updateShifts(current, shifts, 0, -1); // Backtrack
        return success;
    }
    
    if (index >= available.size()) return false;
    if (available.size() - index < workersNeeded - current.size()) return false;
    
    // Include current worker
    current.push_back(available[index]);
    if (generateCombinations(available, index + 1, current, workersNeeded - 1, 
                            shifts, day, avail, dailyNeed, maxShifts, sched)) {
        return true;
    }
    current.pop_back();
    
    // Exclude current worker
    return generateCombinations(available, index + 1, current, workersNeeded, 
                               shifts, day, avail, dailyNeed, maxShifts, sched);
}

// Recursively collect available workers for current day
static void collectAvailable(
    const AvailabilityMatrix& avail,
    size_t day,
    const vector<size_t>& shifts,
    size_t maxShifts,
    Worker_T currentWorker,
    vector<Worker_T>& available
) {
    if (currentWorker >= avail[day].size()) return;
    
    if (avail[day][currentWorker] && shifts[currentWorker] < maxShifts) {
        available.push_back(currentWorker);
    }
    
    collectAvailable(avail, day, shifts, maxShifts, currentWorker + 1, available);
}

// Recursively update shifts for workers in combination
static bool updateShifts(
    const vector<Worker_T>& workers,
    vector<size_t>& shifts,
    size_t index,
    int delta
) {
    if (index >= workers.size()) return true;
    
    Worker_T w = workers[index];
    shifts[w] += delta;
    
    if (shifts[w] < 0 || shifts[w] > shifts.size()) { // Sanity check
        shifts[w] -= delta;
        return false;
    }
    
    return updateShifts(workers, shifts, index + 1, delta);
}
