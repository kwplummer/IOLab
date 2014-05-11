#ifndef PRESENTATIONSHELL_H
#define PRESENTATIONSHELL_H

#include <iostream>
#include <string>

#include "filesystem53.h"

class PresentationShell
{
public:
  PresentationShell();
  void runPresentationShell();
  int commandSwitch(std::string command);
  // commandSwitch - accepts user's command and converts it to an int
  // @param string command is the user's command
  // @return int - numerical equivalent to user's command
  void processCreate(std::string fileName);
  // processCreate - calls filesystem53's create function
  // @param string - fileName
  void processDestroy(std::string fileName);
  // processDestroy - calls filesystem53's delete function
  // @param string - fileName
  void processOpen(std::string fileName);
  // processOpen - calls filesystem 53's open function
  // @param string - fileName
  void processClose(int index);

  void processRead(int index, int count); // rd index count
  void processWrite(int index, char toWrite, int count);
  void processSk(int index, int pos);
  void processDr();
  void processInitiate();
  void processSave();
  ~PresentationShell();

private:
  FileSystem53 fs;
  bool runbit;
  // if testscript will base indices on system output, make value 0,
  // otherwise, make value 1
  static const int OFT_INDEX_ADJUSTMENT_FACTOR = 1;
  int adjustUserInputIndex(int index);
  int adjustSystemOutputIndex(int index);
};
#endif
