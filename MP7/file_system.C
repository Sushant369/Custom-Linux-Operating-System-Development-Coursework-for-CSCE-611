/*
     File        : file_system.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File System class.
                   Has support for numerical file identifiers.
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file_system.H"

/*--------------------------------------------------------------------------*/
/* CLASS Inode */
/*--------------------------------------------------------------------------*/

/* You may need to add a few functions, for example to help read and store 
   inodes from and to disk. */

/*--------------------------------------------------------------------------*/
/* CLASS FileSystem */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

FileSystem::FileSystem() {
    Console::puts("In file system constructor.\n");
    //assert(false);
}

FileSystem::~FileSystem() {
    Console::puts("unmounting file system\n");
    /* Make sure that the inode list and the free list are saved. */
    //assert(false);
}


/*--------------------------------------------------------------------------*/
/* FILE SYSTEM FUNCTIONS */
/*--------------------------------------------------------------------------*/


bool FileSystem::Mount(SimpleDisk * _disk) {
    Console::puts("mounting file system from disk\n");
    disk=_disk;
    /* Here you read the inode list and the free list into memory */
    
    return true;
}

bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size) { 
    Console::puts("formatting disk\n");
    //assert(false);
    /* Here you populate the disk with an initialized (probably empty) inode list
       and a free list. Make sure that blocks used for the inodes and for the free list
       are marked as used, otherwise they may get overwritten. */
    
    unsigned char cache[sizeOfBlock] ; 
              
    int num_blocks = _size/sizeOfBlock;
    
    memset(cache,0,sizeOfBlock);

    for(int i=0;i<num_blocks;i++)
    {
        _disk->write(i,cache);
    }


    memset(cache,0, sizeOfBlock);
    
    _disk->read(0,cache);
    
    Inode *inode = (Inode *) cache;
    
    for(int i=0;i< MAX_INODES; i++)
    {
        inode[i].id = -1;
        inode[i].first_block=-1;
        inode[i].file_size=0;
    }

    _disk->write(0, cache);



    memset(cache, 0, sizeOfBlock);
    
    _disk->read(1, cache);
    
    for(int i=0;i<sizeOfBlock;i++)
    {
        cache[i]=0;
    }
    cache[0] = 1;
    cache[1] = 1;
    
    _disk->write(1,cache);

    
    return true;     
}

Inode * FileSystem::LookupFile(int _file_id) {
    Console::puts("looking up file with id = "); Console::puti(_file_id); Console::puts("\n");
    /* Here you go through the inode list to find the file. */
   // assert(false);
   
   unsigned char cache[sizeOfBlock];
   memset(cache,0,sizeOfBlock);
   
   disk->read(0, cache);
   
   inodes= (Inode *) cache;

for (int i = 0; i < MAX_INODES; i++) {
    if (_file_id == inodes[i].id) {
        Console::puts("File found \n");
        return &(inodes[i]);
    }
}
   return NULL;
}


bool FileSystem::Available(int block)
{
    unsigned char cache[sizeOfBlock];
    memset(cache,0,sizeOfBlock);
        disk->read(1, cache);
	    
	if (cache[block] == 0) {
	    return false;
	}

	cache[block] = 0;
	disk->write(1, cache);
	return true;
    
    
}

int FileSystem::GetFreeBlock()
{
    unsigned char cache[sizeOfBlock];
    memset(cache,0,sizeOfBlock);
        disk->read(1, cache);
        
	for (int i = 0; i < sizeOfBlock; i++) {
	    if (cache[i] == 0) {
		cache[i] = 1;
		disk->write(1, cache);
		return i;
	    }


	disk->write(1, cache); // Perform the disk write after the loop if no zero found
	}
	return -1; // Return -1 if no zero found in the cache
        
}


bool FileSystem::CreateFile(int _file_id) {
    Console::puts("creating file with id:"); Console::puti(_file_id); Console::puts("\n");
    /* Here you check if the file exists already. If so, throw an error.
       Then get yourself a free inode and initialize all the data needed for the
       new file. After this function there will be a new file on disk. */
    bool track = false;
    unsigned char cache[sizeOfBlock];
    memset(cache,0,sizeOfBlock);
    disk->read(0, cache);
    inodes = (Inode* ) cache;
    
for (int i = 0; i < MAX_INODES; i++) {
    // Check if operation is already completed
    if (track) {
        break;
    }

    // Check if the inode with the file ID already exists
    if (inodes[i].id == _file_id) {
        return false; // File already exists, no need to create a new one
    }

    // Check if the inode is free (i.e., its id is -1)
    if (inodes[i].id == -1) {
        // Assign file ID, allocate the first block and set file size
        inodes[i].id = _file_id;
        inodes[i].first_block = GetFreeBlock();
        inodes[i].file_size = sizeOfBlock;
        // Write the updated inode information to disk
        disk->write(0, cache);
        // Notify that a new file was created
        Console::puts("File created\n");
        // Set flag to indicate that the operation is complete
        track  = true;
        break; // Exit the loop as the operation is completed
    }
    // Continue to the next iteration if the current inode is not free
}

    return true;
}

bool FileSystem::DeleteFile(int _file_id) {
    Console::puts("deleting file with id:"); Console::puti(_file_id); Console::puts("\n");
    unsigned char cache[sizeOfBlock];
    memset(cache,0,sizeOfBlock);
    disk->read(0, cache);
    bool track=false;
    inodes = (Inode *) cache;
    for(int i=0;i< MAX_INODES; i++)               
    {  
        if(track)
        {
            continue;
        }
        if(inodes[i].id != _file_id)
        {
            continue;
        }
        inodes[i].id = -1;
        Available(inodes[i].first_block);
        inodes[i].first_block=-1;
        inodes[i].file_size=0;

        disk->write(0,cache);
	//set track to true
        track = true;        
    }
    return true; 
}



