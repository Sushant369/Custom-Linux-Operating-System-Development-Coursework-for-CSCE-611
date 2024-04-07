#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"



PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;



void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
   //assert(false);
	PageTable::kernel_mem_pool = _kernel_mem_pool;
	PageTable::process_mem_pool = _process_mem_pool;
	PageTable::shared_size = _shared_size;
   Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
   //assert(false);
   Console::puts("Constructed Page Table object\n");
   
	unsigned long basePageAddr=0;
	
	unsigned long kernelPoolFrameNum = PageTable::shared_size/PAGE_SIZE;
	
	page_directory = (unsigned long*) (kernel_mem_pool->get_frames(1) * PAGE_SIZE);  
	
	unsigned long* pageTable = (unsigned long*) (kernel_mem_pool->get_frames(1) * PAGE_SIZE);
	
	for(int i=0;i<kernelPoolFrameNum;++i)
	{
		pageTable[i] = basePageAddr | 2 | 1;    
		basePageAddr += PAGE_SIZE;
		//Console::putui(basePageAddr);
		Console::puts("\n");
	}
	
	for(int i=0;i< 1024;++i)
	{
		if(i==0)
		{
			page_directory[i] = reinterpret_cast<unsigned long>(pageTable) | 2 | 1;
		}
		else
		{
			page_directory[i] = 2;
		}
	}
	basePageAddr=0; // reset base Page Address

}


void PageTable::load()
{
//   assert(false);
   Console::puts("Loaded page table\n");
   
	current_page_table=this;
	
	write_cr3( (unsigned long) page_directory );

}

void PageTable::enable_paging()
{
  // assert(false);
   Console::puts("Enabled paging\n");
   	paging_enabled=1;
   	
	write_cr0(read_cr0() | 0x80000000);
   

}

void PageTable::handle_fault(REGS* _r)
{
  //assert(false); 
  Console::puts("handled page fault\n"); // Display a message indicating a page fault is being handled
  unsigned long error_msg = _r->err_code; // Extract the error message from the register

  if ((error_msg & 0x1) == 1)
  {
    Console::puts("All good\n"); // If the error message's lowest bit is 1, it's considered "All good," so we return
    return;
  }

  // Define masks for extracting directory and page table information from the virtual address
  unsigned long PageDirMask = 0xffc00000;   // 11111111110000000000000000000000 in binary
  unsigned long PageTableMask = 0x003ff000; // 00000000001111111111000000000000 in binary

  // Read the virtual address that caused the page fault
  unsigned long virtualAddress = read_cr2();

  // Calculate the directory and page addresses using the masks
  unsigned long directory_address = (virtualAddress & PageDirMask) >> 22; // Extract bits 31-22
  unsigned long page_address = (virtualAddress & PageTableMask) >> 12;   // Extract bits 21-12

  // Get the address of the page directory from the CR3 register
  unsigned long* page_directory_address = (unsigned long*)(read_cr3());

  unsigned long* pageTable = NULL;

  bool valid_page = false;

  // Check if the page directory entry is valid (bit 0 set to 1)
  if ((page_directory_address[directory_address] & 1) == 1)
  {
    valid_page = true;
  }

  // If the page is not valid, allocate a new frame from the kernel memory pool
  if (!valid_page)
  {
    page_directory_address[directory_address] = (unsigned long)(kernel_mem_pool->get_frames(1) * PAGE_SIZE) | 1 | 2;
  }

  // Calculate the page table address from the page directory entry
  pageTable = (unsigned long*)(page_directory_address[directory_address] & (PageTableMask + PageDirMask));

  // If the page is not valid, initialize the page table entries (for all 1024 pages) to 4
  if (!valid_page)
  {
    for (int j = 0; j < 1024; j += 1)
    {
      pageTable[j] = 4;
    }
  }

  // Update the page table entry for the specific page address
  pageTable[page_address] = PageTable::process_mem_pool->get_frames(1) * PAGE_SIZE | 1 | 2;
}


