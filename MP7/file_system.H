/* 
    File: file_system.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 21/11/28

    Description: Simple File System.
    

*/

#ifndef _FILE_SYSTEM_H_ // include file only once
#define _FILE_SYSTEM_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "simple_disk.H"

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

class FileSystem;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

class Inode
{
  friend class FileSystem; 
  friend class File;       
                           
private:
  FileSystem *newFileSystem; 
   long id; 
   long file_size;

   long first_block;
};

/*--------------------------------------------------------------------------*/
/* FORWARD DECLARATIONS */
/*--------------------------------------------------------------------------*/

/* (none) */

/*--------------------------------------------------------------------------*/
/* F i l e S y s t e m  */
/*--------------------------------------------------------------------------*/

class FileSystem
{

  friend class Inode;
  friend class File;

private:
  /* -- DEFINE YOUR FILE SYSTEM DATA STRUCTURES HERE. */

  SimpleDisk *disk;
  unsigned int size;
  unsigned int maximum_blocks;

  static constexpr unsigned int MAX_INODES = SimpleDisk::BLOCK_SIZE / sizeof(Inode);
  
  static const unsigned int sizeOfBlock = SimpleDisk::BLOCK_SIZE;
  /* Just as an example, you can store MAX_INODES in a single INODES block */

  Inode *inodes; // the inode list
  /* The inode list */

  unsigned char *free_blocks;
  /* The free-block list. You may want to implement the "list" as a bitmap. 
     Feel free to use an unsigned char to represent whether a block is free or not; 
     no need to go to bits if you don't want to. 
     If you reserve one block to store the "free list", you can handle a file system up to 
     256kB. (Large enough as a proof of concept.) */
   bool Available(int blck);     
   short GetFreeInode();
   int GetFreeBlock();



   
  /* It may be helpful to two functions to hand out free inodes in the inode list and free
     blocks. These functions also come useful to class Inode and File. */

public:
  FileSystem();
  /* Just initializes local data structures. Does not connect to disk yet. */

  ~FileSystem();
  /* Unmount file system if it has been mounted. */

  bool Mount(SimpleDisk *_disk);
  /* Associates this file system with a disk. Limit to at most one file system per disk.
     Returns true if operation successful (i.e. there is indeed a file system on the disk.) */

  static bool Format(SimpleDisk *_disk, unsigned int _size);

  /* Wipes any file system from the disk and installs an empty file system of given size. */

  Inode *LookupFile(int _file_id);
  /* Find file with given id in file system. If found, return its inode. 
       Otherwise, return null. */

  bool CreateFile(int _file_id);
  /* Create file with given id in the file system. If file exists already,
     abort and return false. Otherwise, return true. */

  bool DeleteFile(int _file_id);
  /* Delete file with given id in the file system; free any disk block occupied by the file. */
};
#endif
