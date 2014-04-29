#include "filesystem53.h"
#include <string.h>
FileSystem53::FileSystem53() : directoryIndex(7) { format(); }

void FileSystem53::OpenFileTable() {}

int FileSystem53::findOft() {}

void FileSystem53::deallocateOft(int index) {}

// If you accept my create(), you must accept these changes!
void FileSystem53::format()
{
  char bytemap[B];
  for(int i = 0; i < 8; ++i)
  {
    bytemap[i] = 1;
  }
  for(int i = 8; i < B; ++i)
  {
    bytemap[i] = 0;
  }
  io.write_block(0, bytemap);
  // Fill emptyFileDescriptors with all -1. -1 is not used.
  char emptyFileDescriptors[B];
  for(int i = 0; i < B; ++i)
  {
    emptyFileDescriptors[i] = -1;
  }
  for(int i = 1; i < 7; ++i)
  {
    io.write_block(i, emptyFileDescriptors);
  }
  // Create directory file
  emptyFileDescriptors[0] = 0;
  io.write_block(directoryIndex, emptyFileDescriptors);
}

char *FileSystem53::readDescriptor(int no) {}

void FileSystem53::clearDescriptor(int no) {}

void FileSystem53::writeDescriptor(int no, char *desc) {}

int FileSystem53::findEmptyDescriptor() {}

int FileSystem53::findEmptyBlock() {}

int FileSystem53::fgetc(int index) {}

int FileSystem53::fputc(int c, int index) {}

bool FileSystem53::feof(int index) {}

int FileSystem53::searchDir(int index, const std::string &fileName) {}

int FileSystem53::create(const std::string &fileName)
{
  char newIndex = -1;
  char possibleIndex = 0;
  char descriptorBlock[B];
  // Loop through all possible descriptor slots to find an empty index.
  for(int i = 1; i < 7; ++i)
  {
    io.read_block(i, descriptorBlock);
    // Each descriptor is 4 chars wide.
    for(int j = 0; j < B; j += 4, ++possibleIndex)
    {
      // If this descriptor is unused (-1)
      if(descriptorBlock[j] == -1)
      {
        newIndex = possibleIndex;
        break;
      }
    }
    if(newIndex != -1)
    {
      // Double for-loops mean double breaks
      break;
    }
  }
  if(newIndex == -1)
  {
    return 1;
  }
  // The directory's descriptor block.
  char directoryDescriptor[B];
  io.read_block(directoryIndex, directoryDescriptor);
  // Go through blocks 1,2,3 and look for our file name
  // Or an opening to add to.
  for(int i = 1; i < 4; ++i)
  {
    if(directoryDescriptor[0] < i)
    {
      // If number of blocks for directory allocated is too small, create one.
      directoryDescriptor[i] = addBlock();
      if(directoryDescriptor[i] == -1)
      {
        return -1;
      }
      // Increase number of blocks, and write it to disk.
      ++directoryDescriptor[0];
      io.write_block(directoryIndex, directoryDescriptor);

      // As we allocated a new block, we found our position, it's at the start.
      // Load the newly added directoryBlock, so we can mark it used.
      char directoryBlock[B];
      io.read_block(directoryDescriptor[i], directoryBlock);
      memset(directoryBlock, 0, B);

      // Copy the filename's data to the start of the directory block
      strncpy(directoryBlock, fileName.c_str(), MAX_FILE_NAME_LEN);
      // Indicate where our file descriptor is.
      directoryBlock[MAX_FILE_NAME_LEN] = newIndex;
      // Write it to disk
      io.write_block(directoryDescriptor[i], directoryBlock);

      // Set our file descriptor to used, and write it.
      char fileIndex = newIndex % (B / 4);
      descriptorBlock[4 * fileIndex] = 0;
      char REMOVEME = 1 + (newIndex / B / 4);
      io.write_block(1 + newIndex / 4, descriptorBlock);

      return 0;
    }
    // The block from ldisk that contains names.
    char directoryBlock[B];
    io.read_block(directoryDescriptor[i], directoryBlock);
    // Each directory entry is MAX_FILE_NAME_LEN+1 long.
    // Go through each entry, find either duplicate names or a free spot.
    for(int j = 0; j < B; j += MAX_FILE_NAME_LEN + 1)
    {
      if(directoryBlock[j] == -1)
      {
        // If the directoryBlock's name starts with null, it's free.
        // Copy the filename's data to the start of the directory block
        // Offset by j characters
        strncpy(directoryBlock + j, fileName.c_str(), MAX_FILE_NAME_LEN);
        // Indicate where our file descriptor is.
        // Put it right after our name.
        directoryBlock[j + MAX_FILE_NAME_LEN] = newIndex;
        // Write it to disk
        io.write_block(directoryDescriptor[i], directoryBlock);

        // Set our file descriptor to used, and write it.
        char fileIndex = newIndex % (B / 4);
        descriptorBlock[4 * fileIndex] = 0;
        const char REMOVEME = 1 + (newIndex / B / 4);
        io.write_block(1 + (newIndex / B / 4), descriptorBlock);

        return 0;
      }
      // if duplicate, return -2
      else if(strncmp(directoryBlock + j, fileName.c_str(),
                      MAX_FILE_NAME_LEN) == 0)
      {
        return -2;
      }
    }
  }
  // If we could not find an opening, no more space!
  return -1;
}

int FileSystem53::openDesc(int desc_no) {}

int FileSystem53::open(const std::string &fileName)
{
  // Get the file descriptor for the director
  char directoryDescriptor[B];
  io.read_block(directoryIndex, directoryDescriptor);
  // Go block by block from the directory.
  for(int i = 1; i <= directoryDescriptor[0]; ++i)
  {
    char directoryBlock[B];
    io.read_block(directoryDescriptor[i], directoryBlock);
    for(int j = 0; j < B; j += MAX_FILE_NAME_LEN + 1)
    {
      if(strncmp(directoryBlock + j, fileName.c_str(), MAX_FILE_NAME_LEN) == 0)
      {
        // Found it, now add it to the OFT if possible.
        bool found = false;
        for(int i = 0; i < 3; ++i)
        {
          if(!table.open[i])
          {
            found = true;
            memset(table.table[i].buf, 0, B);
            table.table[i].pos = 0;
            table.table[i].index = directoryBlock[j + MAX_FILE_NAME_LEN];
            table.open[i] = true;
            return i;
          }
        }
        if(!found)
        {
          return -2;
        }
      }
    }
  }
  return -1;
}
/*    Actual number of bytes returned in mem_area[].
 *    -1 value for error case "File hasn't been open"
 *    -2 value for error case "End-of-file"
 TODOs:
     1. Read the open file table using index.
        1.1 Get the file descriptor number and the current position.
        1.2 Can't get proper file descriptor, return -1.
     2. Read the file descriptor
        2.1 Get file size and block array.
     3. Read 'count' byte(s) from file and store in mem_area[].
        3.1 If current position crosses block boundary, call read_block() to
 read the next block.
        3.2 If current position==file size, stop reading and return.
        3.3 If this is called when current position==file size, return -2.
        3.4 If count > mem_area size, only size of mem_area should be read.
        3.5 Returns actual number of bytes read from file.
        3.6 Update current position so that next read() can be done from the
 first byte haven't-been-read.
  */
int FileSystem53::read(int index, char *memArea, int count)
{
  // If the index is negative, too big, or not open
  if(index < 0 || index > MAX_OPEN_FILE || !table.open[index])
  {
    return -1;
  }
  // If we're being asked to write a negative number of items, return 0.
  if(count < 0)
  {
    return 0;
  }
  int numberRead = 0;
  // The index of the file descriptor, not just in this block.
  const int totalFileIndex = table.table[index].index;
  // The block which holds the file descriptor
  const int descriptorIndex = 1 + ((totalFileIndex * 4) / B);
  char fileDescriptor[B];
  io.read_block(descriptorIndex, fileDescriptor);
  // The index of the file descriptor in the loaded block.
  const int fileIndex = totalFileIndex % (B / 4);

  // The position relative to buf. (180 would be 52 within block 2)
  int relativePos = table.table[index].pos % B;
  // Which block we're in (1 to 3)
  int currentBlock = 1 + (table.table[index].pos / B);
  int maxRead = (unsigned char)fileDescriptor[(fileIndex * 4)];

  // If we don't have a block, return 0;
  if(!fileDescriptor[fileIndex * 4])
  {
    return 0;
  }

  for(int i = 0; i < count; ++i, ++relativePos, ++numberRead)
  {
    if(numberRead > count || numberRead > maxRead)
    {
      --numberRead;
      break;
    }
    // If we're at or past a block boundary, load a new one, or stop.
    if(relativePos >= B)
    {
      // Write what we have to disk. (In the event it was modified. We COULD add
      // a dirty bit(/byte) to only write if needed.
      io.write_block(fileDescriptor[(fileIndex * 4) + currentBlock],
                     table.table[index].buf);
      ++currentBlock;
      // If currentBlock is higher than the length of the file, stop.
      if(currentBlock > (unsigned char)fileDescriptor[fileIndex * 4])
      {
        ++table.table[index].pos += numberRead;
        return numberRead;
      }
      else
      {
        // Read that block from disk.
        io.read_block(fileDescriptor[(fileIndex * 4) + currentBlock],
                      table.table[index].buf);
      }
      // Reset relativePos to be B places back.
      relativePos -= B;
    }
    memArea[numberRead] = table.table[index].buf[relativePos];
  }
  // Once we're done, set the full position to be count higher.
  table.table[index].pos += numberRead;
  return numberRead;
}

/* File Write function:
 *    This writes 'count' number of 'value'(s) to the file indicated by index.
 *    Writing should start from the point pointed by current position of the
 * file.
 *    Current position should be updated accordingly.
 * Parameter(s):
 *    index: File index which indicates the file to be read.
 *    value: a character to be written.
 *    count: Number of repetition.
 * Return:
 *    >0 for successful write
 *    -1 value for error case "File hasn't been open"
 *    -2 for error case "Maximum file size reached" (not implemented.)
 */
int FileSystem53::write(int index, char value, int count)
{
  // If the index is negative, too big, or not open
  if(index < 0 || index > MAX_OPEN_FILE || !table.open[index])
  {
    return -1;
  }
  // If we're being asked to write a negative number of items, return 0.
  if(count < 0)
  {
    return 0;
  }
  if(count == 0)
  {
    return 1;
  }
  // If we're being asked to write too many items.
  if(count + table.table[index].pos > B * 3)
  {
    return -2;
  }
  // The index of the file descriptor, not just in this block.
  const int totalFileIndex = table.table[index].index;
  // The block which holds the file descriptor
  const int descriptorIndex = 1 + ((totalFileIndex * 4) / B);
  char fileDescriptor[B];
  io.read_block(descriptorIndex, fileDescriptor);
  // The index of the file descriptor in the loaded block.
  const int fileIndex = totalFileIndex % (B / 4);

  // The position relative to buf. (180 would be 52 within block 2)
  int relativePos = table.table[index].pos % B;
  // Which block we're in (1 to 3)
  int currentBlock = 1 + (table.table[index].pos / B);

  // If we don't have a block, get one before writing
  if(!fileDescriptor[fileIndex * 4])
  {
    fileDescriptor[(fileIndex * 4) + 1] = addBlock();
    io.write_block(descriptorIndex, fileDescriptor);
  }

  for(int i = 0; i < count; ++i, ++relativePos)
  {
    // If we're at or past a block boundary, load (/create) a new one.
    if(relativePos >= B)
    {
      // Write what we have to disk.
      io.write_block(fileDescriptor[(fileIndex * 4) + currentBlock],
                     table.table[index].buf);
      ++currentBlock;
      // If currentBlock is higher than the length of the file, create a block.
      if(currentBlock * B > (unsigned char)fileDescriptor[fileIndex * 4])
      {
        fileDescriptor[(fileIndex * 4) + currentBlock] = addBlock();
        io.write_block(descriptorIndex, fileDescriptor);
        memset(table.table[index].buf, 0, B);
      }
      else
      {
        // Read that block from disk.
        io.read_block(fileDescriptor[(fileIndex * 4) + currentBlock],
                      table.table[index].buf);
      }
      // Reset relativePos to be B places back.
      relativePos -= B;
    }
    table.table[index].buf[relativePos] = value;
  }
  // Once we're done, set the full position to be count higher.
  table.table[index].pos += count;
  if(table.table[index].pos > (unsigned char)fileDescriptor[fileIndex * 4])
  {
    fileDescriptor[fileIndex * 4] = table.table[index].pos;
    io.write_block(descriptorIndex, fileDescriptor);
  }
  return 1;
}

// HAS NO ERROR CHECKING DO NOT USE IN PRODUCTION!!!!! SEGFAULTS WILL OCCUR!!!!!
int FileSystem53::lseek(int index, int pos)
{
  const int oldBlock = 1 + (table.table[index].pos / B);
  table.table[index].pos = pos;
  const int currentBlock = 1 + (table.table[index].pos / B);
  if(oldBlock != currentBlock)
  {
    char fileDescriptor[B];
    const int totalFileIndex = table.table[index].index;
    const int fileIndex = totalFileIndex % (B / 4);
    const int descriptorIndex = 1 + ((totalFileIndex * 4) / B);
    io.read_block(descriptorIndex, fileDescriptor);
    // Write what we have to disk.
    io.write_block(fileDescriptor[(fileIndex * 4) + oldBlock],
                   table.table[index].buf);
    io.read_block(fileDescriptor[(fileIndex * 4) + currentBlock],
                  table.table[index].buf);
  }
}

void FileSystem53::close(int index) {}

int FileSystem53::deleteFile(const std::string &fileName) {}

void FileSystem53::directory() {}

void FileSystem53::restore(const std::string &name) { io.load(name); }

void FileSystem53::save(const std::string &name) { io.save(name); }

void FileSystem53::diskDump(int start, int size) {}

int FileSystem53::addBlock()
{
  char bytemap[B];
  io.read_block(0, bytemap);
  for(int i = 0; i < B; ++i)
  {
    if(!bytemap[i])
    {
      bytemap[i] = 1;
      io.write_block(0, bytemap);
      return i;
    }
  }
  return -1;
}

FileSystem53::OFT::OFT()
{
  open[0] = false;
  open[1] = false;
  open[2] = false;
}
