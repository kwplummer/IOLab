#include <gtest/gtest.h>
#include "filesystem53.h"

TEST(IOManager, CanCreateManager) { IOManager io; }

TEST(IOManager, WriteBlockWorks)
{
  IOManager io;
  io.write_block(1, "Hey");
  char p[64] = {0};
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
  fs.lseek_broken(0, 0);
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
  fs.lseek_broken(0, 0);
  fs.lseek_broken(1, 0);
  fs.lseek_broken(2, 0);
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
  fs.lseek_broken(0, 0);
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
  fs.lseek_broken(0, 0);
  fs.lseek_broken(1, 0);
  fs.lseek_broken(2, 0);
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
  fs.lseek_broken(0, 0);
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
  fs.lseek_broken(0, 0);
  fs.lseek_broken(1, 0);
  fs.lseek_broken(2, 0);
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
  fs.lseek_broken(0, 0);
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
  fs.lseek_broken(0, 0);
  fs.lseek_broken(1, 0);
  fs.lseek_broken(2, 0);
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
  fs.lseek_broken(0, 0);
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
  fs.lseek_broken(0, 0);
  fs.lseek_broken(1, 0);
  fs.lseek_broken(2, 0);
  ASSERT_EQ(1, fs.write(0, 'f', 66));
  ASSERT_EQ(1, fs.write(2, 'm', 66));
  ASSERT_EQ(1, fs.write(1, 'j', 66));
  ASSERT_EQ(1, fs.write(2, 'm', 66));
  ASSERT_EQ(1, fs.write(0, 'f', 66));
  ASSERT_EQ(1, fs.write(1, 'j', 66));
}

// READ joiewgjewoijgoeiwjhoewjgowejgow

TEST(FileSystem53, ReadingFromInvalidFileHasError)
{
  FileSystem53 fs;
  ASSERT_EQ(-1, fs.read(-1, NULL, 8));
  ASSERT_EQ(-1, fs.read(95, NULL, 8));
}

TEST(FileSystem53, ReadingFromUnopenedFileHasError)
{
  FileSystem53 fs;
  ASSERT_EQ(-1, fs.read(0, NULL, 8));
}

TEST(FileSystem53, CanReadOneCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  ASSERT_EQ(1, fs.write(index, 'f', 1));
  fs.lseek_broken(0, 0);
  char buf[64] = {0};
  ASSERT_EQ(1, fs.read(0, buf, 1));
  ASSERT_STREQ("f", buf);
}

TEST(FileSystem53, CanReadOneCharToOpenFileIfAllThreeAreOpen)
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
  char buf[64] = {0};
  fs.lseek_broken(0, 0);
  fs.lseek_broken(1, 0);
  fs.lseek_broken(2, 0);
  ASSERT_EQ(1, fs.read(0, buf, 1));
  ASSERT_STREQ("f", buf);
  ASSERT_EQ(1, fs.read(1, buf, 1));
  ASSERT_STREQ("j", buf);
  ASSERT_EQ(1, fs.read(2, buf, 1));
  ASSERT_STREQ("m", buf);
}

TEST(FileSystem53, CanReadMoreThanOneBlockOfCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  ASSERT_EQ(1, fs.write(index, 'f', 66));
  char checkMe[67] = {0};
  char buf[256] = {0};
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'f';
  }
  fs.lseek_broken(0, 0);
  ASSERT_EQ(66, fs.read(index, buf, 66));
  ASSERT_STREQ(checkMe, buf);
}

TEST(FileSystem53, CanReadMoreThanOneBlockOfCharToOpenFileIfAllThreeAreOpen)
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
  char checkMe[67] = {'\0'};
  char buf[256] = {'\0'};
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'f';
  }
  fs.lseek_broken(0, 0);
  ASSERT_EQ(66, fs.read(0, buf, 66));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(1, 0);
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'j';
  }
  ASSERT_EQ(66, fs.read(1, buf, 66));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(2, 0);
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'm';
  }
  ASSERT_EQ(66, fs.read(2, buf, 66));
  ASSERT_STREQ(checkMe, buf);
}

TEST(FileSystem53, CanNotReadEmptyFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  fs.open("Test.txt");
  char checkMe[67];
  char buf[65] = {'\0'};
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = '\0';
  }
  ASSERT_EQ(0, fs.read(0, buf, 10));
  ASSERT_STREQ(checkMe, buf);
}

TEST(FileSystem53, ReadingPastEndTruncatesOutput)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  fs.open("Test.txt");
  char checkMe[67] = {'\0'};
  char buf[67] = {'\0'};
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'f';
  }
  ASSERT_EQ(1, fs.write(0, 'f', 66));
  fs.lseek_broken(0, 0);
  ASSERT_EQ(66, fs.read(0, buf, 100));
  ASSERT_STREQ(checkMe, buf);
}

TEST(FileSystem53, CanRequestOnlyABitOfTheFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  fs.open("Test.txt");
  char checkMe[67] = {'\0'};
  char buf[67] = {'\0'};
  for(int i = 0; i < 20; ++i)
  {
    checkMe[i] = 'f';
  }
  ASSERT_EQ(1, fs.write(0, 'f', 66));
  fs.lseek_broken(0, 0);
  ASSERT_EQ(20, fs.read(0, buf, 20));
  ASSERT_STREQ(checkMe, buf);
}

TEST(FileSystem53,
     CanNotReadMoreThanThreeBlocksOfCharToOpenFileIfAllThreeAreOpen)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  fs.open("Test.txt");
  fs.create("Test2.txt");
  fs.open("Test2.txt");
  fs.create("Test3.txt");
  fs.open("Test3.txt");
  char checkMe[67] = {'\0'};
  char buf[67] = {'\0'};
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'f';
  }
  ASSERT_EQ(1, fs.write(0, 'f', 66));
  fs.lseek_broken(0, 0);
  ASSERT_EQ(66, fs.read(0, buf, 100));
  ASSERT_STREQ(checkMe, buf);
  ASSERT_EQ(1, fs.write(1, 'j', 66));
  fs.lseek_broken(1, 0);
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'j';
  }
  ASSERT_EQ(66, fs.read(1, buf, 100));
  ASSERT_STREQ(checkMe, buf);
  ASSERT_EQ(1, fs.write(2, 'm', 66));
  fs.lseek_broken(2, 0);
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'm';
  }
  ASSERT_EQ(66, fs.read(2, buf, 100));
  ASSERT_STREQ(checkMe, buf);
}

TEST(FileSystem53, CanResumeReadOneCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  ASSERT_EQ(1, fs.write(index, 'f', 1));
  ASSERT_EQ(1, fs.write(index, 'f', 2));
  fs.lseek_broken(0, 0);
  char buf[66] = {'\0'};
  char checkMe[66] = {'\0'};
  checkMe[0] = 'f';
  checkMe[1] = 'f';
  checkMe[2] = 'f';
  ASSERT_EQ(3, fs.read(index, buf, 10));
  ASSERT_STREQ(checkMe, buf);
}

TEST(FileSystem53, CanResumeReadOneCharToOpenFileIfAllThreeAreOpen)
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
  ASSERT_EQ(1, fs.write(1, 'j', 2));
  ASSERT_EQ(1, fs.write(2, 'm', 2));
  ASSERT_EQ(1, fs.write(0, 'f', 2));
  fs.lseek_broken(0, 0);
  char buf[66] = {'\0'};
  char checkMe[66] = {'\0'};
  checkMe[0] = 'f';
  checkMe[1] = 'f';
  checkMe[2] = 'f';
  ASSERT_EQ(3, fs.read(0, buf, 10));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(1, 0);
  checkMe[0] = 'j';
  checkMe[1] = 'j';
  checkMe[2] = 'j';
  ASSERT_EQ(3, fs.read(1, buf, 10));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(2, 0);
  checkMe[0] = 'm';
  checkMe[1] = 'm';
  checkMe[2] = 'm';
  ASSERT_EQ(3, fs.read(2, buf, 10));
  ASSERT_STREQ(checkMe, buf);
}

TEST(FileSystem53, CanResumeReadMoreThanOneBlockOfCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  ASSERT_EQ(1, fs.write(index, 'f', 66));
  char checkMe[256] = {'\0'};
  char buf[256] = {'\0'};
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'f';
  }
  fs.lseek_broken(0, 0);
  ASSERT_EQ(66, fs.read(index, buf, 66));
  ASSERT_STREQ(checkMe, buf);
  ASSERT_EQ(1, fs.write(index, 'f', 66));
  fs.lseek_broken(0, 0);
  for(int i = 66; i < 66 * 2; ++i)
  {
    checkMe[i] = 'f';
  }
  ASSERT_EQ(132, fs.read(index, buf, 900));
  ASSERT_STREQ(checkMe, buf);
}

TEST(FileSystem53,
     CanResumeReadMoreThanOneBlockOfCharToOpenFileIfAllThreeAreOpen)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  fs.open("Test.txt");
  fs.create("Test2.txt");
  fs.open("Test2.txt");
  fs.create("Test3.txt");
  fs.open("Test3.txt");
  ASSERT_EQ(1, fs.write(0, 'f', 66));
  char checkMe[256] = {'\0'};
  char buf[256] = {'\0'};
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'f';
  }
  fs.lseek_broken(0, 0);
  ASSERT_EQ(66, fs.read(0, buf, 66));
  ASSERT_STREQ(checkMe, buf);
  ASSERT_EQ(1, fs.write(2, 'm', 66));
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'm';
  }
  fs.lseek_broken(2, 0);
  ASSERT_EQ(66, fs.read(2, buf, 66));
  ASSERT_STREQ(checkMe, buf);
  ASSERT_EQ(1, fs.write(1, 'j', 66));
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'j';
  }
  fs.lseek_broken(1, 0);
  ASSERT_EQ(66, fs.read(1, buf, 66));
  ASSERT_STREQ(checkMe, buf);
  ASSERT_EQ(1, fs.write(2, 'm', 66));
  for(int i = 0; i < 66 * 2; ++i)
  {
    checkMe[i] = 'm';
  }
  fs.lseek_broken(2, 0);
  ASSERT_EQ(132, fs.read(2, buf, 800));
  ASSERT_STREQ(checkMe, buf);
  ASSERT_EQ(1, fs.write(0, 'f', 66));
  for(int i = 0; i < 66 * 2; ++i)
  {
    checkMe[i] = 'f';
  }
  fs.lseek_broken(0, 0);
  ASSERT_EQ(132, fs.read(0, buf, 800));
  ASSERT_STREQ(checkMe, buf);
  ASSERT_EQ(1, fs.write(1, 'j', 66));
  for(int i = 0; i < 66 * 2; ++i)
  {
    checkMe[i] = 'j';
  }
  fs.lseek_broken(1, 0);
  ASSERT_EQ(132, fs.read(1, buf, 800));
  ASSERT_STREQ(checkMe, buf);
}

TEST(FileSystem53, CanOverreadOneCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  char checkMe[8] = {'\0'};
  checkMe[0] = 'f';
  char buf[8] = {'\0'};
  ASSERT_EQ(1, fs.write(index, 'f', 1));
  fs.lseek_broken(0, 0);
  ASSERT_EQ(1, fs.read(index, buf, 8));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(0, 0);
  ASSERT_EQ(1, fs.write(index, 'j', 1));
  fs.lseek_broken(0, 0);
  checkMe[0] = 'j';
  ASSERT_EQ(1, fs.read(index, buf, 8));
  ASSERT_STREQ(checkMe, buf);
}

TEST(FileSystem53, CanOverreadOneCharToOpenFileIfAllThreeAreOpen)
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
  fs.lseek_broken(0, 0);
  char buf[8] = {'\0'};
  char checkMe[8] = {'\0'};
  checkMe[0] = 'f';
  ASSERT_EQ(1, fs.read(0, buf, 8));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(1, 0);
  checkMe[0] = 'j';
  ASSERT_EQ(1, fs.read(1, buf, 8));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(2, 0);
  checkMe[0] = 'm';
  ASSERT_EQ(1, fs.read(2, buf, 8));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(0, 0);
  fs.lseek_broken(1, 0);
  fs.lseek_broken(2, 0);
  ASSERT_EQ(1, fs.write(0, 'l', 1));
  ASSERT_EQ(1, fs.write(1, 'h', 1));
  ASSERT_EQ(1, fs.write(2, 'k', 1));
  fs.lseek_broken(0, 0);
  checkMe[0] = 'l';
  ASSERT_EQ(1, fs.read(0, buf, 8));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(1, 0);
  checkMe[0] = 'h';
  ASSERT_EQ(1, fs.read(1, buf, 8));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(2, 0);
  checkMe[0] = 'k';
  ASSERT_EQ(1, fs.read(2, buf, 8));
  ASSERT_STREQ(checkMe, buf);
}

TEST(FileSystem53, CanOverreadMoreThanOneBlockOfCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  ASSERT_EQ(1, fs.write(index, 'f', 66));
  fs.lseek_broken(0, 0);
  char buf[256] = {'\0'};
  char checkMe[256] = {'\0'};
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'f';
  }
  ASSERT_EQ(66, fs.read(index, buf, 800));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(0, 0);
  ASSERT_EQ(1, fs.write(index, 'k', 66));
  fs.lseek_broken(0, 0);
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'k';
  }
  ASSERT_EQ(66, fs.read(index, buf, 800));
  ASSERT_STREQ(checkMe, buf);
}

TEST(FileSystem53, CanOverreadMoreThanOneBlockOfCharToOpenFileIfAllThreeAreOpen)
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
  fs.lseek_broken(0, 0);
  char buf[256] = {'\0'};
  char checkMe[256] = {'\0'};
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'f';
  }
  ASSERT_EQ(66, fs.read(0, buf, 800));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(1, 0);
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'j';
  }
  ASSERT_EQ(66, fs.read(1, buf, 800));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(2, 0);
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'm';
  }
  ASSERT_EQ(66, fs.read(2, buf, 800));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(0, 0);
  fs.lseek_broken(1, 0);
  fs.lseek_broken(2, 0);
  ASSERT_EQ(1, fs.write(0, 'h', 66));
  ASSERT_EQ(1, fs.write(1, 'i', 66));
  ASSERT_EQ(1, fs.write(2, 'l', 66));
  fs.lseek_broken(0, 0);
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'h';
  }
  ASSERT_EQ(66, fs.read(0, buf, 800));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(1, 0);
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'i';
  }
  ASSERT_EQ(66, fs.read(1, buf, 800));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(2, 0);
  for(int i = 0; i < 66; ++i)
  {
    checkMe[i] = 'l';
  }
  ASSERT_EQ(66, fs.read(2, buf, 800));
  ASSERT_STREQ(checkMe, buf);
}

TEST(FileSystem53, CanResumeOverreadOneCharToOpenFile)
{
  FileSystem53 fs;
  fs.create("Test.txt");
  int index = fs.open("Test.txt");
  ASSERT_EQ(1, fs.write(index, 'f', 1));
  ASSERT_EQ(1, fs.write(index, 'h', 2));
  fs.lseek_broken(0, 0);
  char buf[8] = {0};
  char checkMe[8] = {0};
  checkMe[0] = 'f';
  checkMe[1] = 'h';
  checkMe[2] = 'h';
  ASSERT_EQ(3, fs.read(index, buf, 20));
  ASSERT_STREQ(checkMe, buf);
  fs.lseek_broken(0, 0);
  ASSERT_EQ(1, fs.write(index, 'k', 1));
  ASSERT_EQ(1, fs.write(index, 'l', 2));
  fs.lseek_broken(0, 0);
  checkMe[0] = 'k';
  checkMe[1] = 'l';
  checkMe[2] = 'l';
  ASSERT_EQ(3, fs.read(index, buf, 20));
  ASSERT_STREQ(checkMe, buf);
}

// Not implemented tests
TEST(FileSystem53, CanNotCreateTwoFilesWithSameName) {}
TEST(FileSystem53, CanNotOpenFileNotCreated) {}
