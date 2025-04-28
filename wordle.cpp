#ifndef RECCHECK
//For debugging
#include <iostream>
//For std::remove
#include <vector>
#include <map>
#include <set>
#endif

#include "wordle.h"
#include "dict-eng.h"
using namespace std;
// Add prototypes of helper functions here
// Helper prototype
void backtrack(
    vector<char>& word,
    const vector<int>& var_pos,
    size_t var_index,
    map<char, int> remaining_required,
    const set<string>& dict,
    set<string>& result
);

set<string> wordle(
    const string& in,
    const string& floating,
    const set<string>& dict)
{
    // prrocess input to find variable positions (1 loop)
    vector<int> var_pos;
    vector<char> word;
    map<char, int> fixed_counts;
    for (size_t i = 0; i < in.size(); ++i) { // Loop 1/5
        if (in[i] == '-') {
            var_pos.push_back(i);
            word.push_back('-');
        } else {
            word.push_back(in[i]);
            fixed_counts[in[i]]++;
        }
    }

    // compute floating counts (1 loop)
    map<char, int> floating_counts;
    for (char c : floating) { // Loop 2/5
        floating_counts[c]++;
    }

    // compute required counts (1 loop)
    map<char, int> required_counts;
    int sum_required = 0;
    for (const auto& pair : floating_counts) { // Loop 3/5
        char c = pair.first;
        int required = pair.second - fixed_counts[c];
        if (required > 0) {
            required_counts[c] = required;
            sum_required += required;
        }
    }

    // arly exit check (no loop)
    if (sum_required > static_cast<int>(var_pos.size())) {
        return {};
    }

    // check words recursively
    set<string> result;
    backtrack(word, var_pos, 0, required_counts, dict, result);
    return result;
}

// backtrack helper (1 loop)
void backtrack(
    vector<char>& word,
    const vector<int>& var_pos,
    size_t var_index,
    map<char, int> remaining_required,
    const set<string>& dict,
    set<string>& result)
{
    
    if (var_index == var_pos.size()) {
        if (remaining_required.empty()) {
            string candidate(word.begin(), word.end());
            if (dict.find(candidate) != dict.end()) { // Built-in find (not counted)
                result.insert(candidate);
            }
        }
        return;
    }

    // try all letters (1 loop)
    const int pos = var_pos[var_index]; 
    for (char c = 'a'; c <= 'z'; ++c) { //Loop 4/5
        map<char, int> new_req = remaining_required;
        auto it = new_req.find(c);
        
        
        if (it != new_req.end()) {
            if (it->second <= 0) continue;
            if (--(it->second) == 0) {
                new_req.erase(it);
            }
        }

        // recurse
        word[pos] = c;
        backtrack(word, var_pos, var_index + 1, new_req, dict, result);
    }
}