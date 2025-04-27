#ifndef RECCHECK
// For debugging
#include <iostream>
#include <vector>
#include <map>
#include <set>
using namespace std;
#endif
#include "wordle.h"
#include "dict-eng.h"


//helper
void backtrack(vector<char>& word, const vector<int>& var_pos, unsigned int var_index, map<char, int> remaining_required, set<string>& candidates);

// Definition of primary wordle function
set<string> wordle(
    const string& in,
    const string& floating,
    const set<string>& dict)
{  // Add your code here
    //get variable positions 
    vector<int> var_pos;
    vector<char> word;
    map<char, int> fixed_counts;
    for (unsigned int i = 0; i < in.size(); ++i) {
        if (in[i] == '-') {
            var_pos.push_back(i);
            word.push_back('-');
        } else {
            word.push_back(in[i]);
            fixed_counts[in[i]]++;
        }
    }

    // compute floating counts.
    map<char, int> floating_counts;
    for (char c : floating) {
        floating_counts[c]++;
    }

    // compute required counts.
    map<char, int> required_counts;
    for (auto& pair : floating_counts) {
        char c = pair.first;
        int required = pair.second - fixed_counts[c];
        if (required > 0) {
            required_counts[c] = required;
        }
    }

    // check sum of required counts
    unsigned int sum_required = 0;
    for (auto& pair : required_counts) {
        sum_required += pair.second;
    }

    if (sum_required > var_pos.size()) {
        return {};
    }

    // candidates
    set<string> candidates;
    backtrack(word, var_pos, 0, required_counts, candidates);

    // filter candidates 
    set<string> result;
    for (const string& s : candidates) {
        if (dict.find(s) != dict.end()) {
            result.insert(s);
        }
    }

    return result;
}

// helper function 
void backtrack(vector<char>& word, const vector<int>& var_pos, unsigned int var_index, map<char, int> remaining_required, set<string>& candidates) {
    if (var_index == var_pos.size()) {
        if (remaining_required.empty()) {
            string s(word.begin(), word.end());
            candidates.insert(s);
        }
        return;
    }

    int pos = var_pos[var_index];
    for (char c = 'a'; c <= 'z'; ++c) {
        map<char, int> new_remaining = remaining_required;
        auto it = new_remaining.find(c);
        if (it != new_remaining.end()) {
            if (it->second > 0) {
                it->second--;
                if (it->second == 0) {
                    new_remaining.erase(it);
                }
            }
        }
        word[pos] = c;
        backtrack(word, var_pos, var_index + 1, new_remaining, candidates);
    }
}