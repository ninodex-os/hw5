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
    // Check if we've selected enough workers for this day
    if (current.size() == dailyNeed) {
        // Make a copy of the vector before modifying shifts
        sched[day] = current;
        
        // Update shifts count for all selected workers
        for (size_t i = 0; i < current.size(); i++) {
            shifts[current[i]]++;
        }
        
        // Try scheduling the next day
        bool success = scheduleDay(day + 1, shifts, avail, dailyNeed, maxShifts, sched);
        
        // Backtrack - undo the shift assignments
        for (size_t i = 0; i < current.size(); i++) {
            shifts[current[i]]--;
        }
        
        return success;
    }
    
    // Bounds checking
    if (index >= available.size()) return false;
    
    // Early termination - not enough workers left to consider
    if (available.size() - index < workersNeeded - current.size()) return false;
    
    // Try including the current worker
    current.push_back(available[index]);
    if (generateCombinations(available, index + 1, current, workersNeeded - 1, 
                           shifts, day, avail, dailyNeed, maxShifts, sched)) {
        return true;
    }
    current.pop_back();
    
    // Try excluding the current worker
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
    
    // Only add if worker is available and hasn't reached shift limit
    if (avail[day][currentWorker] && shifts[currentWorker] < maxShifts) {
        available.push_back(currentWorker);
    }
    
    // Continue with next worker
    collectAvailable(avail, day, shifts, maxShifts, currentWorker + 1, available);
}


// Recursively update shifts for workers in combination
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
    
    // Make sure shift count is valid (not negative and not exceeding maximum)
    if (shifts[w] > shifts.size()) { 
        shifts[w] -= delta;
        return false;
    }
    
    if (updateShifts(workers, shifts, index + 1, delta)) {
        return true;
    }
    
    // If updating remaining shifts failed, backtrack this one too
    shifts[w] -= delta;
    return false;
}
