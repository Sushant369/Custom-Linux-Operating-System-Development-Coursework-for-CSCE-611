/*
     File        : file.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File class, with support for
                   sequential read/write operations.
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
#include "file.H"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR */
/*--------------------------------------------------------------------------*/

File::File(FileSystem *_fs, int _id) {
    Console::puts("Opening file.\n");
    //asert(false);    
    id = _id;
    newFileSystem = _fs;

    int blck = newFileSystem->LookupFile(id)->first_block;
    
    memset(block_cache, 0, SimpleDisk::BLOCK_SIZE);
    newFileSystem->disk->read(blck, block_cache);

}

File::~File() {
    Console::puts("Closing file.\n");
    
    int b = newFileSystem->LookupFile(id)->first_block;
    
    newFileSystem->disk->write(b, block_cache);
}

/*--------------------------------------------------------------------------*/
/* FILE FUNCTIONS */
/*--------------------------------------------------------------------------*/

int File::Read(unsigned int _n, char *_buf) 
{
    Console::puts("Reading from file\n");
    int numOfBlck = 0;
    for (int i = 0; i < _n; i++) 
    {
        if (!EoF()) 
        {
            activePosition+=1;
            numOfBlck+=1;
            _buf[i] = block_cache[activePosition-1];   

        }
        else
        {
            i=_n;
        }
    }

    return numOfBlck;
}

int File::Write(unsigned int _n, const char *_buf) 
{
    int numOfBlck = 0;
    Console::puts("Writing to file\n");
    for (int i = 0; i < _n; i++) 
    {
        if (!EoF()) 
        {
        //Console::puti(_n);
            numOfBlck++;
            activePosition+=1;
            block_cache[activePosition-1] = _buf[i];

        }
        else
        {
            i=_n;
        }
    }
    return numOfBlck;
}

void File::Reset() 
{
    if(activePosition!=0)
        activePosition = 0;
}

bool File::EoF() 
{
    return (activePosition == SimpleDisk::BLOCK_SIZE -1);
}
