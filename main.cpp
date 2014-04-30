
#include <iostream>
#include <assert.h>
#include <string.h>
#include "iomanager.h"
int main()
{
  std::cout << "Hello\n";
  IOManager io;
  io.load("Outfile.txt");
  io.write_block(0, "Hey");
  char *hey = new char[255];
  io.read_block(0, hey);
  std::cout << "Read\n";
  std::cout << hey << '\n';
  assert(strcmp("Hey", hey) == 0);
  io.save("Outfile.txt");
}
