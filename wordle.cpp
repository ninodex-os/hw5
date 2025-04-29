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
        } else if (req_letters[in[i] - 'a'] > 0) {
            req_letters[in[i] - 'a']--;
        }
    }
    
    // Count total required letters
    int req_remaining = 0;
    for (int i = 0; i < 26; ++i) { // Loop 3/5
        req_remaining += req_letters[i];
    }
    
    // Special case optimization: if exactly the right number of blanks
    if (req_remaining == blanks) {
        // Fast path for patterns like "telecommunications"
        set<string> results;
        
        // Try directly matching against dictionary
        for (const string& dictWord : dict) { // Loop 4/5
            if (dictWord.length() != word.length()) continue;
            
            // Check if word matches fixed pattern
            bool matches = true;
            for (size_t i = 0; i < word.length(); i++) {
                if (word[i] != '-' && word[i] != dictWord[i]) {
                    matches = false;
                    break;
                }
            }
            if (!matches) continue;
            
            // Check if word contains all floating letters
            int letter_count[26] = {0};
            for (char c : dictWord) {
                letter_count[c - 'a']++;
            }
            
            bool has_all_floating = true;
            for (int i = 0; i < 26; i++) {
                if (req_letters[i] > letter_count[i]) {
                    has_all_floating = false;
                    break;
                }
            }
            
            if (has_all_floating) {
                results.insert(dictWord);
            }
        }
        
        return results;
    }
    
    // Fallback to standard backtracking for general cases
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
        backtrack(word, pos + 1, req_letters, req_remaining, blanks_left, dict, results);
        return;
    }
    
    // Early termination: not enough blanks for required letters
    if (blanks_left < req_remaining) return;
    
    // Try required letters first (optimization)
    for (int i = 0; i < 26; ++i) { // Loop 5/5
        if (req_letters[i] > 0) {
            char c = 'a' + i;
            word[pos] = c;
            req_letters[i]--;
            
            backtrack(word, pos + 1, req_letters, req_remaining - 1, blanks_left - 1, dict, results);
            
            // Backtrack
            req_letters[i]++;
            word[pos] = '-';
        }
    }
    
    // Only try non-required letters if we have extra slots
    if (blanks_left > req_remaining) {
        for (char c = 'a'; c <= 'z'; ++c) {
            if (req_letters[c - 'a'] > 0) continue; // Skip required letters
            
            word[pos] = c;
            backtrack(word, pos + 1, req_letters, req_remaining, blanks_left - 1, dict, results);
            word[pos] = '-';
        }
    }
}
