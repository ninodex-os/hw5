#ifndef RECCHECK
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;
// add or remove necessary headers as you please

#endif

#include "schedwork.h"
// Add your implementation of schedule() and other helper functions here
static bool backtrack(
    size_t day,
    vector<size_t>& shifts,
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched
);

static bool generateAndTestCombos(
    const vector<Worker_T>& available,
    size_t start,
    vector<Worker_T>& currentCombo,
    size_t day,
    vector<size_t>& shifts,
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched
);

bool schedule(
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched
) {
    if (avail.empty() || dailyNeed == 0 || maxShifts == 0) {
        return false;
    }

    const size_t n = avail.size();
    sched.clear();

// Add your code below
    sched.resize(n);

    const size_t k = avail[0].size();
    vector<size_t> shifts(k, 0);

    return backtrack(0, shifts, avail, dailyNeed, maxShifts, sched);
}

static bool backtrack(
    size_t day,
    vector<size_t>& shifts,
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched
) {
    const size_t n = avail.size();
    if (day == n) {
        return true;
    }

    vector<Worker_T> available;
    for (Worker_T w = 0; w < avail[day].size(); ++w) {
        if (avail[day][w] && shifts[w] < maxShifts) {
            available.push_back(w);
        }
    }

    if (available.size() < dailyNeed) {
        return false;
    }

    sort(available.begin(), available.end());

    vector<Worker_T> currentCombo;
    return generateAndTestCombos(available, 0, currentCombo, day, shifts, avail, dailyNeed, maxShifts, sched);
}

static bool generateAndTestCombos(
    const vector<Worker_T>& available,
    size_t start,
    vector<Worker_T>& currentCombo,
    size_t day,
    vector<size_t>& shifts,
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched
) {
    if (currentCombo.size() == dailyNeed) {
        sched[day] = currentCombo;

        for (Worker_T w : currentCombo) {
            shifts[w]++;
        }

        bool success = backtrack(day + 1, shifts, avail, dailyNeed, maxShifts, sched);

        if (success) {
            return true;
        } else {
            for (Worker_T w : currentCombo) {
                shifts[w]--;
            }
            sched[day].clear();
            return false;
        }
    }

    for (size_t i = start; i < available.size(); ++i) {
        currentCombo.push_back(available[i]);
        if (generateAndTestCombos(available, i + 1, currentCombo, day, shifts, avail, dailyNeed, maxShifts, sched)) {
            return true;
        }
        currentCombo.pop_back();
    }

    return false;
}