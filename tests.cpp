#include <gtest/gtest.h>
#include "filesystem53.h"

TEST(IOManager, CanCreateManager) { IOManager io; }

TEST(IOManager, WriteBlockWorks)
{
  IOManager io;
  io.write_block(1, "Hey");
  char p[64];
  io.read_block(1, p);
  ASSERT_STREQ("Hey", p);
}

TEST(IOManager, SaveWorks)
{
  IOManager io;
  io.write_block(1, "Hello There");
  io.save("TestOutput.txt");
  io.write_block(1, "Hey");
  char p[64];
  io.load("TestOutput.txt");
  io.read_block(1, p);
  ASSERT_STREQ("Hello There", p);
}

TEST(FileSystem53, CanCreateFS) { FileSystem53 fs; }

TEST(FileSystem53, NoFilesOpenAtStart)
{
  FileSystem53 fs;
  for(int i = 0; i < 3; ++i)
  {
    ASSERT_FALSE(fs.table.open[i]);
  }
}

TEST(FileSystem53, CanCreateFile)
{
  FileSystem53 fs;
  ASSERT_EQ(0, fs.create("Test1.txt"));
}

TEST(FileSystem53, CanOpenFile)
{
  FileSystem53 fs;
  ASSERT_EQ(0, fs.create("Test1.txt"));
  ASSERT_EQ(0, fs.open("Test1.txt"));
  ASSERT_TRUE(fs.table.open[0]);
}

TEST(FileSystem53, CanOpenAllThreeFiles)
{
  FileSystem53 fs;
  ASSERT_EQ(0, fs.create("Test1.txt"));
  ASSERT_EQ(0, fs.open("Test1.txt"));
  ASSERT_TRUE(fs.table.open[0]);
  ASSERT_EQ(0, fs.create("Test2.txt"));
  ASSERT_EQ(1, fs.open("Test2.txt"));
  ASSERT_TRUE(fs.table.open[1]);
  ASSERT_EQ(0, fs.create("Test3.txt"));
  ASSERT_EQ(2, fs.open("Test3.txt"));
  ASSERT_TRUE(fs.table.open[2]);
}

TEST(FileSystem53, WritingToInvalidFileHasError)
{
  FileSystem53 fs;
  ASSERT_EQ(-1, fs.write(-2, 'f', 8));
  ASSERT_EQ(-1, fs.write(95, 'f', 8));
}

TEST(FileSystem53, WritingToUnopenedFileHasError)
{
  FileSystem53 fs;
  ASSERT_EQ(-1, fs.write(0, 'f', 8));
}

TEST(FileSystem53, CanWriteOneCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  ASSERT_EQ(1, fs.write(index, 'f', 1));
}

TEST(FileSystem53, CanWriteOneCharToOpenFileIfAllThreeAreOpen)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  fs.open("Test.txt");
  fs.create("Test2.txt");
  fs.open("Test2.txt");
  fs.create("Test3.txt");
  fs.open("Test3.txt");
  ASSERT_EQ(1, fs.write(0, 'f', 1));
  ASSERT_EQ(1, fs.write(1, 'j', 1));
  ASSERT_EQ(1, fs.write(2, 'm', 1));
}

TEST(FileSystem53, CanWriteMoreThanOneBlockOfCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  ASSERT_EQ(1, fs.write(index, 'f', 66));
}

TEST(FileSystem53, CanWriteMoreThanOneBlockOfCharToOpenFileIfAllThreeAreOpen)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  fs.open("Test.txt");
  fs.create("Test2.txt");
  fs.open("Test2.txt");
  fs.create("Test3.txt");
  fs.open("Test3.txt");
  ASSERT_EQ(1, fs.write(0, 'f', 66));
  ASSERT_EQ(1, fs.write(1, 'j', 66));
  ASSERT_EQ(1, fs.write(2, 'm', 66));
}

TEST(FileSystem53, CanNotWriteMoreThanThreeBlocksOfCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  ASSERT_EQ(-2, fs.write(index, 'f', 9000));
}

TEST(FileSystem53,
     CanNotWriteMoreThanThreeBlocksOfCharToOpenFileIfAllThreeAreOpen)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  fs.open("Test.txt");
  fs.create("Test2.txt");
  fs.open("Test2.txt");
  fs.create("Test3.txt");
  fs.open("Test3.txt");
  ASSERT_EQ(-2, fs.write(0, 'f', 9000));
  ASSERT_EQ(-2, fs.write(1, 'j', 9000));
  ASSERT_EQ(-2, fs.write(2, 'm', 9000));
}

TEST(FileSystem53, CanResumeWriteOneCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  ASSERT_EQ(1, fs.write(index, 'f', 1));
  ASSERT_EQ(1, fs.write(index, 'f', 2));
}

TEST(FileSystem53, CanResumeWriteOneCharToOpenFileIfAllThreeAreOpen)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  fs.open("Test.txt");
  fs.create("Test2.txt");
  fs.open("Test2.txt");
  fs.create("Test3.txt");
  fs.open("Test3.txt");
  ASSERT_EQ(1, fs.write(0, 'f', 1));
  ASSERT_EQ(1, fs.write(1, 'j', 1));
  ASSERT_EQ(1, fs.write(2, 'm', 1));
  ASSERT_EQ(1, fs.write(1, 'f', 2));
  ASSERT_EQ(1, fs.write(2, 'j', 2));
  ASSERT_EQ(1, fs.write(0, 'm', 2));
}

TEST(FileSystem53, CanResumeWriteMoreThanOneBlockOfCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  ASSERT_EQ(1, fs.write(index, 'f', 66));
  ASSERT_EQ(1, fs.write(index, 'f', 66));
}

TEST(FileSystem53,
     CanResumeWriteMoreThanOneBlockOfCharToOpenFileIfAllThreeAreOpen)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  fs.open("Test.txt");
  fs.create("Test2.txt");
  fs.open("Test2.txt");
  fs.create("Test3.txt");
  fs.open("Test3.txt");
  ASSERT_EQ(1, fs.write(0, 'f', 66));
  ASSERT_EQ(1, fs.write(2, 'm', 66));
  ASSERT_EQ(1, fs.write(1, 'j', 66));
  ASSERT_EQ(1, fs.write(2, 'm', 66));
  ASSERT_EQ(1, fs.write(0, 'f', 66));
  ASSERT_EQ(1, fs.write(1, 'j', 66));
}

TEST(FileSystem53, CanOverwriteOneCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  ASSERT_EQ(1, fs.write(index, 'f', 1));
  fs.lseek(0, 0);
  ASSERT_EQ(1, fs.write(index, 'f', 1));
}

TEST(FileSystem53, CanOverwriteOneCharToOpenFileIfAllThreeAreOpen)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  fs.open("Test.txt");
  fs.create("Test2.txt");
  fs.open("Test2.txt");
  fs.create("Test3.txt");
  fs.open("Test3.txt");
  ASSERT_EQ(1, fs.write(0, 'f', 1));
  ASSERT_EQ(1, fs.write(1, 'j', 1));
  ASSERT_EQ(1, fs.write(2, 'm', 1));
  fs.lseek(0, 0);
  fs.lseek(1, 0);
  fs.lseek(2, 0);
  ASSERT_EQ(1, fs.write(0, 'f', 1));
  ASSERT_EQ(1, fs.write(1, 'j', 1));
  ASSERT_EQ(1, fs.write(2, 'm', 1));
}

TEST(FileSystem53, CanOverwriteMoreThanOneBlockOfCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  ASSERT_EQ(1, fs.write(index, 'f', 66));
  fs.lseek(0, 0);
  ASSERT_EQ(1, fs.write(index, 'f', 66));
}

TEST(FileSystem53,
     CanOverwriteMoreThanOneBlockOfCharToOpenFileIfAllThreeAreOpen)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  fs.open("Test.txt");
  fs.create("Test2.txt");
  fs.open("Test2.txt");
  fs.create("Test3.txt");
  fs.open("Test3.txt");
  ASSERT_EQ(1, fs.write(0, 'f', 66));
  ASSERT_EQ(1, fs.write(1, 'j', 66));
  ASSERT_EQ(1, fs.write(2, 'm', 66));
  fs.lseek(0, 0);
  fs.lseek(1, 0);
  fs.lseek(2, 0);
  ASSERT_EQ(1, fs.write(0, 'f', 66));
  ASSERT_EQ(1, fs.write(1, 'j', 66));
  ASSERT_EQ(1, fs.write(2, 'm', 66));
}

TEST(FileSystem53, CanNotOverwriteMoreThanThreeBlocksOfCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  ASSERT_EQ(-2, fs.write(index, 'f', 9000));
  fs.lseek(0, 0);
  ASSERT_EQ(-2, fs.write(index, 'f', 9000));
}

TEST(FileSystem53,
     CanNotOverwriteMoreThanThreeBlocksOfCharToOpenFileIfAllThreeAreOpen)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  fs.open("Test.txt");
  fs.create("Test2.txt");
  fs.open("Test2.txt");
  fs.create("Test3.txt");
  fs.open("Test3.txt");
  ASSERT_EQ(-2, fs.write(0, 'f', 9000));
  ASSERT_EQ(-2, fs.write(1, 'j', 9000));
  ASSERT_EQ(-2, fs.write(2, 'm', 9000));
  fs.lseek(0, 0);
  fs.lseek(1, 0);
  fs.lseek(2, 0);
  ASSERT_EQ(-2, fs.write(0, 'f', 9000));
  ASSERT_EQ(-2, fs.write(1, 'j', 9000));
  ASSERT_EQ(-2, fs.write(2, 'm', 9000));
}

TEST(FileSystem53, CanResumeOverwriteOneCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  ASSERT_EQ(1, fs.write(index, 'f', 1));
  ASSERT_EQ(1, fs.write(index, 'f', 2));
  fs.lseek(0, 0);
  ASSERT_EQ(1, fs.write(index, 'f', 1));
  ASSERT_EQ(1, fs.write(index, 'f', 2));
}

TEST(FileSystem53, CanResumeOverwriteOneCharToOpenFileIfAllThreeAreOpen)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  fs.open("Test.txt");
  fs.create("Test2.txt");
  fs.open("Test2.txt");
  fs.create("Test3.txt");
  fs.open("Test3.txt");
  ASSERT_EQ(1, fs.write(0, 'f', 1));
  ASSERT_EQ(1, fs.write(1, 'j', 1));
  ASSERT_EQ(1, fs.write(2, 'm', 1));
  ASSERT_EQ(1, fs.write(1, 'f', 2));
  ASSERT_EQ(1, fs.write(2, 'j', 2));
  ASSERT_EQ(1, fs.write(0, 'm', 2));
  fs.lseek(0, 0);
  fs.lseek(1, 0);
  fs.lseek(2, 0);
  ASSERT_EQ(1, fs.write(0, 'f', 1));
  ASSERT_EQ(1, fs.write(1, 'j', 1));
  ASSERT_EQ(1, fs.write(2, 'm', 1));
  ASSERT_EQ(1, fs.write(1, 'f', 2));
  ASSERT_EQ(1, fs.write(2, 'j', 2));
  ASSERT_EQ(1, fs.write(0, 'm', 2));
}

TEST(FileSystem53, CanResumeOverwriteMoreThanOneBlockOfCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  ASSERT_EQ(1, fs.write(index, 'f', 66));
  ASSERT_EQ(1, fs.write(index, 'f', 66));
  fs.lseek(0, 0);
  ASSERT_EQ(1, fs.write(index, 'f', 66));
  ASSERT_EQ(1, fs.write(index, 'f', 66));
}

TEST(FileSystem53,
     CanResumeOverwriteMoreThanOneBlockOfCharToOpenFileIfAllThreeAreOpen)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  fs.open("Test.txt");
  fs.create("Test2.txt");
  fs.open("Test2.txt");
  fs.create("Test3.txt");
  fs.open("Test3.txt");
  ASSERT_EQ(1, fs.write(0, 'f', 66));
  ASSERT_EQ(1, fs.write(2, 'm', 66));
  ASSERT_EQ(1, fs.write(1, 'j', 66));
  ASSERT_EQ(1, fs.write(2, 'm', 66));
  ASSERT_EQ(1, fs.write(0, 'f', 66));
  ASSERT_EQ(1, fs.write(1, 'j', 66));
  fs.lseek(0, 0);
  fs.lseek(1, 0);
  fs.lseek(2, 0);
  ASSERT_EQ(1, fs.write(0, 'f', 66));
  ASSERT_EQ(1, fs.write(2, 'm', 66));
  ASSERT_EQ(1, fs.write(1, 'j', 66));
  ASSERT_EQ(1, fs.write(2, 'm', 66));
  ASSERT_EQ(1, fs.write(0, 'f', 66));
  ASSERT_EQ(1, fs.write(1, 'j', 66));
}

/*
        int create(string symbolic_file_name);
        int open(string symbolic_file_name);
        int write(int index, char value, int count);
        int read(int index, char* mem_area, int count);
        void close(int index);
        int deleteFile(string fileName);

        void directory();
        int lseek(int index, int pos);
*/
/*Not implemented tests*/
TEST(FileSystem53, CanNotCreateTwoFilesWithSameName) { /*TODO*/}
TEST(FileSystem53, CanNotOpenFileNotCreated) { /*TODO*/}
