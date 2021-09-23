#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

map <string, string> variables;

string open_file(string filename) {
    string code = "";
    string codeline = "";
    ifstream codefile(filename);

    if (filename.substr(filename.find_last_of(".") + 1) == "aenc") {
        while (getline(codefile, codeline)) {
            code = code + codeline + "\n";
        }

        code = code + "<EOF>";
    }

    return code;
}

vector <string> condition_lexer(string condition) {
    vector <string> tokens;
    string token = "";
    string var_name = "";
    string str = "";
    int variable_state = 0;
    int string_state = 0;

    condition = condition + "<EOF>";

    for(char& chr : condition) {
        token = token + chr;

        if (token == "=" || token == "<" || token == ">" || token == "!") {
            if (var_name != "") {
                tokens.push_back(var_name);
                variable_state = 0;
                var_name = "";
            }
        }

        if (token == " " || token == "<EOF>") {
            token = "";
        }

        else if (token == "TRUE") {
            tokens.push_back("TRUE");
            token = "";
        }

        else if (token == "FALSE") {
            tokens.push_back("FALSE");
            token = "";
        }

        else if (token == "==") {
            tokens.push_back("EQEQ");
            token = "";
        }

        else if (token == "!=") {
            tokens.push_back("UNEQ");
            token = "";
        }

        else if (token == "VAR") {
            tokens.push_back("VAR");
            variable_state = 1;
            token = "";
        }

        else if (variable_state == 1) {
            var_name = var_name + token;
            token = "";
        }

        else if (token == "\"" || token == " \"") {
            token = "";

            if (string_state == 0) {
                string_state = 1;
            }

            else if (string_state == 1) {
                tokens.push_back(str);
                string_state = 0;
                str = "";
            }
        }

        else if (string_state == 1) {
            str = str + token;
            token = "";
        }
    }

    return tokens;
}

bool condition_parser(vector <string> tokens) {
    if (tokens.size() == 1) {
        if (tokens[0] == "TRUE") {
            return true;
        }

        else if (tokens[0] == "FALSE") {
            return false;
        }
    }

    else if (tokens.size() == 2) {
        if (tokens[0] == "VAR") {
            if (variables[tokens[1]] == "TRUE") {
                return true;
            }

            if (variables[tokens[1]] == "FALSE") {
                return false;
            }
        }
    }

    else if (tokens.size() == 3) {
        if (tokens[1] == "EQEQ") {
            if (tokens[0] == tokens[2]) {
                return true;
            }

            else {
                return false;
            }
        }

        if (tokens[1] == "UNEQ") {
            if (tokens[0] != tokens[2]) {
                return true;
            }

            else {
                return false;
            }
        }
    }

    else if (tokens.size() == 4) {
        if (tokens[0] == "VAR") {
            if (tokens[2] == "EQEQ") {
                if (variables[tokens[1]] == tokens[3]) {
                    return true;
                }

                else {
                    return false;
                }
            }

            else if (tokens[2] == "UNEQ") {
                if (variables[tokens[1]] != tokens[3]) {
                    return true;
                }

                else {
                    return false;
                }
            }
        }

        else if (tokens [2] == "VAR") {
            if (tokens[1] == "EQEQ") {
                if (tokens[0] == variables[tokens[3]]) {
                    return true;
                }

                else {
                    return false;
                }
            }

            else if (tokens[1] == "UNEQ") {
                if (tokens[0] != variables[tokens[3]]) {
                    return true;
                }

                else {
                    return false;
                }
            }
        }

        else {
            return false;
        }
    }

    else if (tokens.size() == 5) {
        if (tokens[2] == "EQEQ") {
            if (variables[tokens[1]] == variables[tokens[4]]) {
                return true;
            }

            else {
                return false;
            }
        }

        else if (tokens[2] == "UNEQ") {
            if (variables[tokens[1]] != variables[tokens[4]]) {
                return true;
            }

            else {
                return false;
            }
        }
    }

    return false;
}

vector <string> lexer(string code) {
    vector <string> tokens;
    string token = "";
    string str = "";
    string var_name = "";
    string task = "";
    string condition = "";
    int ignore = 0;
    int string_state = 0;
    int task_state = 0;
    int condition_state = 0;
    int variable_state = 0;

    for(char& chr : code) {
        token = token + chr;

        if (token == "]") {
            if (ignore >= 1) {
                task = task + "]";
                ignore = ignore - 1;
            }

            else {
                tokens.push_back(task);
                task_state = 0;
                task = "";
            }

            token = "";
        }

        else if (token == "[") {
            if (task_state == 1) {
                task = task + token;
                ignore = ignore + 1;
            }

            else {
                task_state = 1;
            }

            token = "";
        }

        else if (task_state == 1) {
            task = task + token;
            token = "";
        }

        else if (token == ")") {
            tokens.push_back(condition);
            condition_state = 0;
            condition = "";
            token = "";
        }

        else if (condition_state == 1) {
            condition = condition + token;
            token = "";
        }

        else if (token == "(") {
            condition_state = 1;
            token = "";
        }

        else if (token == " ") {
            if (string_state == 0) {
                token = "";
            }
        }

        else if (token == "\n" || token == "<EOF>") {
            if (var_name != "") {
                tokens.push_back(var_name);
                variable_state = 0;
                var_name = "";
            }

            token = "";
        }

        else if (token == "TRUE") {
            tokens.push_back("TRUE");
            token = "";
        }

        else if (token == "FALSE") {
            tokens.push_back("FALSE");
            token = "";
        }

        else if (token == "PRINT") {
            tokens.push_back("PRINT");
            token = "";
        }

        else if (token == "INPUT") {
            tokens.push_back("INPUT");
            token = "";
        }

        else if (token == "SYSTEM") {
            tokens.push_back("SYSTEM");
            token = "";
        }

        else if (token == "VAR" && string_state == 0) {
            tokens.push_back("VAR");
            token = "";
            variable_state = 1;
        }

        else if (token == "WHILE") {
            tokens.push_back("WHILE");
            token = "";
        }

        else if (token == "IF") {
            tokens.push_back("IF");
            token = "";
        }

        else if (token == "=" && string_state == 0) {
            if (var_name != "") {
                tokens.push_back(var_name);
                variable_state = 0;
                var_name = "";
            }

            token = "";
        }

        else if (variable_state == 1) {
            var_name = var_name + token;
            token = "";
        }

        else if (token == "\"" || token == " \"") {
            token = "";

            if (string_state == 0) {
                string_state = 1;
            }

            else if (string_state == 1) {
                tokens.push_back(str);
                string_state = 0;
                str = "";
            }
        }

        else if (string_state == 1) {
            str = str + token;
            token = "";
        }
    }

    return tokens;
}

void parser(vector <string> tokens) {
    vector <string> tokens_2;
    vector <string> condition_tokens;
    bool condition;
    int pos = 0;

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        /*
         * cout << tokens[pos] << endl;
         *
         * pos = pos + 1;
         */

        else if (tokens[pos] == "PRINT") {
            if (tokens[pos + 1] == "VAR") {
                cout << variables[tokens[pos + 2]] << endl;
                pos = pos + 3;
            }

            else {
                cout << tokens[pos + 1] << endl;
                pos = pos + 2;
            }
        }

        else if (tokens[pos] == "INPUT") {
            string input;

            cout << tokens[pos + 1] << " ";
            getline(cin >> ws, input);

            if (tokens[pos + 2] == "VAR") {
                variables[tokens[pos + 3]] = input;
                pos = pos + 4;
            }
        }

        else if (tokens[pos] == "SYSTEM") {
            if (tokens[pos + 1] == "VAR") {
                cout << variables[tokens[pos + 2]] << endl;
                system(variables[tokens[pos + 2]].c_str());
                pos = pos + 3;
            }

            else {
                system(tokens[pos + 1].c_str());
                pos = pos + 2;
            }
        }

        else if (tokens[pos] == "WHILE") {
            condition_tokens.clear();
            condition = 0;

            condition_tokens = condition_lexer(tokens[pos + 1]);
            condition = condition_parser(condition_tokens);

            if (condition == true) {
                tokens_2 = lexer(tokens[pos + 2]);
                parser(tokens_2);
            }

            else {
                pos = pos + 3;
            }
        }

        else if (tokens[pos] == "IF") {
            condition_tokens.clear();
            condition = 0;

            condition_tokens = condition_lexer(tokens[pos + 1]);
            condition = condition_parser(condition_tokens);

            if (condition == true) {
                tokens_2 = lexer(tokens[pos + 2]);
                parser(tokens_2);
            }

            pos = pos + 3;
        }

        else if (tokens[pos] == "VAR") {
            variables[tokens[pos + 1]] = tokens[pos + 2];

            pos = pos + 3;
        }
    }
}

int main() {
    string filename;
    cout << "> ";
    cin >> filename;
    system("cls");

    string code;
    vector <string> tokens;

    code = open_file(filename);
    tokens = lexer(code);
    parser(tokens);
}