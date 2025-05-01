#ifndef RECCHECK
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
// add or remove necessary headers as you please
#endif

#include "schedwork.h"

using namespace std;

// a constant that can be used to indicate an INVALID 
// worker ID if that is useful to your implementation.
// Feel free to not use or delete.
static const Worker_T INVALID_ID = (unsigned int)-1;

// Add prototypes for any helper functions here
bool scheduleHelper(
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched,
    vector<size_t>& shiftsWorked,
    size_t day,
    size_t workerPosition);

bool tryNextWorker(
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched,
    vector<size_t>& shiftsWorked,
    size_t day,
    size_t workerPosition,
    size_t workerId);

bool workerAlreadyScheduled(
    const DailySchedule& sched,
    size_t day,
    size_t currentPosition,
    Worker_T workerId);

bool checkPreviousPosition(
    const DailySchedule& sched,
    size_t day,
    size_t position,
    Worker_T workerId);


// Add your implementation of schedule() and other helper functions here
bool schedule(
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched
)
{
    if(avail.size() == 0U){
        return false;
    }
    sched.clear();
    
    // Initialize schedule with empty days
    sched.resize(avail.size(), vector<Worker_T>(dailyNeed, INVALID_ID));
    
    // Track shifts worked by each worker
    vector<size_t> shiftsWorked(avail[0].size(), 0);
    
    // Start recursive scheduling at day 0, position 0
    return scheduleHelper(avail, dailyNeed, maxShifts, sched, shiftsWorked, 0, 0);
}

bool scheduleHelper(
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched,
    vector<size_t>& shiftsWorked,
    size_t day,
    size_t workerPosition)
{
    // Base case: we've scheduled all days
    if (day >= avail.size()) {
        return true;
    }
    
    // If we've filled all worker positions for this day, move to next day
    if (workerPosition >= dailyNeed) {
        return scheduleHelper(avail, dailyNeed, maxShifts, sched, shiftsWorked, day + 1, 0);
    }
    
    // Try each worker for the current position
    return tryNextWorker(avail, dailyNeed, maxShifts, sched, shiftsWorked, day, workerPosition, 0);
}

bool tryNextWorker(
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched,
    vector<size_t>& shiftsWorked,
    size_t day,
    size_t workerPosition,
    size_t workerId)
{
    // Base case: we've tried all workers without success
    if (workerId >= avail[0].size()) {
        return false;
    }
    
    // Check if this worker is available and hasn't reached max shifts
    if (avail[day][workerId] && shiftsWorked[workerId] < maxShifts) {
        // Check if worker is already scheduled for this day
        if (!workerAlreadyScheduled(sched, day, workerPosition, workerId)) {
            // Schedule this worker
            sched[day][workerPosition] = workerId;
            shiftsWorked[workerId]++;
            
            // Recurse to next position
            if (scheduleHelper(avail, dailyNeed, maxShifts, sched, shiftsWorked, day, workerPosition + 1)) {
                return true;
            }
            
            // Backtrack if unsuccessful
            shiftsWorked[workerId]--;
        }
    }
    
    // Try next worker
    return tryNextWorker(avail, dailyNeed, maxShifts, sched, shiftsWorked, day, workerPosition, workerId + 1);
}

bool workerAlreadyScheduled(
    const DailySchedule& sched,
    size_t day,
    size_t currentPosition,
    Worker_T workerId)
{
    // Base case: we've checked all previous positions
    if (currentPosition == 0) {
        return false;
    }
    
    // Check if worker is scheduled in any previous position today
    return checkPreviousPosition(sched, day, currentPosition - 1, workerId);
}

bool checkPreviousPosition(
    const DailySchedule& sched,
    size_t day,
    size_t position,
    Worker_T workerId)
{
    // Check if worker is scheduled at this position
    if (sched[day][position] == workerId) {
        return true;
    }
    
    // Base case: we've checked position 0
    if (position == 0) {
        return false;
    }
    
    // Check previous position
    return checkPreviousPosition(sched, day, position - 1, workerId);
}
