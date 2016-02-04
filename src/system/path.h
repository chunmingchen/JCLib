#ifndef PATH_H
#define PATH_H

#include <string>
std::string getPath(const std::string &file);
std::string getFilename(const std::string &filepath);
std::string removeFileExtension(const std::string &filename);
bool isFilenameOnly(const std::string &filename);
std::string getFileExtension(const std::string &filename);

#endif


