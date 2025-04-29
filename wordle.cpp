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
    int blanks_left,
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
    
    // Count floating letters
    for (char c : floating) { // Loop 1/5
        req_letters[c - 'a']++;
    }
    
    // Subtract fixed letters and count blanks
    int blanks = 0;
    for (size_t i = 0; i < in.size(); ++i) { // Loop 2/5
        if (in[i] == '-') {
            blanks++;
        } else {
            req_letters[in[i] - 'a'] = max(0, req_letters[in[i] - 'a'] - 1);
        }
    }
    
    // Count total required letters
    int req_remaining = 0;
    for (int i = 0; i < 26; ++i) { // Loop 3/5
        req_remaining += req_letters[i];
    }
    
    // Check if we have enough blank spaces
    if (req_remaining > blanks) {
        return {};
    }
    
    // Start backtracking
    set<string> results;
    backtrack(word, 0, req_letters, req_remaining, blanks, dict, results);
    
    return results;
}

void backtrack(
    string& word,
    size_t pos,
    int req_letters[26],
    int req_remaining,
    int blanks_left,
    const set<string>& dict,
    set<string>& results)
{
    // If we've processed the entire word
    if (pos == word.size()) {
        if (req_remaining == 0 && dict.find(word) != dict.end()) {
            results.insert(word);
        }
        return;
    }
    
    // If this position already has a letter
    if (word[pos] != '-') {
        backtrack(word, pos + 1, req_letters, req_remaining, blanks_left, dict, results);
        return;
    }
    
    // If we need to use all required letters in remaining blanks
    if (blanks_left == req_remaining) {
        for (int i = 0; i < 26; ++i) { // Loop 4/5
            if (req_letters[i] > 0) {
                char c = 'a' + i;
                word[pos] = c;
                
                req_letters[i]--;
                backtrack(word, pos + 1, req_letters, req_remaining - 1, blanks_left - 1, dict, results);
                req_letters[i]++;
                word[pos] = '-';
            }
        }
    } else {
        // Try required letters first
        for (int i = 0; i < 26; ++i) {
            if (req_letters[i] > 0) {
                char c = 'a' + i;
                word[pos] = c;
                
                req_letters[i]--;
                backtrack(word, pos + 1, req_letters, req_remaining - 1, blanks_left - 1, dict, results);
                req_letters[i]++;
                word[pos] = '-';
            }
        }
        
        // Try non-required letters
        for (char c = 'a'; c <= 'z'; ++c) { // Loop 5/5
            if (req_letters[c - 'a'] > 0) continue;
            
            word[pos] = c;
            backtrack(word, pos + 1, req_letters, req_remaining, blanks_left - 1, dict, results);
            word[pos] = '-';
        }
    }
}
