#ifndef PROJECT_IMAGE
#define PROJECT_IMAGE

#include <string>
#include <filesystem>

enum ImageFormat {
    bmp,
    png,
    other
};

struct ImageFile {
    std::filesystem::path filePath;
    char * path;
    ImageFormat format;
    bool hasMessage;
    unsigned long width;
    unsigned long height;
    unsigned long size;
    unsigned int bitsPerPixel;
    std::filesystem::file_time_type lastWriteTime;

    ImageFile(char * path);
    bool readHeader();
    void printInfo();
    bool checkFile(std::string message);
    void encryptMessage(std::string message);
    void decryptMessage();
    ImageFormat getFileExtension();
    ~ImageFile();
};

#endif //PROJECT_IMAGE
