#include "filesystem53.h"
#include <string.h>
FileSystem53::FileSystem53()
{
  format();
}

void FileSystem53::OpenFileTable() {}

int FileSystem53::findOft() {}

void FileSystem53::deallocateOft(int index) {}

void FileSystem53::format()
{
  char input[B];
  for(int i=0;i<7;++i)
  {
    input[i] = 1;
  }
  for(int i=7;i<B;++i)
  {
    input[i] = 0;
  }
  io.write_block(0,input);
  for(int i=0;i<B;++i)
  {
    if(i % 4 == 0)
    {
      input[i] = 0;
    }
    else
    {
      input[i] = -1;
    }
  }
  for(int i=1;i<7;++i)
  {
    io.write_block(i,input);
  }
}

// variable "no" is expected to be a 0 based file descriptor index
// However, since the file descriptors are 4 bytes each
// we need to calculate the offset by 4 bytes
// Therefore,
// input of 0 means file descriptor index 0
// input of 1 means file descriptor index 4
// input of 2 means file descriptor index 8
// etc.
char *FileSystem53::readDescriptor(int no) {

	char * fileDescriptor = new char[4];

	int rawFileDescriptorIndexOffset = no * 4;
	int rawFileDescriptorIndexA = ( rawFileDescriptorIndexOffset / B ) + 1;
	int rawFileDescriptorIndexB = rawFileDescriptorIndexOffset % B;

	fileDescriptor[0] = descTable[rawFileDescriptorIndexA][rawFileDescriptorIndexB++];
	fileDescriptor[1] = descTable[rawFileDescriptorIndexA][rawFileDescriptorIndexB++];
	fileDescriptor[2] = descTable[rawFileDescriptorIndexA][rawFileDescriptorIndexB++];
	fileDescriptor[3] = descTable[rawFileDescriptorIndexA][rawFileDescriptorIndexB];

	return fileDescriptor;
}

void FileSystem53::clearDescriptor(int no) {}

void FileSystem53::writeDescriptor(int no, char *desc) {}

int FileSystem53::findEmptyDescriptor() {}

int FileSystem53::findEmptyBlock() {}

int FileSystem53::fgetc(int index) {}

int FileSystem53::fputc(int c, int index) {}

bool FileSystem53::feof(int index) {}

int FileSystem53::searchDir(int index, const std::string &symbolic_file_name) {}

void FileSystem53::deleteDir(int index, int start_pos, int len) {}

int FileSystem53::create(const std::string &symbolic_file_name) {}

int FileSystem53::openDesc(int desc_no) {}

int FileSystem53::open(const std::string &symbolic_file_name)
{

}

int FileSystem53::read(int index, char *mem_area, int count) {}

int FileSystem53::write(int index, char value, int count) {}

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

	if( !table.open[index] ){
		return EC_FILE_NOT_OPEN;
	}

	// E/O VERIFY FILE IS OPEN

	// if the position being requested to move to is negative
	// return error code for negative seek position
	if( pos < 0 ){
		return EC_NEGATIVE_SEEK_POSITION;
	}

	// go to the file descriptor of this file to obtain the file size
	//int fileDescriptorIndex = table.table[openFileTableIndex].index;
	//char *fileDescriptor = readDescriptor(fileDescriptorIndex);
	char *fileDescriptor = readDescriptor(table.table[index].index);

	// if the position being requested goes beyond file size,
	// set position to the file size
	if( pos > static_cast<unsigned char>(fileDescriptor[FD_FILE_SIZE]) ){
		pos = fileDescriptor[FD_FILE_SIZE];
	}

  const int oldBlock = 1 + (table.table[index].pos / B);
  table.table[index].pos = pos;
  const int currentBlock = 1 + (table.table[index].pos / B);
  if(oldBlock != currentBlock)
  {
    // Write what we have to disk.
    io.write_block(static_cast<int>(static_cast<unsigned char>( fileDescriptor[oldBlock] )),
                   table.table[index].buf);

    // Read the "new" current block from disk
    io.read_block(static_cast<int>(static_cast<unsigned char>( fileDescriptor[currentBlock] )),
                  table.table[index].buf);
  }

  return 0;
}

void FileSystem53::close(int index) {}

int FileSystem53::deleteFile(const std::string &fileName) {}

void FileSystem53::directory() {
	// get the file descriptor of the directory file
	char *directoryFileDescriptor = readDescriptor(FD_DIRECTORY_FILE_DESCRIPTOR_INDEX);

	int byteCount = 0;
	int directoryFileSize = static_cast<int>(static_cast<unsigned char>( directoryFileDescriptor[FD_FILE_SIZE] ));
	int currentBlockIndex;
	char *currentBlock = new char[B];
	int i,j,k;
	char * fileDescriptor;
	int fileDescriptorIndex, fileSize;

	// if the directory file is empty
	if( directoryFileSize <= 0 ){
		std::cout << "Directory File is empty!" << std::endl;
	}else{
		/*
		 * IF THE DIRECTORY FILE NEEDS TO BE ON THE OFT
		 * WE REGISTER IT HERE
		 */

		// iterative over the blocks of the directory and output the data
		for( i = FD_FIRST_BLOCK ; i <= FD_THIRD_BLOCK && byteCount < directoryFileSize ; i++ ){

			currentBlockIndex = static_cast<int>(static_cast<unsigned char>( directoryFileDescriptor[i] ));

			// if the block specified by the file descriptor is valid
			if( currentBlockIndex > 0 ){

				// get that block from disk
				io.read_block( currentBlockIndex , currentBlock );

				k = 0; // file name string counter

				// iterate over the files listed in this block
				for( j = 0 ; j < B && byteCount < directoryFileSize ; j++ ){
					// if this is a file descriptor index byte
					if( k == 10 ){
						// File name display has been completed.
						// Next, get the file size
						fileDescriptorIndex = static_cast<int>(static_cast<unsigned char>( currentBlock[j] ));
						fileDescriptor = readDescriptor(fileDescriptorIndex);

						std::cout << " " << static_cast<int>(static_cast<unsigned char>( fileDescriptor[FD_FILE_SIZE] ) ) << " bytes";

						// if this is NOT the last file to be displayed
						if( byteCount+1 < directoryFileSize ){
							// display comma
							std::cout << ", ";
						}

						k = 0;

					}else{
						if( currentBlock[j] != 0 ){
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

void FileSystem53::restore(const std::string &name)
{
  io.load(name);
}

void FileSystem53::save(const std::string &name)
{
  io.save(name);
}

void FileSystem53::diskDump(int start, int size) {}
