#include "filesystem53.h"
#include <string.h>
#include <iostream>
FileSystem53::FileSystem53() : directoryIndex(7), LDISK_FILE_NAME("ldisk.txt")
{
  // format the disk all the time per TA's instruction
  format();

  // restore ldisk contents
  restore(LDISK_FILE_NAME);

  // copy descriptor table from ldisk to descTable
  for(int i = START_INDEX_BLOCK_DESC_TABLE; i <= END_INDEX_BLOCK_DESC_TABLE;
      i++)
  {
    io.read_block(i, descTable[i]);
  }

  OpenFileTable();

  // per TA's instruction,
  // the directory should always be in the OFT
  // under a special "hidden" 4th slot (i.e. index 3).
  // Therefore, we have hard coded the hidden OFT index containing the
  // directory file as a constant
  char *fileDescriptor = readDescriptor(FD_DIRECTORY_FILE_DESCRIPTOR_INDEX);
  // if the directory file is available,
  if(fileDescriptor[FD_FILE_SIZE] >= 0)
  {

    char *dataBlock = new char[B];

    // if the directory file is NOT empty
    if(fileDescriptor[FD_FILE_SIZE] == 0)
    {
      // get the first block of the directory file from ldisk
      io.read_block(fileDescriptor[FD_FIRST_BLOCK], dataBlock);

      // else
    }
    else
    {
      // fill the data block will null values
      for(int i; i < B; ++i)
      {
        dataBlock[i] = -1;
      }
    }

    // load directory data block in the open file table
    initializeOFT(OFT_DIRECTORY_INDEX, dataBlock,
                  FD_DIRECTORY_FILE_DESCRIPTOR_INDEX);
  }
}

void FileSystem53::OpenFileTable()
{
  char blankData[B];
  int i;
  for(i = 0; i < B; ++i)
  {
    blankData[i] = 0;
  }

  for(i = 0; i < MAX_OPEN_FILE; ++i)
  {
    table.table[i].pos = 0;
    table.table[i].index = -1;
    mempcpy(table.table[i].buf, blankData, B);
  }
}

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
// However, since the file descriptors are 4 bytes each,
// we need to calculate the offset by 4 bytes
// Therefore,
// input of 0 means file descriptor index 0
// input of 1 means file descriptor index 4
// input of 2 means file descriptor index 8
// etc.
char *FileSystem53::readDescriptor(int no)
{
  char *fileDescriptor = new char[DESCR_SIZE];

  const int rawFileDescriptorIndexOffset = no * DESCR_SIZE;
  const int rawFileDescriptorIndexA = (rawFileDescriptorIndexOffset / B) + 1;
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

/* Write descriptor
 *   1. Update descriptor entry
 *   2. Mark bitmap
 *   3. Write back to disk
 * Parameter(s):
 *    no: Descriptor number to write
 *    desc: descriptor to write
 * Return:
 *    none
 */
void FileSystem53::writeDescriptor(int no, const char *desc)
{
  const int rawFileDescriptorIndexOffset = no * 4;
  const int rawFileDescriptorIndexBlockNumber =
      (rawFileDescriptorIndexOffset / B) + 1;
  int rawFileDescriptorIndexBlockPosition = rawFileDescriptorIndexOffset % B;

  // grab the descTable block
  char *descriptorBlock = descTable[rawFileDescriptorIndexBlockNumber];

  // modify descriptorBlock from index rawFileDescriptorIndexBlockPosition to
  // rawFileDescriptorIndexBlockPosition + 4
  for(int i = 0; i < 4; ++i)
  {
    descriptorBlock[rawFileDescriptorIndexBlockPosition] = desc[i];
    descTable[rawFileDescriptorIndexBlockNumber]
             [rawFileDescriptorIndexBlockPosition] = desc[i];
    ++rawFileDescriptorIndexBlockPosition;
  }

  // write back to disk
  io.write_block(rawFileDescriptorIndexBlockNumber, descriptorBlock);
}

int FileSystem53::findEmptyDescriptor() {}

int FileSystem53::findEmptyBlock() {}

int FileSystem53::fgetc(int index) {}

int FileSystem53::fputc(int c, int index) {}

bool FileSystem53::feof(int index) {}

int FileSystem53::searchDir(const std::string &fileName,
                            char *directoryDataMemArea)
{

  int fileDescriptorIndex;
  bool fileNameMatch;

  char *directoryData = new char[DIRECTORY_DATA_CHUNK_SIZE];
  std::string foundFileName;
  std::string xx;
  // perform a linear search for the file
  lseek(OFT_DIRECTORY_INDEX, 0);
  int directoryDataByteLocation = 0;

  int directoryDataSize =
      read(OFT_DIRECTORY_INDEX, directoryData, DIRECTORY_DATA_CHUNK_SIZE);

  // if directory file is empty
  if(directoryDataSize == 0)
  {
    return EC_FILE_NOT_FOUND;
  }

  while(directoryDataSize != EC_EOF)
  {
    foundFileName.clear();
    for(int i = 0; i < DIRECTORY_DATA_CHUNK_SIZE - 1 && directoryData[i] != -1;
        ++i)
    {
      foundFileName.insert(i, 1, directoryData[i]);
    }
    /*
    std::cout << "Direcotry Data " << directoryData << std::endl;
    for( int k = 0 ; k < DIRECTORY_DATA_CHUNK_SIZE ; k++ ){
            std::cout << directoryData[k];
    }
    std::cout << std::endl;
    std::cout << "Found File Name=" << foundFileName << std::endl;

    std::cout << "Position " << static_cast<int>(static_cast<unsigned char>(
    table.table[3].pos ) ) << std::endl;
    std::cin >> xx;
    */
    // if the found file name matches the input file name
    if(foundFileName == fileName)
    {

      // copy over directory data to memory area
      for(int i = 0; i < DIRECTORY_DATA_CHUNK_SIZE; ++i)
      {
        directoryDataMemArea[i] = directoryData[i];
      }
      delete[] directoryData;

      // return file descriptor index
      return directoryDataByteLocation;
    }
    directoryDataSize =
        read(OFT_DIRECTORY_INDEX, directoryData, DIRECTORY_DATA_CHUNK_SIZE);
    directoryDataByteLocation += DIRECTORY_DATA_CHUNK_SIZE;
  }

  delete[] directoryData;

  return EC_FILE_NOT_FOUND;
}

int FileSystem53::create(const std::string &fileName)
{
  // verify that no file already exists with the same file name
  char *directoryData = new char[DIRECTORY_DATA_CHUNK_SIZE];
  int directoryFileByteLocation = searchDir(fileName, directoryData);

  // we actually want the an error code of file not found for create()
  // to continue
  if(directoryFileByteLocation != EC_FILE_NOT_FOUND)
  {
    delete[] directoryData;
    return EC_DUPLICATE_FILE_NAME;
  }

  // find an empty file descriptor slot and BUT DO NOT fill it with anything
  // yet.
  // Just get the file descriptor index that you found ( between 0 - 64 )
  // CORE ASSUMPTION:
  // if the file size slot in a file descriptor index == -1,
  // then we have found a empty file descriptor
  char *fileDescriptor = new char[DESCR_SIZE];
  int fileDescriptorIndex = -1;
  for(int i = 0; i < B; ++i)
  {
    fileDescriptor = readDescriptor(i);

    // if the file size slot in a file descriptor index == -1,
    // then we have found a empty file descriptor
    if(static_cast<int>(fileDescriptor[FD_FILE_SIZE]) == -1)
    {
      fileDescriptorIndex = i;
      break;
    }
    delete[] fileDescriptor;
  }

  // if no empty file descriptor slot, found
  if(fileDescriptorIndex == -1)
  {
    delete[] fileDescriptor;
    delete[] directoryData;
    return EC_NO_SPACE_IN_DISK;
  }

  // look for an empty slot in the directory
  lseek(OFT_DIRECTORY_INDEX, 0);
  directoryData[DIRECTORY_DATA_CHUNK_SIZE - 1] = 1;
  int resultOfRead;
  int fileCount = 0;

  // To find an empty slot,
  // we are relying on the facts:
  // 1) file descriptor indices are integers 0 to 63
  // 2) file descriptor index of each directory data is always at the last
  // position of that data chunk
  // 3) read() will return an end-of-file if there is simply no more data that
  // can be stored
  // CORE ASSUMPTION: directory file is open all the time
  while(directoryData[DIRECTORY_DATA_CHUNK_SIZE - 1] >= 0)
  {
    delete[] directoryData;
    directoryData = new char[DIRECTORY_DATA_CHUNK_SIZE];
    resultOfRead =
        read(OFT_DIRECTORY_INDEX, directoryData, DIRECTORY_DATA_CHUNK_SIZE);

    // if max number of files allowed is already reached
    // (not including the directory file itself)
    // or
    // if directory file is full
    if(fileCount == MAX_FILE_NO || resultOfRead == EC_EOF)
    {
      delete[] directoryData;
      delete[] fileDescriptor;
      return EC_NO_SPACE_IN_DISK;
    }

    ++fileCount;

    // directory file is completely empty
    if(resultOfRead == 0 && fileCount == 1)
    {
      break;
    }
  }

  // if we reached this point, out file count is over by 1 since it is counting
  // the empty directory data slot
  // thus, we correct it
  --fileCount;

  // if max number of files allowed is already reached
  // (not including the directory file itself)
  if(fileCount == MAX_FILE_NO)
  {
    delete[] directoryData;
    delete[] fileDescriptor;
    return EC_NO_SPACE_IN_DISK;
  }

  // if this is the first file to be recorded in the directory

  // write the file name in the directory along with the
  // file descriptor index found earlier
  int byteLocationToWrite = (fileCount + 1) * DIRECTORY_DATA_CHUNK_SIZE;
  lseek(OFT_DIRECTORY_INDEX, byteLocationToWrite);
  int j = 0;
  for(j = 0; j < MAX_FILE_NAME_LEN; ++j)
  {
    write(OFT_DIRECTORY_INDEX, fileName[j], 1);
  }
  write(OFT_DIRECTORY_INDEX, fileDescriptorIndex, 1);

  // update the file size of the directory (do I have to do this here or does
  // write() take care of that already?)
  // update the bytemap if necessary (do I have to do this here or does write()
  // take care of that already?)
  // I believe write() should actually take care of these (i.e. it makes sense)

  // make and allocate a file descriptor for new file
  // initialize the file descriptor to 0,-1,-1,-1
  delete[] fileDescriptor;
  fileDescriptor = new char[DESCR_SIZE];
  fileDescriptor[FD_FILE_SIZE] = 0;
  fileDescriptor[FD_FIRST_BLOCK] = -1;
  fileDescriptor[FD_SECOND_BLOCK] = -1;
  fileDescriptor[FD_THIRD_BLOCK] = -1;
  writeDescriptor(fileDescriptorIndex, fileDescriptor);

  delete[] directoryData;
  delete[] fileDescriptor;
  return 0; // success
}

int FileSystem53::old_create(const std::string &fileName)
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
  // go to the open file table to grab file descriptor index of directory
  int directoryIndex = table.table[OFT_DIRECTORY_INDEX].index;

  // use file descriptor index to grab the file descriptor from the descTtable
  char *directoryDescriptor = readDescriptor(directoryIndex);
  int byteCount = 0;

  // Go block by block from the directory.
  for(int i = 1;
      i <= FD_THIRD_BLOCK && byteCount < directoryDescriptor[FD_FILE_SIZE]; ++i)
  {
    char directoryBlock[B];
    io.read_block(directoryDescriptor[i], directoryBlock);
    for(int j = 0; j < B; j += MAX_FILE_NAME_LEN + 1)
    {
      if(strncmp(directoryBlock + j, fileName.c_str(), MAX_FILE_NAME_LEN) == 0)
      {
        for(int k = 0; k < 3; ++k)
        {
          if(!table.open[k])
          {
            memset(table.table[k].buf, 0, B);
            table.table[k].pos = 0;
            table.table[k].index = directoryBlock[j + MAX_FILE_NAME_LEN];
            table.open[k] = true;
            delete[] directoryDescriptor;
            return k;
          }
        }
        delete[] directoryDescriptor;
        return EC_OFT_FULL;
      }
      ++byteCount;
    }
  }
  delete[] directoryDescriptor;
  return EC_FILE_NOT_OPEN;
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

  // If the current position == file size before any reading even began for this
  // call,
  // and the count > 0, return EC_EOF;
  if(static_cast<int>(static_cast<unsigned char>(
         fileDescriptor[(fileIndex * 4)])) == table.table[index].pos)
  {
    return EC_EOF;
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
  delete[] fileDescriptor;
  return 0;
}

int FileSystem53::close(int index)
{
  const int oldBlock = 1 + (table.table[index].pos / B);
  const int fileDescriptorIndex = table.table[index].index;

  char *fileDescriptor = readDescriptor(fileDescriptorIndex);

  // Check if the index @ table is even open to begin with
  if(!table.open[index])
  {
    delete[] fileDescriptor;
    return EC_FILE_NOT_OPEN;
  }
  // Freeing OFT entry
  table.open[index] = false;

  // Write the buffer to the disk
  io.write_block(fileDescriptor[oldBlock], table.table[index].buf);

  // Resetting to 0, saying there's nothing in this index.
  table.table[index].pos = 0;
  // The file that is open is being closed
  table.table[index].index = 0;
  delete fileDescriptor;
  return 0;
}

int FileSystem53::destroy(const std::string &fileName) { deleteFile(fileName); }

/* Delete file function:
 *    Delete a file
 * Parameter(s):
 *    symbolic_file_name: a file name to be deleted.
 * Return:
 *    Return 0 with success
 *    Return -1 with error (ie. No such file).
 */
int FileSystem53::deleteFile(const std::string &fileName)
{

  // look for file's descriptor index in directory file
  char *directoryData = new char[DIRECTORY_DATA_CHUNK_SIZE];
  int directoryFileByteLocation = searchDir(fileName, directoryData);
  int fileDescriptorIndex = static_cast<int>(
      static_cast<unsigned char>(directoryData[DIRECTORY_DATA_CHUNK_SIZE - 1]));

  // if file NOT found
  if(directoryFileByteLocation == EC_FILE_NOT_FOUND)
  {
    return EC_DELETE_FAILURE;
  }

  // grab file descriptor
  char *fileDescriptor = readDescriptor(fileDescriptorIndex);

  // if file is open
  /*
   * Per instructor's note during lecture
   * File that are currently open cannot be deleted
   */
  if(searchOFT(fileDescriptorIndex) != EC_FILE_NOT_OPEN)
  {
    return EC_DELETE_FAILURE;
  }

  // clear out the bytemap corresponding to the blocks occupied by file
  fileDescriptor[FD_FILE_SIZE] = -1;
  for(int i = 1; i < DESCR_SIZE; ++i)
  {
    if(fileDescriptor[i] != -1)
    {
      descTable[BYTEMAP_BLOCK_INDEX][static_cast<int>(
          static_cast<unsigned char>(fileDescriptor[i]))] = 0;
    }
    fileDescriptor[i] = -1;
  }

  // deallocate file from file descriptor
  writeDescriptor(fileDescriptorIndex, fileDescriptor);

  // remove entry from directory file
  lseek(OFT_DIRECTORY_INDEX, directoryFileByteLocation);
  write(OFT_DIRECTORY_INDEX, -1, DIRECTORY_DATA_CHUNK_SIZE);

  // decrease file size of directory by 11 bytes IF and ONLY IF
  // the deleted file was the last entry in the directory
  // By the way, we decrease the file size by marking the
  // file descriptor of the directory file itself

  // remove any blocks from the file descriptor of the directory
  // if necessary

  return 0; // success
}

void FileSystem53::directory()
{
  int i, k;
  char *fileDescriptor;
  int fileDescriptorIndex, fileSize;

  // reset seek position to 0
  lseek(OFT_DIRECTORY_INDEX, 0);

  char *directory = new char[B];

  // read all the contents of directory
  fileSize = read(OFT_DIRECTORY_INDEX, directory, MAX_FILE_SIZE);

  if(fileSize == EC_FILE_NOT_OPEN)
  {
    std::cout << "File hasn't been open \n";
  }
  else if(fileSize == EC_EOF)
  {
    std::cout << "End-of-file \n";
  }
  else if(fileSize == 0)
  {
    std::cout << "Directory File is empty! \n";
  }
  else
  {
    k = 0; // file name string counter

    for(i = 0; i < fileSize; i++)
    {

      // if this is a file descriptor index byte
      if(k == 10)
      {
        // File name display has been completed.
        // Next, get the file size
        fileDescriptorIndex =
            static_cast<int>(static_cast<unsigned char>(directory[i]));
        fileDescriptor = readDescriptor(fileDescriptorIndex);

        std::cout << " " << static_cast<int>(static_cast<unsigned char>(
                                fileDescriptor[FD_FILE_SIZE])) << " bytes";

        // if this is NOT the last file to be displayed
        if(i + 1 < fileSize)
        {
          // display comma
          std::cout << ", ";
        }

        k = 0;
        delete[] fileDescriptor;
      }
      else
      {
        if(directory[i] > 0)
        {
          std::cout << directory[i];
        }
        k++;
      }
    }
  }
}

void FileSystem53::restore(const std::string &name) { io.load(name); }

void FileSystem53::save(const std::string &name) { io.save(name); }

void FileSystem53::diskDump(int start, int size) {}

/*
 * addBlock() marks the first available
 * block in the bytemap and returns the block index
 * that it marked.
 *
 * Note that it does not allocate the actual block with data.
 */
int FileSystem53::addBlock()
{
  char bytemap[B];
  memcpy(bytemap, descTable[0], B);
  for(int i = 0; i < B; ++i)
  {
    if(!bytemap[i])
    {
      bytemap[i] = 1;
      memcpy(descTable[0], bytemap, B);
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
  open[3] = false;
}

void FileSystem53::initializeOFT(int oftIndex, char *dataBlock,
                                 char fileDescriptorIndex)
{
  table.table[oftIndex].index = fileDescriptorIndex;
  table.table[oftIndex].pos = 0;
  mempcpy(table.table[oftIndex].buf, dataBlock, B);
  table.open[oftIndex] = true;
}

int FileSystem53::searchOFT(int fileDescriptorIndex)
{
  for(int i = 0; i < MAX_OPEN_FILE; i++)
  {
    if(table.open[i] && static_cast<int>(static_cast<unsigned char>(
                            table.table[i].index)) == fileDescriptorIndex)
    {
      return i;
    }
  }
  return EC_FILE_NOT_OPEN;
}
