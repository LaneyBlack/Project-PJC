#include "imageFile.h"
#include "iostream"
#include <fstream>

using namespace std;

/**
 * Constructor of the ImageFile
 * @param path
 * Creates new filesystem path
 * Throws exception if fileFormat is not supported
 */
ImageFile::ImageFile(char *path) {
    this->path = path;
    filePath = filesystem::path(path);
    format = getFileExtension();
    if (format == ImageFormat::other)
        throw std::runtime_error("App does not support " + filePath.extension().string() + "  file extensions");
    width = 0;
    height = 0;
    size = 0;
    bitsPerPixel = 0;
    lastWriteTime = last_write_time(filePath);
    hasMessage = false;
}


/**
 * this method reads header of the file, to get the image information
 * if successfully then @return true, else @return false
 * .BMP -----------------------------
 * .bmp read is nice and easy
 * 14 40 in hex is 5184 in decimal
 * in .bmp file it looks like 00 00 40 14 and fs.read(val,4) reads it correctly
 * .PNG -----------------------------
 * .png read is harder
 * 07 80 in hex is 1920 in decimal
 * in .png file it looks like 00 00 07 80 and fs.read(val,4) reads 8007, which is not correct
 * so I read ot one by one
 */
bool ImageFile::readHeader() {
    if (format == ImageFormat::bmp) {
        ifstream fileStream(path, ios::binary);
        fileStream.seekg(2, ios::beg); //skip to file size
        fileStream.read((char *) &size, 4);
        fileStream.seekg(12, ios::cur); //skip to dimensions
        fileStream.read((char *) &width, 4);
        fileStream.read((char *) &height, 4);
        fileStream.seekg(28, ios::beg); //skip to bits per pixel
        fileStream.read((char *) &bitsPerPixel, 2);
        fileStream.seekg(-9, ios::end); //my mark lays here
        unsigned int tmp = 0;
        fileStream.read((char *) &tmp, 1); //if byte is 4A, then I have putted a message down there
        if (tmp == 166)  //4A in decimal is 166
            hasMessage = true;
        fileStream.close();
        return true;
    } else if (format == ImageFormat::png) {
        unsigned int tmp = 0;
        ifstream fileStream(path, ios::binary);
        fileStream.seekg(8, ios::beg); //skip to IHDR chunk
        fileStream.seekg(8, ios::cur); //skip to dimensions
        for (int i = 3; i >= 0; --i) {
            fileStream.read((char *) &tmp, 1);
            width += (tmp << 8 * i);
        }
        for (int i = 3; i >= 0; --i) {
            fileStream.read((char *) &tmp, 1);
            height += (tmp << 8 * i);
        }
        fileStream.read((char *) &bitsPerPixel, 1);
        fileStream.seekg(-9, ios::end);
        fileStream.read((char *) &tmp, 1); //if byte is A4, then I have putted a message down there
        if (tmp == 166)  //A4 in decimal is 166
            hasMessage = true;
        fileStream.seekg(0, ios::end);
        size = fileStream.tellg();
        fileStream.close();
        return true;
    }
    return false;
}

/**
 * function gets a file extension to future use
 * @return ImageFormat type
 */
ImageFormat ImageFile::getFileExtension() const {
    if (this->filePath.extension() == ".bmp")
        return ImageFormat::bmp;
    else if (this->filePath.extension() == ".png")
        return ImageFormat::png;
    return other;
}
/**
 * Encrypts message into the file
 * @return true if encrypted, @return false if not encrypted
 *
 * Encrypting file starts from the end, to minimise the chance of destructing the image.
 * For some reason after encrypting I cannot open file in CLion, but Windows open's it easily
 *
 * Copying the data from one file to another, to then edit it and add my encryption,
 * because method write() rewrites the hole file, if this file was not written in the same file.open().
 * Rewriting the file goes by every 8 bytes. So I reserved 8 byte long long to have a safe memory container
 * to work with.
 *
 * At the start and end of the message I put A4 (166 in decimal) to mark the message
 * Shifting every character in the message by 12 character in ASCII, so the message is not visible
 * from the hex editor.
 *
 * The end of the file is usually 8 bytes long. So I skip those bytes, place my mark and start encrypting my message.
 *
 * At the end of the method deleting the old file and replacing it with a new one.
 */
bool ImageFile::encryptMessage(string message) {
    if (checkFile(message)) {
        fstream fileInput(path, ios::in | ios::binary);
        filesystem::path newFile = filesystem::path(
                (filePath.string().substr(0, filePath.string().find_last_of("\\/")) + "\\out.png"));
        //                         separating file folder with file name and                        creating a new file
        fstream fileOutput(newFile.string(), ios::out | ios::binary);
        long long temp = 0; //reservation of 8 safe bytes in memory to work with
        while (fileInput.good()) {
            if (size - fileInput.tellg() > 7) {
                fileInput.read((char *) &temp, 8);
                fileOutput.write((char *) &temp, 8);
            } else if (size - fileInput.tellg() > 0) {
                int bytesLeft = size - fileInput.tellg(); // if I put this operation in method without a new var it doesn't work
                fileInput.read((char *) &temp, bytesLeft);
                fileOutput.write((char *) &temp, bytesLeft);
            } else
                break;
        }
        fileInput.close();
        unsigned int letter = 166; // its A4 in hex, that means start of the message
        fileOutput.seekg(-9, ios::end); //safe place from the end of the file
        fileOutput.write((char *) &letter, 1);
        for (char i: message) {
            if (fileOutput.tellg() < 124)
                return false;
            fileOutput.seekg(-3, ios::cur);
            letter = i + 12;
            fileOutput.write((char *) &letter, 1);
        }
        letter = 166;
        fileOutput.seekg(-2, ios::cur);
        fileOutput.write((char *) &letter, 1);
        fileOutput.close();
        //now replacing a file
        string tmpFilePath = filePath.string();
        filesystem::remove(filePath);
        filesystem::rename(newFile, tmpFilePath);
        newFile.replace_filename(tmpFilePath); //because we also need to replace path in the variable
        filePath = newFile;
        cout << filePath.string() << endl;
        return true;
    }
    return false;
}

/**
 * Function decrypts the message from the file
 * @return true if successfully, @return false if error
 *
 * skipping the end of the file ( 8 bytes)
 * and start reading chars and shifting them by 12 characters in ASCII backwards.
 * If the byte next to the character is A4 (166 in decimal) then it's the end of the message.
 * Printing the message into the console.
 */
bool ImageFile::decryptMessage() const {
    string message;
    bool res = false;
    unsigned int tmp = 0;
    ifstream fileStream(path, ios::in | ios::binary);
    fileStream.seekg(-9, ios::end);
    fileStream.read((char *) &tmp, 1);
    if (tmp == 166) {
        do {
            fileStream.seekg(-3, ios::cur);
            fileStream.read((char *) &tmp, 1);
            message += (char) (tmp - 12);
            fileStream.seekg(-2, ios::cur);
            fileStream.read((char *) &tmp, 1);
            fileStream.seekg(1, ios::cur);
        } while (tmp != 166);
        res = true;
    }
    cout << "Message is: " << endl;
    cout << message << endl;
    return res;
}

/**
 * This function checks if it's possible to encrypt a @param message into the file.
 * If not @return false, if yes @return true.
 * Header size is 124 bytes, ending size is 8 and my marks take 1 byte each. So in both file extensions number of
 * characters to be written is (fileSize - 124 - 8 - 2) / 3
 */
bool ImageFile::checkFile(const string &message) {
    readHeader();
    if (hasMessage) {
        cout << "This file already has a message!" << endl;
        return false;
    } else if ((format == ImageFormat::bmp ||format == ImageFormat::png) // header end
                                                && (message.size() > (size - 124 - 8 - 2) / 3)) {
        cout << "The file is too short, to this message to be encrypted" << endl;
        return false;
    }
    return true;
}
/**
 * This function get
 * @param value is the filesystem type timestamp of last file editing
 * @return string value of this timestamp
 *
 * First of all I get a system time clock and then get the last edition time of the file in the system time.
 * Then I turn gotten message into ASCII time, by getting local time of the previous made variable.
 */
auto getStringTime(filesystem::file_time_type * value) {
    long long int cftime = chrono::system_clock::to_time_t(chrono::file_clock::to_sys(*value));
    return asctime(localtime(&cftime));
}
/**
 * Prints all the info of the file
 */
void ImageFile::printInfo() {
    cout << "File name: " << this->filePath.filename().string() <<
         "\nFile size: " << this->size << " bytes" <<
         "\nFile dimensions: " << width << "x" << height <<
         "\nBits per pixel (per channel): " << bitsPerPixel <<
         "\nLast edited: " << getStringTime(&lastWriteTime) << //date ends with \n
         "Has message: " << (hasMessage ? "yes" : "no") << endl;
}
/**
 * Destructor that destroys ImageFile varibles
 */
ImageFile::~ImageFile() {
    delete &filePath;
}
