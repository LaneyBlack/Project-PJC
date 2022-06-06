#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;
using namespace std;
/**
 * Enum with all available command outputs
 */
enum Commands {
    info,
    encrypt,
    decrypt,
    check,
    help,
    error,
    leave
};
/**
 * Translates string from Higher case, to Lower case
 * @param value
 * @return
 */
string toLowerCase(string value) {
    for (int i = 0; i < value.size(); ++i)
        value[i] = tolower(value[i]);
    return value;
}
/**
 * Method made to convert a string input into enum Command value
 * @param input
 * @return
 */
Commands getCommand(string input) {
    input = toLowerCase(input);
    if (input == "-info" || input == "-i") return info;
    if (input == "-encrypt" || input == "-e") return encrypt;
    if (input == "-decrypt" || input == "-d") return decrypt;
    if (input == "-check" || input == "-c") return check;
    if (input == "-help" || input == "-h") return help;
    if (input == "-leave" || input == "-l") return leave;
    return error;
}

/**
 * Main
 * All work with user is done here
 * @return
 */
int main() {
    cout << "Program loaded and ready to go:" << endl;
    string in;
    do {
        cin >> in;
        switch (getCommand(in)) {
            case info:
                break;
            case encrypt:
                break;
            case decrypt:
                break;
            case check:
                break;
            case help:
                cout << "Commands:\n"
                        " -h, -help\t---\tstands for user manual\n"
                        " -i, -info\t---\tto change path to the file we are working with\n"
                        " -e, -encrypt\t---\tencrypts message from file\n"
                        " -d, -decrypt\t---\tdecrypts message from file\n"
                        " -c, -check\t---\tchecks if it's safe to work with file\n"
                        " -l, -leave\t---\texits from program" << endl;
                break;
            case error:
                cout << "Error! Wrong command input." << endl;
                break;
            case leave:
                cout << "Program closing" << endl;
                break;
        }
    } while (getCommand(in) != leave);


    return 0;
}
