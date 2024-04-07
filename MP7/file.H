/*
     File        : file.H

     Author      : Riccardo Bettati
     Modified    : 2021/11/18

     Description : Simple File class with sequential read/write operations.
 
*/

#ifndef _FILE_H_
#define _FILE_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "file_system.H"

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

class Inode;
class FileSystem;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* class  F i l e   */
/*--------------------------------------------------------------------------*/

class File  {

private:
    /* -- your file data structures here ... */
    long activePosition;
    int id;
    Inode *inode;
    FileSystem *newFileSystem;
    

    /* You will need a reference to the inode, maybe even a reference to the
       file system.
       You may also wans or write next. */

    unsigned char block_cache[SimpleDisk::BLOCK_SIZE];
    /* It will be helpful to have a cached copy of the block that you are reading
       from and writing to. In the base submission, files have only one block, which
       you can cache here. You read the data from disk to cache whenever you open the
       file, and you write the data to disk whenever you close the file.
    */


public:

    File(FileSystem * _fs, int _id);
    

    ~File();
    /* Closes the file. Deledata structures associated with the file handle. */

    int Read(unsigned int _n, char * _buf);
   

    int Write(unsigned int _n, const char * _buf);
    

    void Reset();
    /* Set the ’current pohe beginning of the file. */

    bool EoF();
    

};

#endif
