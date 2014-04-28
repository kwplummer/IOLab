#include "filesystem53.h"
#include <string.h>
FileSystem53::FileSystem53()
{
  format();
}

void FileSystem53::OpenFileTable() {}

int FileSystem53::findOft() {}

void FileSystem53::deallocateOft(int index) {}

void FileSystem53::format()
{
  char input[B];
  for(int i=0;i<7;++i)
  {
    input[i] = 1;
  }
  for(int i=7;i<B;++i)
  {
    input[i] = 0;
  }
  io.write_block(0,input);
  for(int i=0;i<B;++i)
  {
    if(i % 4 == 0)
    {
      input[i] = 0;
    }
    else
    {
      input[i] = -1;
    }
  }
  for(int i=1;i<7;++i)
  {
    io.write_block(i,input);
  }
}

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

int FileSystem53::open(const std::string &symbolic_file_name)
{

}

int FileSystem53::read(int index, char *mem_area, int count) {}

int FileSystem53::write(int index, char value, int count) {}

int FileSystem53::lseek(int index, int pos) {}

void FileSystem53::close(int index) {}

int FileSystem53::deleteFile(const std::string &fileName) {}

void FileSystem53::directory() {}

void FileSystem53::restore(const std::string &name)
{
  io.load(name);
}

void FileSystem53::save(const std::string &name)
{
  io.save(name);
}

void FileSystem53::diskDump(int start, int size) {}
