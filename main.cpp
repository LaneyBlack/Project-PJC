#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <winsock.h>

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
 * @param value string input
 * @return lowercase string
 */
string toLowerCase(string value) {
    for (int i = 0; i < value.size(); ++i)
        value[i] = tolower(value[i]);
    return value;
}

/**
 * Method made to convert a string input into enum Command value
 * @param input string command
 * @return enum command
 */
Commands getCommand(string input) {
    input = toLowerCase(input);
    if (input == "--info" || input == "-i") return info;
    if (input == "--encrypt" || input == "-e") return encrypt;
    if (input == "--decrypt" || input == "-d") return decrypt;
    if (input == "--check" || input == "-c") return check;
    if (input == "--help" || input == "-h") return help;
    if (input == "--leave" || input == "-l") return leave;
    return error;
}

/**
 * Prints all the commands into commandline
 */
void printHelp() {
    cout << "Commands:\n"
            " -h, --help\t---\tstands for user manual\n"
            " -i, --info\t---\tto change path to the file we are working with\n"
            " -e, --encrypt\t---\tencrypts message from file\n"
            " -d, --decrypt\t---\tdecrypts message from file\n"
            " -c, --check\t---\tchecks if it's safe to work with file\n"
            " -l, --leave\t---\texits from program" << endl;
}

bool checkFile(fs::path file) {
    if (file.extension() == ".jpg" || file.extension() == ".png" || file.extension() == ".jpeg")
        return true;
    return false;
}

bool getImageDimensions(string path, int width, int height) {
    //FixMe
    FILE * file = fopen(path,"rb");
    fs::path filePath(path);
    if (file == 0 || !(filePath.extension() == ".jpg" || filePath.extension() == ".png" || filePath.extension() == ".jpeg"))
        return false;
    fseek(file, 0, SEEK_END);
    long fileLength = ftell(file);
    if (fileLength < 24) {
        fclose(file);
        return false;
    }
    char info [24];
    fread(info, 1, 24, file);
    if (filePath.extension() == ".jpg" || filePath.extension() == ".jpeg" ) {
        height = (info[7]<<8) + info[8];
        width = (info[9]<<8) + info[10];
        cout << height << endl;
        cout << width << endl;
        return true;
    }

}

void printInfo(string imagePath) {
    //FixMe
    fs::path file(imagePath);
    if (checkFile(file)) {
        cout << "Filename:\t" << file.filename().string() << endl;
        int x, y;
        getImageDimensions(file.filename().string(),x,y);
    } else {
        cout << "Wrong path or file format!" << endl;
    }
}

/**
 * Main
 * All work with user is done here
 * @return
 */
int main() {
    cout << "Program loaded and ready to go (-h stands for help):" << endl;
    string in, path;
    do {
        cin >> in;
        switch (getCommand(in)) {
            case info:
                cout << "Please enter a file path to get info:" << endl;
                cin >> path;
                printInfo(path);
                break;
            case encrypt:
                break;
            case decrypt:
                break;
            case check:
                break;
            case help:
                printHelp();
                break;
            case error:
                cout << "Error! Wrong command input." << endl;
                break;
            case leave:
                cout << "Program closed" << endl;
                break;
        }
    } while (getCommand(in) != leave);


    return 0;
}
