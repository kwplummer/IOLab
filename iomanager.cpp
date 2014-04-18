#include "iomanager.h"
#include <string.h>
#include <fstream>
#include <iostream>

void IOManager::read_block(int i, char *p)
{
  strncpy(p,ldisk[i],B);
}

void IOManager::write_block(int i, char *p)
{
  strncpy(ldisk[i],p,B);
}

void IOManager::load()
{
  std::ifstream in("OutFile.txt");
  if(in.fail())
  {
    std::cout << "Is not open.\n";
  }
  else
  {
    for(int i=0;i<L;++i)
    {
      in.getline(ldisk[i],B);
      std::cout << ldisk[i] << '\n';
    }
    in.close();
  }
}

void IOManager::save()
{
  std::ofstream out("OutFile.txt");
  for(int i=0;i<L;++i)
  {
    out << ldisk[i] << '\n';
  }
}
