#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <limits.h>

using namespace std;

map <string, string> variables;
map <string, string> chached_variables;
map <string, string> functions;
map <string, string> function_arguments;

vector <string> pre_func;

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

string execute(const string& command) {
    system((command + " > temp.aenc").c_str());
 
    ifstream ifs("temp.aenc");
    string ret{ istreambuf_iterator<char>(ifs), istreambuf_iterator<char>() };
    ifs.close();

    if (remove("temp.aenc") != 0) {
        cout << "Error while deleting temporary file." <<  endl;
    }

    return ret;
}

bool is_file_exists(const std::string& filename) {
    ifstream ifile;
    ifile.open(filename);

    if(ifile) {
        return true;
    }

    return false;
}

bool is_dir_exists(const char *path) {
    struct stat info;

    if(stat(path, &info) != 0) {
        return false;
    }

    else if(info.st_mode & S_IFDIR) {
        return true;
    }

    else {
        return false;
    }
}

string get_directory(string path) {
    size_t found = path.find_last_of("/\\");
    
    return (path.substr(0, found));
}

string get_raw_name(string fullname) {
    size_t lastindex = fullname.find_last_of("."); 

    return fullname.substr(0, lastindex); 
}

string get_name(string path) {
    return path.substr(path.find_last_of("/\\") + 1);
}

string lib_reader_code(string str) {
    string code;

    if (is_dir_exists(str.c_str())) {
        code = open_file(str + "\\init.aenc");
    }

    else {
        if (is_file_exists(str)) {
            code = open_file(str);
        }
        
        else {
            if (is_dir_exists(("C:\\AeNC\\include\\" + str).c_str())) {
                code = open_file("C:\\AeNC\\include\\" + str + "\\init.aenc");
            }

            else {
                if (is_file_exists("C:\\AeNC\\include\\" + str)) {
                    code = open_file("C:\\AeNC\\include\\" + str);
                }
            }
        }
    }

    return code;
}

string lib_reader_lib_name(string str) {
    string lib_name;

    if (is_dir_exists(str.c_str())) {
        lib_name = get_raw_name(get_name(str));
    }

    else {
        if (is_file_exists(str)) {
            lib_name = get_raw_name(get_name(str));
        }
        
        else {
            if (is_dir_exists(("C:\\AeNC\\include\\" + str).c_str())) {
                lib_name = get_raw_name(get_name(str));
            }

            else {
                if (is_file_exists("C:\\AeNC\\include\\" + str)) {
                    lib_name = get_raw_name(get_name(str));
                }
            }
        }
    }

    return lib_name;
}

vector <string> expression_lexer(string expression) {
    vector <string> tokens;
    string token = "";
    string var_name = "";
    string str = "";
    bool variable_state = false;
    bool string_state = false;

    expression = expression + "<EOF>";

    for(char& chr : expression) {
        token = token + chr;

        if (token == " ") {
            if (string_state == true) {
                str = str + " ";
            }
        }

        if (token == " " || token == "<EOF>") {
            if (var_name != "") {
                tokens.push_back(var_name);
                variable_state = false;
                var_name = "";
            }

            token = "";
        }

        else if (token == "\"" || token == " \"" || token == "\\\"") {
            if (token == " \"") {
                str = str + " ";
            }

            token = "";

            if (string_state == false) {
                string_state = true;
            }

            else if (string_state == true) {
                tokens.push_back(str);
                string_state = false;
                str = "";
            }
        }

        else if (string_state == true) {
            if (token == "\\") {

            }

            else if (token == "\\q") {
                str = str + "\"";
                token = "";
            }

            else if (token == "\\\\") {
                str = str + "\\";
                token = "";
            }

            else {
                str = str + token;
                token = "";
            }
        }

        else if (token == "<") {
            if (var_name != "") {
                tokens.push_back(var_name);
                variable_state = false;
                var_name = "";
            }
        }

        else if (token == "+") {
            if (var_name != "") {
                tokens.push_back(var_name);
                variable_state = false;
                var_name = "";
            }

            tokens.push_back("PLUS");
            token = "";
        }

        else if (token == "var") {
            tokens.push_back("VAR");
            variable_state = true;
            token = "";
        }

        else if (variable_state == true) {
            var_name = var_name + token;
            token = "";
        }
    }

    return tokens;
}

string expression_parser(vector <string> tokens) {
    vector <string> new_tokens;
    string result;
    int pos = 0;

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        if (tokens[pos] == "VAR") {
            new_tokens.push_back(variables[tokens[pos + 1]]);
            pos = pos + 2;
        }

        else {
            new_tokens.push_back(tokens[pos]);
            pos = pos + 1;
        }
    }

    pos = 0;
    tokens = new_tokens;

    result = tokens[pos];

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        if (tokens[pos] == "PLUS") {
            result = result + tokens[pos + 1];
            pos = pos + 2;
        }

        else {
            pos = pos + 1;
        }
    }

    return result;
}

vector <string> condition_lexer(string condition) {
    vector <string> tokens;
    string token = "";
    string var_name = "";
    string str = "";
    bool variable_state = false;
    bool string_state = false;

    condition = condition + "<EOF>";

    for(char& chr : condition) {
        token = token + chr;

        if (token == " " || token == "<" || token == "!" || token == "=") {
            if (var_name != "") {
                tokens.push_back(var_name);
                variable_state = false;
                var_name = "";
            }
        }

        if (token == " " || token == "<EOF>") {
            if (string_state == false) {
                token = "";
            }
        }

        else if (variable_state == true) {
            var_name = var_name + token;
            token = "";
        }

        else if (token == "\"" || token == " \"" || token == "\\\"") {
            if (token == " \"") {
                str = str + " ";
            }

            token = "";

            if (string_state == false) {
                string_state = true;
            }

            else if (string_state == true) {
                tokens.push_back(str);
                string_state = false;
                str = "";
            }
        }

        else if (string_state == true) {
            if (token == "\\") {

            }

            else if (token == "\\q") {
                str = str + "\"";
                token = "";
            }

            else if (token == "\\\\") {
                str = str + "\\";
                token = "";
            }

            else {
                str = str + token;
                token = "";
            }
        }

        else if (token == "true") {
            tokens.push_back("TRUE");
            token = "";
        }

        else if (token == "false") {
            tokens.push_back("FALSE");
            token = "";
        }

        else if (token == "in") {
            tokens.push_back("IN");
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

        else if (token == "var") {
            tokens.push_back("VAR");
            variable_state = true;
            token = "";
        }

        else if (token == "and") {
            tokens.push_back("AND");
            token = "";
        }

        else if (token == "or") {
            tokens.push_back("OR");
            token = "";
        }
    }

    return tokens;
}

bool condition_parser(vector <string> tokens) {
    vector <string> new_tokens;
    int pos = 0;

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        if (tokens[pos] == "VAR") {
            new_tokens.push_back(variables[tokens[pos + 1]]);
            pos = pos + 2;
        }

        else {
            new_tokens.push_back(tokens[pos]);
            pos = pos + 1;
        }
    }

    pos = 0;
    tokens = new_tokens;
    new_tokens.clear();

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        if (tokens[pos] == "NOT") {
            if (tokens[pos + 1] == "TRUE") {
                new_tokens.push_back("FALSE");
            }

            else if (tokens[pos + 1] == "FALSE") {
                new_tokens.push_back("TRUE");
            }

            else {
                cout << "Error." << endl;
            }
            
            pos = pos + 2;
        }

        else {
            new_tokens.push_back(tokens[pos]);
            pos = pos + 1;
        }
    }

    pos = 0;
    tokens = new_tokens;
    new_tokens.clear();

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        else if (tokens[pos] == "EQEQ") {
            if (tokens[pos - 1] == tokens[pos + 1]) {
                new_tokens.push_back("TRUE");
            }

            else {
                new_tokens.push_back("FALSE");
            }

            pos = pos + 2;
        }

        else if (tokens[pos] == "UNEQ") {
            if (tokens[pos - 1] != tokens[pos + 1]) {
                new_tokens.push_back("TRUE");
            }

            else {
                new_tokens.push_back("FALSE");
            }

            pos = pos + 2;
        }
        
        else if (tokens[pos] == "AND") {
            new_tokens.push_back("AND");
            pos = pos + 1;
        }

        else if (tokens[pos] == "OR") {
            new_tokens.push_back("OR");
            pos = pos + 1;
        }

        else if (tokens[pos] == "TRUE" && tokens[pos + 1] != "EQEQ" && tokens[pos + 1] != "UNEQ") {
            new_tokens.push_back("TRUE");
            pos = pos + 1;
        }

        else if (tokens[pos] == "FALSE" && tokens[pos + 1] != "EQEQ" && tokens[pos + 1] != "UNEQ") {
            new_tokens.push_back("FALSE");
            pos = pos + 1;
        }

        else {
            pos = pos + 1;
        }
    }

    pos = 0;
    tokens = new_tokens;
    new_tokens.clear();

    while (true) {
        if (tokens.size() <= pos) {
            break;
        }

        else if (tokens[pos] == "OR") {
            if (tokens[pos - 1] == "TRUE" || tokens[pos + 1] == "TRUE") {
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 0);
                tokens.insert(tokens.begin() + 0, {"TRUE"});
            }

            else {
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 0);
                tokens.insert(tokens.begin() + 0, {"FALSE"});
            }
        }

        else if (tokens[pos] == "AND") {
            if (tokens[pos - 1] == "TRUE" && tokens[pos + 1] == "TRUE") {
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 0);
                tokens.insert(tokens.begin() + 0, {"TRUE"});
            }

            else {
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 0);
                tokens.insert(tokens.begin() + 0, {"FALSE"});
            }
        }

        else {
            pos = pos + 1;
        }
    }

    if (tokens.size() == 1) {
        if (tokens[0] == "TRUE") {
            return true;
        }

        else {
            return false;
        }
    }

    else {
        return false;
    }
}

vector <string> argument_lexer(string condition) {
    vector <string> tokens;
    string token = "";
    string var_name = "";
    string str = "";
    bool variable_state = false;
    bool string_state = false;

    condition = condition + "<EOF>";

    for (char& chr : condition) {
        token = token + chr;

        if (token == " " || token == "\n") {
            if (string_state == false) {
                token = "";
            }
        }

        else if (token == "<") {
            if (var_name != "") {
                tokens.push_back(var_name);
                var_name = "";
                variable_state = false;
            }

            token = "";
        }

        else if (token == "\"" || token == " \"" || token == "\\\"") {
            if (string_state == true) {
                tokens.push_back(str);
                str = "";
                string_state = false;
            }

            else {
                string_state = true;
            }

            token = "";
        }

        else if (string_state == true) {
            if (token == "\\") {

            }

            else if (token == "\\q") {
                str = str + "\"";
                token = "";
            }

            else if (token == "\\\\") {
                str = str + "\\";
                token = "";
            }

            else {
                str = str + token;
                token = "";
            }
        }

        else if (token == ",") {
            if (var_name != "") {
                tokens.push_back(var_name);
                var_name = "";
                variable_state = false;
            }

            tokens.push_back("COMMA");

            token = "";
        }

        else if (token == "var") {
            tokens.push_back("VAR");
            variable_state = true;
            var_name = "";
            token = "";
        }

        else if (variable_state == true) {
            var_name = var_name + token;
            token = "";
        }
    }

    return tokens;
}

map <string, string> argument_parser(vector <string> tokens, vector <string> tokens_2) {
    map <string, string> temp_variables;
    int pos = 0;
    int pos_2 = 0;

    while (true) {
        if (tokens.size() <= pos || tokens_2.size() <= pos_2) {
            break;
        }

        else if (tokens[pos] == "VAR") {
            if (tokens_2[pos_2] == "VAR") {
                temp_variables[tokens[pos + 1]] = variables[tokens_2[pos_2 + 1]];
                pos = pos + 3;
                pos_2 = pos_2 + 3;
            }

            else {
                temp_variables[tokens[pos + 1]] = tokens_2[pos_2];
                pos = pos + 3;
                pos_2 = pos_2 + 2;
            }
        }

        else {
            cout << "Error." << endl;
        }
    }

    return temp_variables;
}

vector <string> lexer(string code, string lib_name = "") {
    vector <string> tokens;
    vector <string> new_tokens;
    string token = "";
    string str = "";
    string var_name = "";
    string func_name = "";
    string task = "";
    string condition = "";
    string new_code = "";
    string new_lib_name = "";
    int ignore = 0;
    bool string_state = false;
    bool task_state = false;
    bool condition_state = false;
    bool variable_state = false;
    bool func_state = false;
    bool lib_state = false;

    for(char& chr : code) {
        token = token + chr;

        if (token == "}") {
            if (ignore > false) {
                task = task + "}";
                ignore = ignore - 1;
            }

            else {
                tokens.push_back(task);
                task_state = false;
                task = "";
            }

            token = "";
        }

        else if (token == "{") {
            if (task_state == 1) {
                task = task + token;
                ignore = ignore + 1;
            }

            else {
                task_state = 1;
            }

            token = "";
        }

        else if (task_state == true) {
            task = task + token;
            token = "";
        }

        else if (token == ")") {
            tokens.push_back("<CEX>");
            tokens.push_back(condition);
            condition_state = false;
            condition = "";
            token = "";
        }

        else if (condition_state == true) {
            condition = condition + token;
            token = "";
        }

        else if (token == "(") {
            if (func_name != "") {
                if (lib_name != "") {
                    tokens.push_back(lib_name + "." + func_name);
                    pre_func.push_back(lib_name + "." + func_name);
                }

                else {
                    tokens.push_back(func_name);
                    pre_func.push_back(func_name);
                }

                func_state = false;
                func_name = "";
            }

            condition_state = true;
            token = "";
        }

        else if (token == " ") {
            if (string_state == false) {
                token = "";
            }
        }

        else if (token == "\n" || token == "<EOF>") {
            token = "";
        }

        else if (token == "\"" || token == " \"" || token == "\\\"") {
            if (token == " \"") {
                str = str + " ";
            }

            if (string_state == false) {
                string_state = true;
                token = "";
            }

            else if (string_state == true) {
                if (token == "\\\"") {
                    str = str + "\\";
                }

                if (lib_state == true) {
                    new_code = lib_reader_code(str);
                    new_lib_name = lib_reader_lib_name(str);
                    new_tokens = lexer(new_code, new_lib_name);

                    int pos = 0;

                    while (true) {
                        if (new_tokens.size() <= pos) {
                            break;
                        }

                        else if (new_tokens[pos] == "FUNC") {
                            pre_func.push_back(new_tokens[pos + 1]);
                            functions[new_tokens[pos + 1]] = new_tokens[pos + 4];
                            function_arguments[new_tokens[pos + 1]] = new_tokens[pos + 3];
                            pos = pos + 5;
                        }
                        
                        else {
                            pos = pos + 1;
                        }
                    }

                    lib_state = false;
                }

                else {
                    tokens.push_back(str);
                }

                string_state = false;
                token = "";
                str = "";
            }
        }

        else if (string_state == true) {
            if (token == "\\") {

            }

            else if (token == "\\q") {
                str = str + "\"";
                token = "";
            }

            else if (token == "\\\\") {
                str = str + "\\";
                token = "";
            }

            else {
                str = str + token;
                token = "";
            }
        }

        else if (func_state == true) {
            func_name = func_name + token;
            token = "";
        }

        else if (token == "=") {
            if (var_name != "") {
                if (lib_name != "") {
                    tokens.push_back(lib_name + "." + var_name);
                }

                else {
                    tokens.push_back(var_name);
                }

                variable_state = false;
                var_name = "";
            }

            token = "";
        }

        else if (variable_state == true) {
            var_name = var_name + token;
            token = "";
        }

        else if (token == "include") {
            lib_state = true;
            token = "";
        }

        else if (token == "true") {
            tokens.push_back("TRUE");
            token = "";
        }

        else if (token == "false") {
            tokens.push_back("FALSE");
            token = "";
        }

        else if (token == "start") {
            tokens.push_back("START");
            token = "";
        }

        else if (token == "return") {
            tokens.push_back("RETURN");
            token = "";
        }

        else if (token == "func") {
            tokens.push_back("FUNC");
            token = "";
            func_state = 1;
        }

        else if (token == "system") {
            tokens.push_back("SYSTEM");
            token = "";
        }

        else if (token == "var") {
            tokens.push_back("VAR");
            token = "";
            variable_state = true;
        }

        else if (token == "while") {
            tokens.push_back("WHILE");
            token = "";
        }

        else if (token == "for") {
            tokens.push_back("FOR");
            token = "";
        }

        else if (token == "if") {
            tokens.push_back("IF");
            token = "";
        }

        else if (token == "else") {
            tokens.push_back("ELSE");
            token = "";
        }

        else if (token == "!namespace") {
            lib_name = "";
            token = "";
        }

        else {
            for (string i : pre_func) {
                if (token == i) {
                    tokens.push_back("START");
                    tokens.push_back(i);
                    token = "";
                }
            }
        }
    }

    return tokens;
}

string parser(vector <string> tokens) {
    map <string, string> new_variables;
    vector <string> new_tokens;
    vector <string> task_tokens;
    vector <string> condition_tokens;
    vector <string> expression_tokens;
    string code = "";
    string lib_name = "";
    string result = "";
    string chached_variable = "";
    bool condition = false;
    bool last_condition = false;
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

        else if (tokens[pos] == "FUNC") {
            functions[tokens[pos + 1]] = tokens[pos + 4];
            function_arguments[tokens[pos + 1]] = tokens[pos + 3];
            pos = pos + 5;

            if (tokens[pos - 4] == "main") {
                tokens.insert(tokens.begin() + pos, "START");
                tokens.insert(tokens.begin() + pos + 1, "main");
                tokens.insert(tokens.begin() + pos + 2, "<CEX>");
                tokens.insert(tokens.begin() + pos + 3, "");
            }
        }

        else if (tokens[pos] == "RETURN") {
            string input;

            if (tokens[pos + 1] == "VAR") {
                result = variables[tokens[pos + 2]];
                pos = pos + 3;
            }

            else if (tokens[pos + 1] == "SYSTEM") {
                if (tokens[pos + 2] == "<CEX>") {
                    expression_tokens = expression_lexer(tokens[pos + 3]);
                    result = execute(expression_parser(expression_tokens));
                    pos = pos + 4;
                }

                else if (tokens[pos + 2] == "VAR") {
                    result = execute(variables[tokens[pos + 3]]);
                    pos = pos + 4;
                }

                else {
                    result = execute(tokens[pos + 2]);
                    pos = pos + 3;
                }
            }

            else if (tokens[pos + 1] == "INPUT") {
                if (tokens[pos + 2] == "<CEX>") {
                    expression_tokens = expression_lexer(tokens[pos + 3]);
                    cout << expression_parser(expression_tokens);
                }

                else {
                    cout << tokens[pos + 2];
                }
                
                getline(cin >> ws, input);

                result = input;

                if (tokens[pos + 2] == "<CEX>") {
                    pos = pos + 4;
                }

                else {
                    pos = pos + 3;
                }
            }

            else if (tokens[pos + 1] == "<CEX>") {
                expression_tokens = expression_lexer(tokens[pos + 2]);
                result = expression_parser(expression_tokens);
                pos = pos + 3;
            }

            else {
                result = tokens[pos + 1];
                pos = pos + 2;
            }

            break;
        }

        else if (tokens[pos] == "START") {
            if (tokens[pos + 2] == "<CEX>") {
                chached_variables = variables;
                variables = argument_parser(argument_lexer(function_arguments[tokens[pos + 1]]), argument_lexer(tokens[pos + 3]));
                new_tokens = lexer(functions[tokens[pos + 1]]);
                parser(new_tokens);
                variables = chached_variables;
            }

            pos = pos + 4;
        }

        else if (tokens[pos] == "SYSTEM") {
            if (tokens[pos + 1] == "<CEX>") {
                expression_tokens = expression_lexer(tokens[pos + 2]);
                system(expression_parser(expression_tokens).c_str());
                pos = pos + 3;
            }

            else if (tokens[pos + 1] == "VAR") {
                system(variables[tokens[pos + 2]].c_str());
                pos = pos + 3;
            }
        }

        else if (tokens[pos] == "WHILE") {
            condition_tokens.clear();
            condition = false;

            condition_tokens = condition_lexer(tokens[pos + 2]);
            condition = condition_parser(condition_tokens);

            if (condition == true) {
                new_tokens = lexer(tokens[pos + 3]);
                parser(new_tokens);
            }

            else {
                pos = pos + 4;
            }
        }

        else if (tokens[pos] == "FOR") {
            string text;

            new_tokens.clear();
            new_tokens = condition_lexer(tokens[pos + 2]);

            if (new_tokens[3] == "VAR") {
                text = variables[new_tokens[4]];
            }

            else {
                text = new_tokens[3];
            }

            for (char& chr : text) {
                variables[new_tokens[1]] = chr;
                task_tokens = lexer(tokens[pos + 3]);
                parser(task_tokens);
            }

            pos = pos + 4;
        }

        else if (tokens[pos] == "IF") {
            condition_tokens = condition_lexer(tokens[pos + 2]);
            condition = condition_parser(condition_tokens);

            if (condition == true) {
                new_tokens = lexer(tokens[pos + 3]);
                parser(new_tokens);
                last_condition = true;
            }

            pos = pos + 4;
        }

        else if (tokens[pos] == "ELSE") {
            if (tokens[pos + 1] == "IF") {
                if (tokens[pos - 4] == "IF") {
                    if (last_condition == false) {
                        condition_tokens = condition_lexer(tokens[pos + 3]);
                        condition = condition_parser(condition_tokens);

                        if (condition == true) {
                            new_tokens = lexer(tokens[pos + 4]);
                            parser(new_tokens);
                            last_condition = true;
                        }

                        else {
                            last_condition = false;
                        }
                    }

                    else {
                        last_condition = true;
                    }

                    pos = pos + 5;
                }
            }

            else {
                if (last_condition == false) {
                    new_tokens = lexer(tokens[pos + 1]);
                    parser(new_tokens);
                    last_condition = true;
                }

                pos = pos + 4;
            }
        }

        else if (tokens[pos] == "VAR") {
            string input;

            if (tokens[pos + 2] == "VAR") {
                variables[tokens[pos + 1]] = variables[tokens[pos + 3]];
                pos = pos + 4;
            }

            else if (tokens[pos + 2] == "SYSTEM") {
                if (tokens[pos + 3] == "<CEX>") {
                    expression_tokens = expression_lexer(tokens[pos + 4]);
                    variables[tokens[pos + 1]] = execute(expression_parser(expression_tokens));
                    pos = pos + 5;
                }

                else if (tokens[pos + 3] == "VAR") {
                    variables[tokens[pos + 1]] = execute(variables[tokens[pos + 4]]);
                    pos = pos + 5;
                }
            }

            else if (tokens[pos + 2] == "INPUT") {
                if (tokens[pos + 3] == "<CEX>") {
                    expression_tokens = expression_lexer(tokens[pos + 4]);
                    cout << expression_parser(expression_tokens);
                    pos = pos + 5;
                }
                
                getline(cin >> ws, input);

                variables[tokens[pos + 1]] = input;
            }

            else if (tokens[pos + 2] == "START") {
                if (tokens[pos + 4] == "<CEX>") {
                    chached_variables = variables;
                    variables = argument_parser(argument_lexer(function_arguments[tokens[pos + 3]]), argument_lexer(tokens[pos + 5]));
                    new_tokens = lexer(functions[tokens[pos + 3]]);
                    chached_variable = parser(new_tokens);
                    variables = chached_variables;
                    variables[tokens[pos + 1]] = chached_variable;
                }

                pos = pos + 6;
            }

            else if (tokens[pos + 2] == "<CEX>") {
                expression_tokens = expression_lexer(tokens[pos + 3]);
                variables[tokens[pos + 1]] = expression_parser(expression_tokens);
                pos = pos + 4;
            }

            else {
                variables[tokens[pos + 1]] = tokens[pos + 2];
                pos = pos + 3;
            }
        }
    }

    return result;
}

int main(int argc, char **argv) {
    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();
    chrono::duration <double> elapsed_seconds;
    vector <string> tokens;
    string filename;
    string code;
    string arg1 = "";
    string arg2 = "";

    if (argc > 1) {
        arg1 = argv[1];
    }

    else {
        cout << "No input files, proccess terminated." << endl;

        return 0;
    }
    
    if (argc > 2) {
        arg2 = argv[2];
    }

    if (arg1 == "--version") {
        cout << "AeNC 0.0.7" << endl;
        return 0;
    }

    // filename = "test.aenc";

    code = open_file(arg1);
    tokens = lexer(code);

    if(arg2 == "--info") {
        end = chrono::system_clock::now();

        elapsed_seconds = end - start;
        cout << "Ready in: " << elapsed_seconds.count() << endl;

        start = chrono::system_clock::now();
    }

    parser(tokens);

    if(arg2 == "--info") {
        end = chrono::system_clock::now();

        elapsed_seconds = end - start;
        cout << "Finished in: " << elapsed_seconds.count() << endl;
    }

    return 0;
}