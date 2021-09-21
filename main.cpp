#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

vector <string> tokens = {};

string codeline = "";
string code = "";

void open_file(string filename) {
    if(filename.substr(filename.find_last_of(".") + 1) == "aenc") {
        ifstream codefile(filename);

        while (getline(codefile, codeline)) {
            code = code + codeline + "\n";
        }
    }
}

void lexer(string code) {
    string token;
    string str;
    int stage = 0;

    for(char& chr : code) {
        token = token + chr;

        if (token == " ") {
            if (stage == 0) {
                token = "";
            }
        }

        else if (token == "\n") {
            token = "";
        }

        else if (token == "PRINT") {
            tokens.push_back("PRINT");
            token = "";
        }

        else if (token == "\"") {
            token = "";

            if (stage == 0) {
                stage = 1;
            }

            else if (stage == 1) {
                tokens.push_back(str);
                stage = 0;
                str = "";
            }
        }

        else if (stage == 1) {
            str = str + token; 
            token = "";
        }
    }
}

void parser() {
    int pos = 0;

    while (true) {
        if (tokens[pos] == "PRINT") {
            cout << tokens[pos + 1] << endl;
            pos = pos + 2;
        }

        else {
            pos = pos + 1;
        }
    }
}

int main() {
    string filename;
    cout << "> ";
    cin >> filename;
    system("cls");

    open_file(filename);
    lexer(code);
    parser();
}