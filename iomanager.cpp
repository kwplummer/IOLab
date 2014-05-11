#include "iomanager.h"
#include <string.h>
#include <fstream>
#include <iostream>

void IOManager::read_block(int i, char *p) const { mempcpy(p, ldisk[i], B); }

void IOManager::write_block(int i, char *p) { mempcpy(ldisk[i], p, B); }

bool IOManager::load(const std::string &name)
{
  std::ifstream in(name.c_str());
  if(in.fail())
  {
    return false;
  }
  else
  {
	std::string rawFileInput;

    for(int i = 0; i < L; ++i)
    {
        getline(in,rawFileInput);
        for( int j = 0 ; j < B ; ++j ){
      	  ldisk[i][j] = rawFileInput[j];
        }
    }

    in.close();
    return true;
  }
}

void IOManager::save(const std::string &name) const
{
  std::ofstream out(name.c_str());
  for(int i = 0; i < L; ++i)
  {
    out << ldisk[i] << '\n';
  }
}
