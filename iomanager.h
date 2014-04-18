#ifndef IOMANAGER_H
#define IOMANAGER_H

class IOManager
{
  static const int L=64;
  static const int B=64;
  char ldisk[L][B];
public:
  void read_block(int i, char *p);
  void write_block(int i, char *p);
  void load();
  void save();
};

#endif // IOMANAGER_H
