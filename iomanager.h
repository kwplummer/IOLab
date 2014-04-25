#ifndef IOMANAGER_H
#define IOMANAGER_H
#include <string>
class IOManager
{
  static const int L = 64;
  static const int B = 64;
  char ldisk[L][B];

public:
  void read_block(int i, char *p) const;
  void write_block(int i, char *p);
  void load(const std::string &name);
  void save(const std::string &name) const;
};

#endif // IOMANAGER_H
