#include "iomanager.h"
#include <string.h>
#include <fstream>
#include <iostream>

void IOManager::read_block(int i, char *p) const
{
  mempcpy(p,ldisk[i],B);
}

void IOManager::write_block(int i, char *p)
{
  mempcpy(ldisk[i],p,B);
}

void IOManager::load(const std::string &name)
{
  std::ifstream in(name);
  if(in.fail())
  {
    std::cout << "Is not open.\n";
  }
  else
  {
    for(int i=0;i<L;++i)
    {
      in.getline(ldisk[i],B);
    }
    in.close();
  }
}

void IOManager::save(const std::string &name) const
{
  std::ofstream out(name);
  for(int i=0;i<L;++i)
  {
    out << ldisk[i] << '\n';
  }
}
