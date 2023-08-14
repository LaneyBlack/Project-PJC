#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include "imageFile.h"


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
    error
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
 * Method made to convert a string input into enum Command value.
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
    return error;
}

/**
 * Prints all the commands into commandline.
 */
void printHelp() {
    cout << "Commands:\n"
            " -h, --help\t\t\t\t\t---\tstands for user manual\n"
            " -i, --info [FileName] \t\t\t\t---\t to get information about the file\n"
            " -e, --encrypt [FileName] \"text\"\t\t---\tencrypts message from file\n"
            " -d, --decrypt [FileName] [FileOutputName]\t---\tdecrypts message from file\n"
            " -c, --check [FileName]\t\t\t\t---\tchecks if it's safe to work with file\n"
            " Commands are both usable in upper and lower case." << endl;
}

void wrongCommandInput() {
    throw runtime_error("Error! Wrong command input.\n"
                        "Please use command \'-h\' to see the manual");
}

/**
 * Main
 * Creating an instance of the ImageFile.
 * Then by getting a enum of the command starting working with files.
 * Printing feedback to the user
 * @return 0 if no errors
 */
int main(int argc, char *argv[]) {
    ImageFile *imageFile;
    try {
        switch (getCommand(argv[1])) {
            case info:
                if (argc != 3)
                    wrongCommandInput();
                imageFile = new ImageFile(argv[2]);
                if (imageFile->readHeader())
                    imageFile->printInfo();
                break;
            case encrypt:
                if (argc != 4)
                    wrongCommandInput();
                imageFile = new ImageFile(argv[2]);
                if (imageFile->encryptMessage(argv[3]))
                    cout << "Encrypted successfully" << endl;
                else
                    cout << "Encryption failed" << endl;
                break;
            case decrypt:
                if (argc != 3)
                    wrongCommandInput();
                imageFile = new ImageFile(argv[2]);
                if (imageFile->decryptMessage())
                    cout << "Decrypted successfully" << endl;
                else
                    cout << "Decryption failed" << endl;
                break;
            case check:
                if (argc != 4)
                    wrongCommandInput();
                imageFile = new ImageFile(argv[2]);
                if (imageFile->checkFile(argv[3]))
                    cout << "This message can be encrypted into this file" << endl;
                break;
            case help:
                printHelp();
                break;
            case error:
                wrongCommandInput();
                break;
        }
    } catch (runtime_error e) {
        cout << e.what() << endl;
    }
    return 0;
}
