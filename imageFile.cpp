#include "imageFile.h"
#include "iostream"
#include <fstream>

using namespace std;

/**
 * Constructor of the ImageFile
 * @param path
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

ImageFormat ImageFile::getFileExtension() {
    if (this->filePath.extension() == ".bmp")
        return ImageFormat::bmp;
    else if (this->filePath.extension() == ".png")
        return ImageFormat::png;
    return other;
}

void ImageFile::encryptMessage(string message) { //ToDo &
    if (checkFile(message)) {
        fstream fileInput(path, ios::in | ios::binary);
        fstream fileOutput;
        unsigned long tempBytes = 0;
        int headerSize = 0;
        if (format == ImageFormat::bmp) {
            fileOutput.open("out.bmp", ios::out | ios::binary);
            headerSize = 124;
        } else if (format == ImageFormat::png) {
            fileOutput.open("out.png", ios::out | ios::binary);
            headerSize = 242;
        }
        for (int i = 0; i < headerSize; i += 8) {
            if (headerSize - i > 7) {
                fileInput.read((char *) &tempBytes, 8);
                fileOutput.write((char *) &tempBytes, 8);
            } else {
                fileInput.read((char *) &tempBytes, headerSize % 8);
                fileOutput.write((char *) &tempBytes, headerSize % 8);
            }
        }

        unsigned int letter = 166;
        unsigned int messageIndex = 0;
        fileOutput.write((char *) &letter, 1);
        fileInput.seekg(1, ios::cur);
        while (!fileInput.eof()) {
            if (messageIndex < message.size()) {
                tempBytes = 0;
                fileInput.read((char *) &tempBytes, 7);
                fileOutput.write((char *) &tempBytes, 7);
                letter = (int) message[messageIndex];
                fileOutput.write((char *) &letter, 1);
                fileInput.seekg(1, ios::cur);
                ++messageIndex;
            } else if (messageIndex == message.size()) {
                letter = 166;
                fileOutput.write((char *) &letter, 1);
                fileInput.seekg(1, ios::cur);
                ++messageIndex;
            } else if (size - fileInput.tellg() > 8) {
                tempBytes = 0;
                fileInput.read((char *) &tempBytes, 8);
                fileOutput.write((char *) &tempBytes, 8);
            } else if (size - fileInput.tellg() > 0) {
                tempBytes = 0;
                int tmp = size - fileInput.tellg();
                cout << tmp << endl;
                fileInput.read((char *) &tempBytes, tmp);
                fileOutput.write((char *) &tempBytes, tmp);
//
//                fileInput.read((char *) &tempBytes, 6);
//                fileOutput.write((char *) &tempBytes, 6);
            } else {
                break;
            }
        }
//        fileInput.read((char *) &tempBytes, size - fileInput.tellg());
//        fileOutput.write((char *) &tempBytes, size - fileInput.tellg());
        fileInput.close();
        fileOutput.close();
        filePath.replace_filename("out.png");
//        unsigned int fileBytes[size];
//        for (int i = 0; i < size; ++i) {
//            fileInput.read((char *) &fileBytes[i], 1);
//        }
//        fileInput.seekg(0, ios::beg);
//        int headerSize = 0;
//        if (format == ImageFormat::bmp)
//            headerSize = 124;
//        else if (format == ImageFormat::png)
//            headerSize = 34;
//        unsigned int tmp = 166;
//        for (int i = 0; i < size; ++i) {
//            if (i < headerSize)
//                fileInput.write((char *) &fileBytes[i], 1);
//            else if (i == headerSize)
//                fileInput.write((char *) &letter, 1);
//            else if (i > headerSize) {
//                if (fileInput.tellg() %)
//            }
    }
}
//    if (checkFile(message)) {
//        fstream fileStream(path, ofstream::app | ios::binary);
//        if (format == ImageFormat::bmp)
//            fileStream.seekg(124, ios::beg);
//        else if (format == ImageFormat::png)
//            fileStream.seekg(34, ios::beg);
//        unsigned int tmp = 166;
//        fileStream.write((char *) &tmp, 1);
//        for (int i = 0; i < message.size(); ++i) {
//            fileStream.seekg(7, ios::cur);
//            tmp = (int) message[i];
//            fileStream.write((char *) &tmp, 1);
//        }
//        tmp = 166;
//        fileStream.write((char *) &tmp, 1);
//        hasMessage = true;
//        fileStream.close();
//    }
//}

void ImageFile::decryptMessage() {
    string message = "";
    unsigned int tmp = 0;
    ifstream fileStream(path, ios::binary);
    if (format == ImageFormat::bmp)
        fileStream.seekg(124, ios::beg);
    else if (format == ImageFormat::png)
        fileStream.seekg(34, ios::beg);
    fileStream.read((char *) &tmp, 1);
    if (tmp == 166) {
        do {
            fileStream.seekg(7, ios::cur);
            fileStream.read((char *) &tmp, 1);
            message += (char) tmp;
            fileStream.read((char *) &tmp, 1);
            fileStream.seekg(-1,ios::cur);
        } while (tmp != 166);
    }
    cout << message << endl;
}

bool ImageFile::checkFile(string message) { //ToDo &
    readHeader();
    if (hasMessage) {
        cout << "This file already has a message!" << endl;
        return false;
    } else if ((format == ImageFormat::bmp && (message.size() > (size - 124 - 2) / 8)) ||
               (format == ImageFormat::png && (message.size() > (size - 34 - 2) / 8))) {
        cout << "The file is too short, to this message to be encrypted" << endl;
        return false;
    }
    return true;
}

auto getStringTime(filesystem::file_time_type value) { //ToDo &
    //ToDo understand
    auto cftime = chrono::system_clock::to_time_t(chrono::file_clock::to_sys(value));
    return asctime(localtime(&cftime));
}

void ImageFile::printInfo() {
    cout << "File name: " << this->filePath.filename().string() <<
         "\nFile size: " << this->size << " bytes" <<
         "\nFile dimensions: " << width << "x" << height <<
         "\nBits per pixel (per channel): " << bitsPerPixel <<
         "\nLast edited: " << getStringTime(lastWriteTime) << //date ends with \n
         "Has message: " << (hasMessage ? "yes" : "no") << endl;
}

/**
 * this method reads header of the file, to get the image information
 * if successfully then @return true, else @return false
 * .BMP -----------------------------
 * .bmp read is nice and easy
 * 1440 in hex is 5184 in decimal
 * in .bmp file it looks like 00 00 40 14 and fs.read(val,4) reads it correctly
 * .PNG -----------------------------
 * .png read is harder
 * 780 in hex is 1920 in decimal
 * in .png file it looks like 00 00 07 80 and fs.read(val,4) reads 8007, which is not correct
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
        fileStream.seekg(124, ios::beg); //the biggest bmp header size is 124 bytes,
        // so to be sure the start of the message is here
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
        fileStream.seekg(34, ios::beg); //png information ends at 33 bytes
        fileStream.read((char *) &tmp, 1); //if byte is 74, then I have putted a message down there
        if (tmp == 116)  //74 in decimal is 116c
            hasMessage = true;
        fileStream.seekg(0, ios::end);
        size = fileStream.tellg();
        fileStream.close();
        return true;
    }
    return false;
}

ImageFile::~ImageFile() {
}
