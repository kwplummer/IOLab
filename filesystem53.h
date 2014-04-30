/*
 * ldisk[0][] = bytemap, [1..6][] is descriptors. [7..] Data
 *
 * --Directory file--
 * Empty, as nothing's created.
 * Bytemap[0..6] is 1.
 *
 * --Creating a file named f1.txt--
 * Empty, title is first content of Directory file.
 * Find descriptor index (1, right after directory's 0), it's descriptor is
 *this:
 *
 * --Descriptor--
 * 0 (size) -1 -1 -1 (blocks)
 * We now update directory, as something's created.
 * Now there's something in directory, it needs a block.
 * Reserve first data block, block 7. Mark bytemap[7] to true. (1)
 *
 * --We write "Hello", char[5] to f1.txt--
 * We allocate block 8. Mark bytemap[8] to true (1)
 *
 * Kyle: Read(), Write()
 * Sophia: Create() Destroy()
 * Ben: Seek() Directory()
 * Holly: Open(), Close()
 *
 * Meet at Wednesday 12:30
*/

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
  char descTable[K][B]; // Unused?
  IOManager io;
  struct OFT
  {
    struct OpenFile
    {
      char buf[B];       // Copy of block we're in
      unsigned char pos; // What to read/write next (0..(B*3)).
      char index;        // Our index in File Descriptor
    };
    OpenFile table[3];
    bool open[3];
    OFT();
  } table;
  
  // ERROR CODES
  static const int EC_FILE_NOT_OPEN = -1;
  static const int EC_NEGATIVE_SEEK_POSITION = -100;

  // FILE DESCRIPTOR CONSTANTS
  static const int FD_DIRECTORY_FILE_DESCRIPTOR_INDEX = 0;
  static const int FD_FILE_SIZE = 0;
  static const int FD_FIRST_BLOCK = 1;
  static const int FD_SECOND_BLOCK = 2;
  static const int FD_THIRD_BLOCK = 3;  

public:
  FileSystem53();
  void OpenFileTable();
  int findOft();
  void deallocateOft(int index);
  void format();
  char *readDescriptor(int no);
  void clearDescriptor(int no);
  void writeDescriptor(int no, char *desc);
  int findEmptyDescriptor();
  int findEmptyBlock();
  int fgetc(int index);
  int fputc(int c, int index);
  bool feof(int index);
  int searchDir(int index, const std::string &fileName);
  int create(const std::string &fileName);
  int openDesc(int desc_no);
  int open(const std::string &fileName);
  int read(int index, char *memArea, int count);
  int write(int index, char value, int count);
  int lseek(int index, int pos);
  void close(int index);
  int deleteFile(const std::string &fileName);
  void directory();
  void restore(const std::string &name);
  void save(const std::string &name);
  void diskDump(int start, int size);
  // Personal additions:
  int addBlock();
  // TESTS, REMOVE BEFORE SUBMITTING.
  FRIEND_TEST(FileSystem53, NoFilesOpenAtStart);
  FRIEND_TEST(FileSystem53, CanOpenFile);
  FRIEND_TEST(FileSystem53, CanOpenAllThreeFiles);
  //REMOVE THIS, IT IS FOR TESTING.
  void lseek_broken(int index, int pos);
};
