/*
 File: vm_pool.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

#define my_size 512
/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "vm_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "page_table.H" 

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) {
	//assert(false);
    Console::puts("Constructed VMPool object.\n");
	pageTable = _page_table;
    baseAddrs=     _base_address;
    framePool =         _frame_pool;
    size = _size;
    NumFreeReg = 1;
    pageTable->register_pool(this);
    memoryDum = (cont_memory* ) baseAddrs;
    for(int i=0;i<my_size;i++)                 
    {
        if(i==0)
        {
			//Console::puts(memoryDum[i].start_addr);
            memoryDum[i].start_addr = baseAddrs;             
            memoryDum[i].size = Machine::PAGE_SIZE;
        }
        else if(i<my_size)
        {
            memoryDum[i].start_addr =0;
            memoryDum[i].size=0;
        }
    }
}

unsigned long VMPool::allocate(unsigned long _size) {
    //assert(false);
	Console::puts("Allocated region of memory.\n");
    if(NumFreeReg >=my_size)            
    {
		Console::puts("Error in allocator, no memory\n");
        assert(false);
    }
	
    unsigned long offset = _size%Machine::PAGE_SIZE;
	//Console::puts(offset);
	unsigned long PageCount = _size/Machine::PAGE_SIZE;
	//Console::puts(PageCount);
    unsigned long addrsDum = memoryDum[NumFreeReg-1].start_addr ;

	 
	if (offset>0)
	{
	 PageCount++;
	}
    
    addrsDum+=memoryDum[NumFreeReg-1].size;
    //Console::puts(addrsDum);
	NumFreeReg++;

    memoryDum[NumFreeReg-1].start_addr = addrsDum;
	
	
    memoryDum[NumFreeReg-1].size = PageCount * Machine::PAGE_SIZE;
    

    return addrsDum;
}

void VMPool::release(unsigned long _start_address) {
    //assert(false);
    Console::puts("Released continuous region of memory with respective start address.\n");
    int pt=0;
	
    for(int i=0;i<my_size;i++)
    {
        if(memoryDum[i].start_addr == _start_address)
        {
            pt=i;
			break;			
        }
		else
		{
			continue;
		}

    }

    unsigned long TempAddrs = _start_address;
	
    for(unsigned long i=0;i<memoryDum[pt].size / Machine::PAGE_SIZE;i++)
    {
        unsigned long CurrentPageNum= TempAddrs;
		//Console::puts(CurrentPageNum);
        CurrentPageNum += (i*(Machine::PAGE_SIZE));
        pageTable->free_page(CurrentPageNum);
    }

    NumFreeReg--;
    for(int i=pt;i<=NumFreeReg+1;i++)
    {
        if(i!=NumFreeReg+1)
		{
            memoryDum[i] = memoryDum[i+1];
    	}
		else
		{
			continue;
		}
	}

    
}

bool VMPool::is_legitimate(unsigned long _address) {
    //assert(false);
    Console::puts("Checked extensively if address is part of an already allocated block of memory or not.\n");
    for(int i=0;i<NumFreeReg;i++)
    {
        if((_address == baseAddrs))
        {
            return true;
        }
        unsigned long MaxAddrs= memoryDum[i].start_addr + memoryDum[i].size;
        
		
		if( (_address >= memoryDum[i].start_addr)  )       
        {
            if(_address > MaxAddrs)
			{
                continue;
            }
			
			return true;
        }
    }
   return false;
}

