// C++ Macro Processor 
#include <iostream>
#include <string>
#include "hashmap.cpp"

#define CAPACITY 50000
#define INVALID "@10NULL01@"

enum State {
    plaintext,
    reading_macro,
    reading_arg,
};

int process(std::string input, State s, std::string macro_arg);

#define MACRO '/'

std::string output_buffer;
hashmap * user_macros = new hashmap(CAPACITY);

int evaluate_macro(string *macro, std::vector<string> *args) {
    if (*macro == "def") {
        // add the mapping to the table.
        user_macros->add_mapping((*args)[2], (*args)[1]);
        (*args)[2].clear();
        (*args)[1].clear();
    } else if (*macro == "undef") {
        // remove the mapping
        user_macros->remove_mapping((*args)[1]);
        (*args)[1].clear();
    } else if (*macro == "ifdef") {
        // if the macro is mapped, then process THEN plaintext, otherwise process ELSE as plaintext
        if (user_macros->is_mapped((*args)[3])) {
            process((*args)[2], plaintext, "");
        } else {
            process((*args)[1], plaintext, "");
        }
    } else if (*macro == "expandafter") {
        process(((*args)[2]), plaintext, "");
        process(((*args)[1]), plaintext, "");
    } else {
        // otherwise, it is a user_macro
        string user_macro_def = user_macros->get_mapping(*macro);
        if (user_macro_def != INVALID) {
            // the user_macro definiton is processed, with the parameter passed.
            process(user_macro_def, plaintext, (*args)[1]);
            (*args)[1].clear();
        } else {
            output_buffer.clear();
            cout << "[Invalid Macro Call] \"" << (*macro) << "\" is not defined";
            return 1;
        }
    }

    (*macro).clear();
    return 0;
}

int process(std::string input, State s, std::string macro_arg) {
    int input_length = input.length();

    std::string macro_sub_buffer;

    std:vector<string> args;
    int expected_num_of_args;
    
    for (int i = 0; i < input_length; i++) {
        char p_letter = input[i];
        switch(s) {
            case plaintext:        
                if (p_letter == MACRO) {
                    s = reading_macro;
                } else if (p_letter == '#' && !macro_arg.empty()) {
                    process(macro_arg, plaintext, "");
                } else {
                    if (!isalnum(p_letter) && !isspace(p_letter)) {
                        // legal, non-macro letters must be a space or alphanumeric
                        std::cout << "[Invalid Plaintext Character] : " << p_letter << " is not alphanumeric.";
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
                    if (evaluate_macro(&macro_sub_buffer, &args) != 0) {
                        return 1;
                    }
                    output_buffer += p_letter;
                    s = plaintext;
                } else {
                    if (!isalnum(p_letter)) {
                        // macro characters must be alphanumeric 
                        std::cout << "[Invalid Macro Character] : " << p_letter << " is not alphanumeric.";
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
                        if ((((p_letter != '{') && isalnum(p_letter))) || (isspace(p_letter)) || (p_letter == '#')) {
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

                    if (evaluate_macro(&macro_sub_buffer, &args) != 0) {
                        return 1;
                    }
                    s = plaintext;
                }

                break;
            default:
                break;
        }
    }

    if (s != plaintext) {
        // if we are still in the reading_macro state when we exit, we evaluate the macro since the macro
        // call was up to E0F.
        if (evaluate_macro(&macro_sub_buffer, &args) != 0) {
            return 1;
        }
    } 

    return 0;
}

int main() {
    // obtaining user input
    std::string input;
    std::getline(std::cin, input);
    process(input, plaintext, "");
    std::cout << output_buffer;
    
    return 0;
}