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
    string& word,
    size_t pos,
    int req_letters[26],
    int req_remaining,
    const set<string>& dict,
    set<string>& results
);

set<string> wordle(
    const string& in,
    const string& floating,
    const set<string>& dict)
{
    // prrocess input to find variable positions (1 loop)
    string word = in;
    int req_letters[26] = {0}; // Count of required letters a-z
    
    // Count floating letters (Loop 1/4)
    for (char c : floating) {
        req_letters[c - 'a']++;
    }
    
    // Subtract fixed letters from required and count blanks (Loop 2/4)
    int blanks = 0;
    for (size_t i = 0; i < in.size(); ++i) {
        if (in[i] == '-') {
            blanks++;
        } else if (req_letters[in[i] - 'a'] > 0) {
            req_letters[in[i] - 'a']--;
        }
    }
    
    // Count total required letters (Loop 3/4)
    int req_remaining = 0;
    for (int i = 0; i < 26; ++i) {
        req_remaining += req_letters[i];
    }
    
    // Check if we have enough blank spaces
    if (req_remaining > blanks) {
        return {};
    }
    
    // Start backtracking
    set<string> results;
    backtrack(word, 0, req_letters, req_remaining, dict, results);
    
    return results;
}

void backtrack(
    string& word,
    size_t pos,
    int req_letters[26],
    int req_remaining,
    const set<string>& dict,
    set<string>& results
) {
    // Base case: full word constructed
    if (pos == word.size()) {
        // Only check dictionary if all required letters used
        if (req_remaining == 0 && dict.find(word) != dict.end()) {
            results.insert(word);
        }
        return;
    }
    
    // Skip fixed positions
    if (word[pos] != '-') {
        backtrack(word, pos + 1, req_letters, req_remaining, dict, results);
        return;
    }
    
    // Calculate remaining blank positions
    int blanks_left = 0;
    for (size_t i = pos; i < word.size(); i++) { // Loop 4/4
        if (word[i] == '-') blanks_left++;
    }
    
    // Early termination: not enough blanks for required letters
    if (blanks_left < req_remaining) return;
    
    // Try all letters at this position
    for (char c = 'a'; c <= 'z'; c++) {
        // If this is a required letter, use it
        if (req_letters[c - 'a'] > 0) {
            word[pos] = c;
            req_letters[c - 'a']--;
            
            backtrack(word, pos + 1, req_letters, req_remaining - 1, dict, results);
            
            // Backtrack
            req_letters[c - 'a']++;
            word[pos] = '-';
        }
        // If we have extra slots, we can use non-required letters
        else if (blanks_left > req_remaining) {
            word[pos] = c;
            backtrack(word, pos + 1, req_letters, req_remaining, dict, results);
            word[pos] = '-';
        }
    }
}
