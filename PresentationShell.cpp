#include "PresentationShell.h"
#include <sstream>
PresentationShell::PresentationShell() { runbit = true; }

void PresentationShell::runPresentationShell()
{
  do
  {
    std::string inputString;
    std::string command, name;
    int index, count, pos;
    char writeChar;

    int commandValue;
    std::cout << "fileSystem53>";
    std::getline(std::cin, inputString);
    std::istringstream in(inputString);
    in >> command;
    commandValue = commandSwitch(command);

    switch(commandValue)
    {
    case 1: // cr (create)
    {
      in >> name;
      processCreate(name);
      break;
    }
    case 2: // de (destroy)
    {
      in >> name;
      processDestroy(name);
      break;
    }
    case 3: // op (open)
    {
      in >> name;
      processOpen(name);
      break;
    }
    case 4: // cl (close)
    {
      in >> index;
      processClose(index);
      break;
    }
    case 5: // rd (read)
    {
      in >> index >> count;
      processRead(index, count);
      break;
    }
    case 6: // wr (write)
    {
      in >> index >> writeChar >> count;
      processWrite(index, writeChar, count);
      break;
    }
    case 7: // sk (current position)
    {
      in >> index >> pos;
      processSk(index, pos);
      break;
    }
    case 8: // dr (lists files)
    {
      fs.directory();
      break;
    }
    case 9: // in (initialize)
    {
      processInitiate();
      break;
    }
    case 10: // sv (save)
    {
      fs.save();
      std::cout << "disk saved\n";
      break;
    }
    case 11:
    {
      runbit = false;
      break;
    }
    default:
    {
      std::cerr << "Error: Invalid command\n";
    }
    }
  } while(runbit);
}

int PresentationShell::commandSwitch(std::string command)
{
  if(command == "cr")
  {
    return 1;
  }
  else if(command == "de")
  {
    return 2;
  }
  else if(command == "op")
  {
    return 3;
  }
  else if(command == "cl")
  {
    return 4;
  }
  else if(command == "rd")
  {
    return 5;
  }
  else if(command == "wr")
  {
    return 6;
  }
  else if(command == "sk")
  {
    return 7;
  }
  else if(command == "dr")
  {
    return 8;
  }
  else if(command == "in")
  {
    return 9;
  }
  else if(command == "sv")
  {
    return 10;
  }
  else if(command == "q")
  {
    return 11;
  }
  else
  {
    return -1;
  }
}

void PresentationShell::processCreate(std::string fileName)
{
  int createResult = fs.create(fileName);
  if(createResult == 0)
  {
    std::cout << "file " << fileName << " created" << std::endl;
  }
  else if(createResult == -1)
  {
    std::cerr << "Error: File creation failed: no space available" << std::endl;
  }
  else if(createResult == -2)
  {
    std::cerr << "Error: File creation failed: file name already exists"
              << std::endl;
  }
  else
  {
    std::cerr << "Error: File creation failed" << std::endl;
  }
}

void PresentationShell::processDestroy(std::string fileName)
{
  int deleteResult = fs.deleteFile(fileName);

  if(deleteResult == 0)
  {
    std::cout << "file " << fileName << " deleted" << std::endl;
  }
  else
  {
    std::cerr << "Error: File deletion failed" << std::endl;
  }
}

void PresentationShell::processOpen(std::string fileName)
{
  int openResult = fs.open(fileName);

  if(openResult < 0)
  {
    std::cerr << "Error: File cannot be opened" << std::endl;
  }
  else if(openResult == -400)
  {
    std::cout << "Error: file " << fileName << " is already opened.\n";
  }
  else
  {
    std::cout << "file " << fileName
              << " opened, index = " << adjustSystemOutputIndex(openResult)
              << std::endl;
  }
}
void PresentationShell::processClose(int index)
{
  index = adjustUserInputIndex(index);
  std::string fileName = fs.getFileName(index);
  int closeResult = fs.close(index);

  if(closeResult == 0)
  {
    std::cout << "file " << fileName << " closed" << std::endl;
  }
  else
  {
    std::cerr << "Error: File cannot be closed" << std::endl;
  }
}

void PresentationShell::processRead(int index, int count)
{
  index = adjustUserInputIndex(index);
  char *readArray;
  readArray = new char[count];
  int charactersRead;

  if(count > 0)
  {
    charactersRead = fs.read(index, readArray, count);
    if(charactersRead > 0)
    {
      std::cout << charactersRead << " bytes read: ";
      for(int i = 0; i < charactersRead; i++)
      {
        std::cout << readArray[i];
      }
      std::cout << std::endl;
    }
    else
    {
      std::cerr << "Error: Read failed" << std::endl;
    }
  }
  else
  {
    std::cerr << "Error: Invalid argument. Must read more than 0 characters."
              << std::endl;
  }
}

void PresentationShell::processWrite(int index, char toWrite, int count)
{
  index = adjustUserInputIndex(index);
  int writeResult = fs.write(index, toWrite, count);
  if(writeResult > 0)
  {
    std::cout << count << " bytes written" << std::endl;
  }
  else
  {
    std::cerr << "Error: write failed" << std::endl;
  }
}

void PresentationShell::processSk(int index, int pos)
{
  index = adjustUserInputIndex(index);
  int result = fs.lseek(index, pos);
  if(result >= 0)
  {
    std::cout << "current position is " << result << '\n';
  }
  else if(result == -1)
  {
    std::cerr << "Error: File not open\n";
  }
  else if(result == -2)
  {
    std::cerr << "Error: Skipping fast end of file\n";
  }
  else if(result == -100)
  {
    std::cerr << "Error: Cannot seek to a negative position\n";
  }
}

void PresentationShell::processDr() {}
void PresentationShell::processInitiate()
{
  int result = fs.restore();

  if(result == 0)
  {
    std::cout << "disk restored\n";
  }
  else
  {
    std::cout << "disk initialized\n";
  }
}

void PresentationShell::processSave() {}

int PresentationShell::adjustUserInputIndex(int index)
{
  return index - OFT_INDEX_ADJUSTMENT_FACTOR;
}

int PresentationShell::adjustSystemOutputIndex(int index)
{
  return index + OFT_INDEX_ADJUSTMENT_FACTOR;
}

PresentationShell::~PresentationShell() {}
