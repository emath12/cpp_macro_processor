// C++ Macro Processor 
#include <iostream>
#include <string>
#include "hashmap.cpp"

#define CAPACITY 50000
#define INVALID "@10NULL01@"
#define EMPTY ""

enum State {
    plaintext,
    reading_macro,
    reading_arg,
};

// /def{hello}{/def{obama}{44}}

int process(std::string input, State s);

#define MACRO '/'

std::string output_buffer;
hashmap * user_macros = new hashmap(CAPACITY);

string disperse_parameter(string input, string param) {

    int len = input.length();
    string new_input;
    
    for (int i = 0; i < len; i++) {
        if (input[i] == '#') {
            new_input += param; 
        } else {
            new_input += input[i];
        }
    }

    return new_input;
}

string evaluate_macro(string *macro, std::vector<string> *args) {
    if (*macro == "def") {
        // add the mapping to the table.
        user_macros->add_mapping((*args)[2], (*args)[1]);
    } else if (*macro == "undef") {
        // remove the mapping
        user_macros->remove_mapping((*args)[1]);
    } else if (*macro == "ifdef") {
        // if the macro is mapped, then process THEN plaintext, otherwise process ELSE as plaintext

        if (user_macros->is_mapped((*args)[3])) {
            return (*args)[2];
        } else {
            return (*args)[1];
        }   

    } else {
        // otherwise, it is a user_macro
        string user_macro_def = user_macros->get_mapping(*macro);
        if (user_macro_def != INVALID) {
            // the user_macro definiton is processed, with the parameter passed.
            string correct_mapping = disperse_parameter(user_macro_def, (*args)[1]);
            return correct_mapping;
        } else {
            output_buffer.clear();
            cout << "[Invalid Macro Call] \"" << (*macro) << "\" is not defined";
            return INVALID;
        }
    }

    return EMPTY;
}

int process(string input, State s) {
    string macro_sub_buffer;

    vector<string> args;
    int expected_num_of_args;
    string macro_expansion;

    // the string iself that is paused .
    string paused_str;
    int paused_str_index;

    // a pointer to the current string that we are processing. 
    string *current_str = &input;
 
    for (int i = 0; i < (*current_str).length(); i++) {
    
        char p_letter = (*current_str)[i];

        switch(s) {
            case plaintext:        
                if (p_letter == MACRO) {
                    s = reading_macro;
                } else {
                    if (!isalnum(p_letter) && !isspace(p_letter)) {
                        // legal, non-macro letters must be a space or alphanumeric
                        cout << "[Invalid Plaintext Character] : " << p_letter << " is not alphanumeric.";
                        output_buffer.clear();
                        return 1;
                    }
                    output_buffer += p_letter;
                }
                break;
            case reading_macro:
                if (p_letter == '{') {
                    // evaluluate which macro it is and set the correct number of args.
                    if (macro_sub_buffer == "def") {
                        expected_num_of_args = 2;
                    } else if (macro_sub_buffer == "undef") {
                        expected_num_of_args = 1;
                    } else if (macro_sub_buffer == "ifdef") {
                        expected_num_of_args = 3;
                    } else {
                        // user macro 
                        expected_num_of_args = 1;
                    }
                
                    if (args.empty()) {
                        args.resize(3);
                    }

                    if (expected_num_of_args > args.size() - 1) {
                        args.resize(args.size() * 2);
                    }

                    s = reading_arg;
                } else if (isspace(p_letter)) {
                    // we are done processing the macro, we ran into a space. Spaces of any sort are illegal in macros and denote its end.
                    
                    output_buffer += p_letter;

                    string evaluated_macro = evaluate_macro(&macro_sub_buffer, &args);

                    args[3].clear();
                    args[2].clear();
                    args[1].clear();
                    macro_sub_buffer.clear();

                    if (evaluated_macro == INVALID) { 
                        return 1;
                    } else if (evaluated_macro != EMPTY) {
                        paused_str = *current_str;
                        paused_str_index = i;
                        
                        current_str = &evaluated_macro;
                        i = -1;
                    }

                    s = plaintext;
                } else {
                    if (!isalnum(p_letter)) {
                        // macro characters must be alphanumeric 
                        cout << "[Invalid Macro Character] : " << p_letter << " is not alphanumeric.";
                        output_buffer.clear();
                        return 1;
                    }

                    macro_sub_buffer.push_back(p_letter);
                }

                break;
            case reading_arg:
            
                if (expected_num_of_args > 0) {

                    if (p_letter == '}') {
                        expected_num_of_args -= 1;
                    } else {
                        if ((((p_letter != '{') && isalnum(p_letter))) || (isspace(p_letter)) || (p_letter == '#') || (p_letter == MACRO)) {
                            // argument letters must be alphanumeric or a space.
                            // as we process multiple args, we will run into a '{', which we should not add
                            // to the macro def.
                            args[expected_num_of_args].push_back(p_letter);
                        } 
                    }

                } else {
                    // time to evaluate the macro if we reach here

                    // handling the letter itself we are processing.
                    if (isalnum(p_letter) || isspace(p_letter)) {
                        output_buffer += p_letter;
                    }

                    string evaluated_macro = evaluate_macro(&macro_sub_buffer, &args);

                    args[3].clear();
                    args[2].clear();
                    args[1].clear();
                    macro_sub_buffer.clear();

                    if (evaluated_macro == INVALID) { 
                        return 1;
                    } else if (evaluated_macro != EMPTY) {
                        paused_str = *current_str;
                        paused_str_index = i;
                        
                        current_str = &evaluated_macro;
                        i = -1;
                    }

                    s = plaintext;
                }

                break;
            default:
                break;
        }

        // returning to the string we paused initially to evaluate.
        if (i == ((*current_str).length() - 1)) {
            if (!paused_str.empty()) {
                (*current_str) = paused_str;
                paused_str.clear();
                i = paused_str_index - 1;
            }
        }
    }

    if (s != plaintext) {
        // if we are still in the reading_macro state when we exit, we evaluate the macro since the macro
        // call was up to E0F. 

        // Thus, we should process what there is. 

        string evaluated_macro = evaluate_macro(&macro_sub_buffer, &args);

        args[3].clear();
        args[2].clear();
        args[1].clear();
        macro_sub_buffer.clear();

        if (evaluated_macro == INVALID) {
            return 1;
        } else {
            process(evaluated_macro, plaintext);        
        }
    } 

    return 0;
}

int main() {
    // obtaining user input
    string input = "/def{obama}{44} /ifdef{obama}{cow}{bee} /obama";
    // getline(std::cin, input);
    process(input, plaintext);
    cout << output_buffer;
    
    return 0;
}