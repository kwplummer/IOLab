/*
 *
//============================================================================
// Description : ICS 53 Part 2
//============================================================================

#include <iostream>
#include "filesystem53.h"
#include <cassert>
#include <string>

using namespace std;
string booleanToString(bool result);

int main()
{
  cout << "ICS 53 Part 2 Testing" << endl << endl;
  cout << "Initiating Tests....." << endl;

  FileSystem53 fs;

  cout << "Testing directory() which should only contain 1 file: the directory "
          "file itself as an entry: " << endl;
  cout << "NOTE: The name of the directory file is '__________' (we mean 11 "
          "underscore characters)" << endl;
  fs.directory();
  cout << endl << endl;

  cout << "Testing create(Test.txt) = "
       << booleanToString((0 == fs.create("Test.txt"))) << endl;
  cout << "Testing open(Test.txt) = "
       << booleanToString((0 == fs.open("Test.txt"))) << endl;
  cout << "Testing create(Test2.txt) = "
       << booleanToString((0 == fs.create("Test2.txt"))) << endl;
  cout << "Testing open(Test2.txt) = "
       << booleanToString((1 == fs.open("Test2.txt"))) << endl;
  cout << "Testing create(Test3.txt) = "
       << booleanToString((0 == fs.create("Test3.txt"))) << endl;
  cout << "Testing open(Test3.txt) = "
       << booleanToString((2 == fs.open("Test3.txt"))) << endl;
  fs.listDesc();
  cout << "Testing write(0, 'f', 1) = "
       << booleanToString((1 == fs.write(0, 'f', 1))) << endl;
  fs.listDesc();
  cout << "Testing write(1, 'j', 1) = "
       << booleanToString((1 == fs.write(1, 'j', 1))) << endl;
  fs.listDesc();
  cout << "Testing write(2, 'm', 1) = "
       << booleanToString((1 == fs.write(2, 'm', 1))) << endl;
  cout << "Testing write(1, 'j', 2) = "
       << booleanToString((1 == fs.write(1, 'j', 2))) << endl;
  cout << "Testing write(2, 'm', 2) = "
       << booleanToString((1 == fs.write(2, 'm', 2))) << endl;
  cout << "Testing write(0, 'f', 2) = "
       << booleanToString((1 == fs.write(0, 'f', 2))) << endl;
  cout << "Testing lseek(0, 0) = " << booleanToString((0 == fs.lseek(0, 0)))
       << endl;

  char buf[66] = {'\0'};
  char checkMe[66] = {'\0'};
  checkMe[0] = 'f';
  checkMe[1] = 'f';
  checkMe[2] = 'f';

  cout << "Testing read(0, buf, 10) = "
       << booleanToString((3 == fs.read(0, buf, 10))) << endl;
  cout << "Testing value of buf = " << booleanToString((checkMe[3] == buf[3]))
       << endl;
  cout << "Testing lseek(1, 0) = " << booleanToString(0 == fs.lseek(1, 0))
       << endl;

  checkMe[0] = 'j';
  checkMe[1] = 'j';
  checkMe[2] = 'j';

  cout << "Testing read(1, buf, 10) = "
       << booleanToString((3 == fs.read(1, buf, 10))) << endl;
  cout << "Testing value of buf = " << booleanToString((checkMe[1] == buf[1]))
       << endl;
  cout << "Testing lseek(2, 0) = " << booleanToString(0 == fs.lseek(2, 0))
       << endl;

  checkMe[0] = 'm';
  checkMe[1] = 'm';
  checkMe[2] = 'm';

  cout << "Testing read(2, buf, 10) = "
       << booleanToString((3 == fs.read(2, buf, 10))) << endl;
  cout << "Testing value of buf = " << booleanToString((checkMe[2] == buf[2]))
       << endl;

  cout << "Testing directory() which should contain 4 files: the directory "
          "file itself is part of the entries: " << endl;
  fs.directory();
  cout << endl << endl;
  fs.close(0);
  cout << "Testing destroy(Test.txt) = "
       << booleanToString((0 == fs.destroy("Test.txt"))) << endl;
  fs.close(2);
  cout << "Testing destroy(Test.txt) = "
       << booleanToString((-1 == fs.destroy("Test.txt"))) << endl;
  cout << "Final List of Directories:" << endl;
  fs.directory();
  cout << endl << endl;

  fs.create("Blah.txt");
  fs.directory();
  int fileIndex = fs.open("Blah.txt");
  if(fileIndex >= 0)
  {
    fs.write(fileIndex, 'g', 10);
  }
  fs.directory();
  fs.lseek(fileIndex, 0);
  cout << fs.read(fileIndex, buf, 100) << endl;
  cout << "Read " << buf << std::endl;
  fs.directory();

  cout << fs.create("HUUUUUU.txtJEUEJERHEKRJELR") << endl;
  // fs.close(fileIndex);
  cout << "What is in OFT " << fileIndex << fs.getFileName(fileIndex)
       << " should be blah.txt " << endl;
  cout << "Destroying blah " << fs.destroy("Blah.txt") << endl;
  fs.close(0);
  cout << fs.destroy("Test.txt") << endl;
  fs.close(1);
  cout << fs.destroy("Test3.txt") << endl;
  fs.close(2);
  cout << fs.destroy("Test2.txt") << endl;
  cout << fs.destroy("Test1.txt") << endl;
  cout << "FINAL DIRECTORY LIST" << endl;
  fs.directory();
  return 0;
}

string booleanToString(bool result) { return (result ? "PASS" : "FAIL"); }
*/

#include "PresentationShell.h"
int main()
{
  PresentationShell sh;
  sh.runPresentationShell();
}
