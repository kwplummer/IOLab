#include "iomanager.h"
#include <string.h>

IOManager::IOManager()
{
}

void IOManager::read_block(int i, char *p)
{

}

void IOManager::write_block(int i, char *p)
{
  strncpy(ldisk[i],p,B);
}

void IOManager::load()
{

}

void IOManager::save()
{

}
