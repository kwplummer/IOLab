#include "filesystem53.h"

FileSystem53::FileSystem53(int l, int b, const std::string &storage)
{
  format();
}

void FileSystem53::OpenFileTable() {}

int FileSystem53::findOft() {}

void FileSystem53::deallocateOft(int index) {}

void FileSystem53::format() {}

char *FileSystem53::readDescriptor(int no) {}

void FileSystem53::clearDescriptor(int no) {}

void FileSystem53::writeDescriptor(int no, char *desc) {}

int FileSystem53::findEmptyDescriptor() {}

int FileSystem53::findEmptyBlock() {}

int FileSystem53::fgetc(int index) {}

int FileSystem53::fputc(int c, int index) {}

bool FileSystem53::feof(int index) {}

int FileSystem53::searchDir(int index, const std::string &symbolic_file_name) {}

void FileSystem53::deleteDir(int index, int start_pos, int len) {}

int FileSystem53::create(const std::string &symbolic_file_name) {}

int FileSystem53::openDesc(int desc_no) {}

int FileSystem53::open(const std::string &symbolic_file_name) {}

int FileSystem53::read(int index, char *mem_area, int count) {}

int FileSystem53::write(int index, char value, int count) {}

int FileSystem53::lseek(int index, int pos) {}

void FileSystem53::close(int index) {}

int FileSystem53::deleteFile(const std::string &fileName) {}

void FileSystem53::directory() {}

void FileSystem53::restore(const std::string &name) {}

void FileSystem53::save(const std::string &name) {}

void FileSystem53::diskDump(int start, int size) {}
