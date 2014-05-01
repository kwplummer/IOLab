#include "filesystem53.h"
#include <string.h>
#include <iostream>
FileSystem53::FileSystem53() : directoryIndex(7) {
	// format the disk all the time per TA's instruction
	format();

	// restore ldisk contents
	restore(LDISK_FILE_NAME);

	// copy descriptor table from ldisk to descTable
	for( int i = START_INDEX_BLOCK_DESC_TABLE ; i <= END_INDEX_BLOCK_DESC_TABLE ; i++ ){
		io.read_block(i,descTable[i]);
	}
}

void FileSystem53::OpenFileTable() {}

int FileSystem53::findOft() {}

void FileSystem53::deallocateOft(int index) {}

// If you accept my create(), you must accept these changes!
// My create depends on file descriptor sizes being -1 until created.
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

// variable "no" is expected to be a 0 based file descriptor index
// However, since the file descriptors are 4 bytes each
// we need to calculate the offset by 4 bytes
// Therefore,
// input of 0 means file descriptor index 0
// input of 1 means file descriptor index 4
// input of 2 means file descriptor index 8
// etc.
char *FileSystem53::readDescriptor(int no)
{

  char *fileDescriptor = new char[4];

  int rawFileDescriptorIndexOffset = no * 4;
  int rawFileDescriptorIndexA = (rawFileDescriptorIndexOffset / B) + 1;
  int rawFileDescriptorIndexB = rawFileDescriptorIndexOffset % B;

  fileDescriptor[0] =
      descTable[rawFileDescriptorIndexA][rawFileDescriptorIndexB++];
  fileDescriptor[1] =
      descTable[rawFileDescriptorIndexA][rawFileDescriptorIndexB++];
  fileDescriptor[2] =
      descTable[rawFileDescriptorIndexA][rawFileDescriptorIndexB++];
  fileDescriptor[3] =
      descTable[rawFileDescriptorIndexA][rawFileDescriptorIndexB];

  return fileDescriptor;
}

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
  // Get the file descriptor for the directory
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

int FileSystem53::lseek(int index, int pos)
{
  // S/0 VERIFY FILE IS OPEN
  /*
  bool openFileTableIndex = -1;

  // look for index among the open files
  for( int i = 0 ; i < MAX_OPEN_FILE ; i++ ){
  // if there is an open file in OpenFile index i
  // and it has the same file descriptor index as the input,
  // then the file is open
  if( table.open[i] && table.table[i].index == index ){
  openFileTableIndex = i;
  break;
  }
  }

  // if the file index being referenced is not open,
  // return error code for File Not Open
  if( openFileTableIndex == -1 ){
  return EC_FILE_NOT_OPEN;
  }
  */

  if(!table.open[index])
  {
    return EC_FILE_NOT_OPEN;
  }

  // E/O VERIFY FILE IS OPEN

  // if the position being requested to move to is negative
  // return error code for negative seek position
  if(pos < 0)
  {
    return EC_NEGATIVE_SEEK_POSITION;
  }

  // go to the file descriptor of this file to obtain the file size
  // int fileDescriptorIndex = table.table[openFileTableIndex].index;
  // char *fileDescriptor = readDescriptor(fileDescriptorIndex);
  char *fileDescriptor = readDescriptor(table.table[index].index);

  // if the position being requested goes beyond file size,
  // set position to the file size
  if(pos > static_cast<unsigned char>(fileDescriptor[FD_FILE_SIZE]))
  {
    pos = fileDescriptor[FD_FILE_SIZE];
  }

  const int oldBlock = 1 + (table.table[index].pos / B);
  table.table[index].pos = pos;
  const int currentBlock = 1 + (table.table[index].pos / B);
  if(oldBlock != currentBlock)
  {
    // Write what we have to disk.
    io.write_block(
        static_cast<int>(static_cast<unsigned char>(fileDescriptor[oldBlock])),
        table.table[index].buf);

    // Read the "new" current block from disk
    io.read_block(static_cast<int>(
                      static_cast<unsigned char>(fileDescriptor[currentBlock])),
                  table.table[index].buf);
  }

  return 0;
}

void FileSystem53::lseek_broken(int index, int pos)
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

void FileSystem53::directory()
{
  // get the file descriptor of the directory file
  char *directoryFileDescriptor =
      readDescriptor(FD_DIRECTORY_FILE_DESCRIPTOR_INDEX);

  int byteCount = 0;
  int directoryFileSize = static_cast<int>(
      static_cast<unsigned char>(directoryFileDescriptor[FD_FILE_SIZE]));
  int currentBlockIndex;
  char *currentBlock = new char[B];
  int i, j, k;
  char *fileDescriptor;
  int fileDescriptorIndex, fileSize;

  // if the directory file is empty
  if(directoryFileSize <= 0)
  {
    std::cout << "Directory File is empty!" << std::endl;
  }
  else
  {
    /*
     * IF THE DIRECTORY FILE NEEDS TO BE ON THE OFT
     * WE REGISTER IT HERE
     */

    // iterative over the blocks of the directory and output the data
    for(i = FD_FIRST_BLOCK;
        i <= FD_THIRD_BLOCK && byteCount < directoryFileSize; i++)
    {

      currentBlockIndex = static_cast<int>(
          static_cast<unsigned char>(directoryFileDescriptor[i]));

      // if the block specified by the file descriptor is valid
      if(currentBlockIndex > 0)
      {

        // get that block from disk
        io.read_block(currentBlockIndex, currentBlock);

        k = 0; // file name string counter

        // iterate over the files listed in this block
        for(j = 0; j < B && byteCount < directoryFileSize; j++)
        {
          // if this is a file descriptor index byte
          if(k == 10)
          {
            // File name display has been completed.
            // Next, get the file size
            fileDescriptorIndex =
                static_cast<int>(static_cast<unsigned char>(currentBlock[j]));
            fileDescriptor = readDescriptor(fileDescriptorIndex);

            std::cout << " " << static_cast<int>(static_cast<unsigned char>(
                                    fileDescriptor[FD_FILE_SIZE])) << " bytes";

            // if this is NOT the last file to be displayed
            if(byteCount + 1 < directoryFileSize)
            {
              // display comma
              std::cout << ", ";
            }

            k = 0;
          }
          else
          {
            if(currentBlock[j] != 0)
            {
              std::cout << currentBlock[j];
            }
            k++;
          }
          byteCount++;
        }
      }
    }
  }
}

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
