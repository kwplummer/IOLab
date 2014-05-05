#include <string>
#include "iomanager.h"
// REMOVE THIS INCLUDE:
#include <gtest/gtest_prod.h>
class FileSystem53
{
  static const int B = 64; // Block length
  static const int K = 7;  // Number of blocks for descriptor table
  static const int FILE_SIZE_FIELD = 1;
  static const int ARRAY_SIZE = 3;
  static const int DESCR_SIZE = FILE_SIZE_FIELD + ARRAY_SIZE;
  static const int MAX_FILE_NO = 14;
  static const int MAX_BLOCK_NO = 64;
  static const int MAX_BLOCK_NO_DIV8 = MAX_BLOCK_NO / 8;
  static const int MAX_FILE_NAME_LEN = 10; // Maximum size of file name in byte.
  static const int MAX_OPEN_FILE = 3;
  static const int FILEIO_BUFFER_SIZE = 64; // Size of file io bufer
  static const int _EOF = -1;               // End-of-File
  const int directoryIndex;
  char descTable[K][B];

  static const int OFT_DIRECTORY_INDEX = 3;
  static const int MAX_FILE_SIZE = B * 3;
  static const int DIRECTORY_DATA_CHUNK_SIZE = 11;
  static const int BYTEMAP_BLOCK_INDEX = 0;

  IOManager io;
  struct OFT
  {
    struct OpenFile
    {
      char buf[B];       // Copy of block we're in
      unsigned char pos; // What to read/write next (0..(B*3)).
      char index;        // Our index in File Descriptor
    };
    OpenFile table[4];
    bool open[4];
    OFT();
  } table;

  // ERROR CODES
  static const int EC_FILE_NOT_OPEN = -1;
  static const int EC_FILE_NOT_FOUND = -1;
  static const int EC_NO_SPACE_IN_DISK = -1;
  static const int EC_DELETE_FAILURE = -1;
  static const int EC_OFT_FULL = -2;
  static const int EC_EOF = -2;
  static const int EC_DUPLICATE_FILE_NAME = -2;
  static const int EC_NEGATIVE_SEEK_POSITION = -100;
  static const int EC_FILE_CURRENTLY_IN_USE = -200;

  // FILE DESCRIPTOR CONSTANTS
  static const int FD_DIRECTORY_FILE_DESCRIPTOR_INDEX = 0;
  static const int FD_FILE_SIZE = 0;
  static const int FD_FIRST_BLOCK = 1;
  static const int FD_SECOND_BLOCK = 2;
  static const int FD_THIRD_BLOCK = 3;
  const std::string LDISK_FILE_NAME;
  static const int START_INDEX_BLOCK_DESC_TABLE = 0;
  static const int END_INDEX_BLOCK_DESC_TABLE = 6;

public:
  FileSystem53();
  void openFileTable();
  void format();
  char *readDescriptor(int no);
  void writeDescriptor(int no, const char *desc);
  int searchDir(const std::string &fileName, char *directoryDataMemArea);
  int create(const std::string &fileName);
  int open(const std::string &fileName);
  int read(int index, char *memArea, int count);
  int write(int index, char value, int count);
  int lseek(int index, int pos);
  int close(int index);
  int deleteFile(const std::string &fileName);
  void directory();
  void restore(const std::string &name);
  void save(const std::string &name);
  int addBlock();
  void initializeOFT(int oftIndex, char *dataBlock, char fileDescriptorIndex);
  int destroy(const std::string &fileName);
  int searchOFT(int fileDescriptorIndex);
  void writeDirectory(const std::string &fileName, const char *directoryData);

  // TESTS, REMOVE BEFORE SUBMITTING.
  FRIEND_TEST(FileSystem53, NoFilesOpenAtStart);
  FRIEND_TEST(FileSystem53, CanOpenFile);
  FRIEND_TEST(FileSystem53, CanOpenAllThreeFiles);
};
