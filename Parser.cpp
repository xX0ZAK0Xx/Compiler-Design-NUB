#include <bits/stdc++.h>
using namespace std;

string removeSpace(const string& str) {
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

vector<string> breakProduction(const string& str, char spliter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, spliter)) {
        tokens.push_back(token);
    }
    return tokens;
}
void calculateFollow(const string& symbol, const map<string, vector<vector<string>>>& productionRules, const map<string, set<string>>& firstSets, map<string, set<string>>& followSets, set<string>& visited) {
    visited.insert(symbol);
    // Added the symbol in visited, cz it is now being calculated

    for (const auto& rule : productionRules) {
        //iterating through each production rule
        // { "E": [ T E'], "E'": [+ T E' | #], ....... }
        // rule = "E": [ T E'], then "E'": [+ T E' | #]
        const string& leftSymbol = rule.first;
        //taken the symbol
        // leftSymbol = E
        for (const vector<string>& production : rule.second) {
            // iterating through each alternates
            // T E' , 
            // production = T E', then production = another rule after | symbol
            for (size_t i = 0; i < production.size(); ++i) {
                // to iterate through each character of production. 
                //currently production.size() = 2, because production = T, E'
                if (production[i] == symbol) {
                    // To check if the any character of the production is the non terminal symbol we are calculating for
                    if (i < production.size() - 1) {
                        // as production.size() -1 = 2 -1 = 1, and i = 0, 
                        // so it checks if the current character is following any other character, if yes then:
                        const string& nextSymbol = production[i + 1]; // taken the next character
                        // nextSymbol = E'
                        if (productionRules.find(nextSymbol) == productionRules.end()) { // then it is a terminal
                            followSets[symbol].insert(nextSymbol); // so add the terminal into te followSet of that symbol
                        } else {// if it is non terminal
                            set<string> firstOfNext = firstSets.at(nextSymbol);
                            //We take the first set of the nextSymbol
                            followSets[symbol].insert(firstOfNext.begin(), firstOfNext.end());
                            //then insert it into to followSet of that symbol
                            followSets[symbol].erase("#"); // Remove epsilon from the follow set as it doesnt contain epsilon
                            if (firstOfNext.find("#") != firstOfNext.end()) {
                                // now if the firstSet contains epsilon for the next character, then I have to calculate the followSet again for the nextSymbol
                                if (visited.find(nextSymbol) == visited.end()) {
                                    //if the next symbol was not visited yet
                                    // or I can say, if the follow for the nextSymbol was not calculated yet,
                                    // then call for it
                                    calculateFollow(nextSymbol, productionRules, firstSets, followSets, visited);
                                }
                                set<string> followOfNext = followSets.at(nextSymbol);
                                // then add the follow set of the nextSymbol into the follow set of the current symbol
                                followSets[symbol].insert(followOfNext.begin(), followOfNext.end());
                            }
                        }
                    } else { // the symbol is calling the parent
                        if (leftSymbol != symbol && visited.find(leftSymbol) == visited.end()) {
                            //if leftSymbol is not the same as the current symbol and if it hasn't been visited before (to avoid infinite recursion)
                            calculateFollow(leftSymbol, productionRules, firstSets, followSets, visited);
                        }
                        if (leftSymbol != symbol && productionRules.find(leftSymbol) != productionRules.end()) {
                        // if leftSymbol is not the same as the current symbol and if leftSymbol is also a non-terminal
                            set<string> followOfLeft = followSets.at(leftSymbol);
                            //then take the follow of that parent/leftSymbol
                            followSets[symbol].insert(followOfLeft.begin(), followOfLeft.end());
                        }
                    }
                }
            }
        }
    }
    visited.erase(symbol);
}

// Function to calculate first set for a non-terminal symbol
set<string> calculateFirstFunction(const string& symbol, const map<string, vector<vector<string>>>& productionRules, map<string, set<string>>& firstSets) {
    // Check if first set is already calculated
    if (firstSets.find(symbol) != firstSets.end()) {
        //if not found then .find() returns the iterator after the last element as .end() 
        return firstSets[symbol];
    }
    
    set<string> firstSet;
    
    // For each production rule of the symbol
    for (const vector<string>& production : productionRules.at(symbol)) {
        //["A", "C", "B"], ["C", "b", "b"], ["B", "a"]
        for (const string& term : production) {
            //["A", "C", "B"]
            // If terminal symbol, add to FIRST set
            if (productionRules.find(term) == productionRules.end()) {
                //here .find() finds symbol in production rules, if not found then its a terminal
                firstSet.insert(term);
                break;
            }
            // If non-terminal symbol, it'll calculate its first set recursively
            else {
                set<string> termFirst = calculateFirstFunction(term, productionRules, firstSets);
                firstSet.insert(termFirst.begin(), termFirst.end());
                // If epsilon is not in FIRST(term), stop and don't add epsilon
                if (termFirst.find("#") == termFirst.end()) {
                    break;
                }//otherwise add terminals for the next symbol in current production
            }
        }
    }
    
    firstSets[symbol] = firstSet;
    return firstSet;
}

int main() {
    int numRules;
    cout << "Enter the number of grammar rules: ";
    cin >> numRules;
    cin.ignore();
    
    map<string, vector<vector<string>>> productionRules;
    
    for (int i = 0; i < numRules; ++i) {
        string rule;
        cout << "Enter grammar rule " << i + 1 << ": ";
        getline(cin, rule);
        // S -> A C B | C b b | B a
        size_t arrowPos = rule.find("->"); 
        string symbol = removeSpace(rule.substr(0, arrowPos)); // extracting the symbol
        string productionPart = removeSpace(rule.substr(arrowPos + 2)); // extracting the production as after arrow till end
        vector<string> alternatives = breakProduction(productionPart, '|'); // splitted the productions as:
        // {A C B | C b b | B a} : A C B , C b b , B a

        for (const string& alt : alternatives) { // iterating over all alternative productions
            vector<string> parts;
            stringstream ss(removeSpace(alt));//A C B
            string part;
            while (ss >> part) {//taking each space separated terminals/nonterminals
                parts.push_back(part);
                //firstly part = A,
                //then    part = A,C
                //then    part = A,C,B 
            }
            productionRules[symbol].push_back(parts);
            /*
            {
                "S": [
                    ["A", "C", "B"],
                    ["C", "b", "b"],
                    ["B", "a"]
                ],
                .....
                .....
            }
            */
        }
    }
    
    map<string, set<string>> firstSets;// A map where firstsets will be stored

    for (const auto& rule : productionRules) {
        calculateFirstFunction(rule.first, productionRules, firstSets);
        //              symbol,        rules,          output
    }
    
    // Print the calculated FIRST sets
    for (const auto& entry : firstSets) {
        cout << "FIRST(" << entry.first << ") = { ";
        bool firstTerm = true;
        for (const string& term : entry.second) {
            if (!firstTerm) {
                cout << ", ";
            }
            cout << term;
            firstTerm = false;
        }
        cout << " }\n";
    }
    cout<<endl;
    
    map<string, set<string>> followSets;
    followSets["S"].insert("$");
    //inserting $ sign for the first rule
    set<string> visited;
    // a set to track the visited symbol

    for (const auto& rule : productionRules) {
        calculateFollow(rule.first, productionRules, firstSets, followSets, visited);
        //              symbol,     rules,            firstSet, output,     track visited symbol           
    }

    // Print the calculated FOLLOW sets
    for (const auto& entry : followSets) {
        cout << "FOLLOW(" << entry.first << ") = { ";
        bool firstTerm = true;
        for (const string& term : entry.second) {
            if (!firstTerm) {
                cout << ", ";
            }
            cout << term;
            firstTerm = false;
        }
        cout << " }\n";
    }

    return 0;
}

/*
E  -> T E'
E' -> + T E' | #
T  -> F T'
T' -> * F T' | #
F  -> ( E ) | id

FIRST(E) = { ( , id }
FIRST(E’) = { +, # }
FIRST(T) = { ( , id }
FIRST(T’) = { *, # }
FIRST(F) = { ( , id }

FOLLOW(E)  = { $ , ) }
FOLLOW(E’) = {  $, ) }
FOLLOW(T)  = { + , $ , ) }
FOLLOW(T’) = { + , $ , ) }
FOLLOW(F)  = { *, +, $, ) }
--------------------------------------

S -> a B D h
B -> c C
C -> b C | #
D -> E F
E -> g | #
F -> f | #

FIRST set
FIRST(S) = { a }
FIRST(B) = { c }
FIRST(C) = { b , # }
FIRST(D) = { g, f, # }
FIRST(E) = { g , # }
FIRST(F) = { f , # }

FOLLOW(S) = { $ } 
FOLLOW(B) = { g , f , h }
FOLLOW(C) = { g , f , h }
FOLLOW(D) = { h }
FOLLOW(E) = { f , h }
FOLLOW(F) = { h } 

---------------------------------------

S -> A C B | C b b | B a
A -> d a | B C
B -> g | #
C -> h | #

FIRST(S) = { d, g, h, #, b, a}
FIRST(A) = { d, g, h, # }
FIRST(B) = { g, # }
FIRST(C) = { h, # }

FOLLOW(S) = { $ }
FOLLOW(A) = { h, g, $ }
FOLLOW(B) = { a, $, h, g }
FOLLOW(C) = { b, g, $, h }

*/