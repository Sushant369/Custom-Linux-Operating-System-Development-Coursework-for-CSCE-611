#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

VMPool * PageTable::vm[];
PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;



void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
//    assert(false);
    Console::puts("Initialized Paging System\n");
	PageTable::kernel_mem_pool = _kernel_mem_pool;
	PageTable::process_mem_pool = _process_mem_pool;
	PageTable::shared_size = _shared_size;
}

PageTable::PageTable()
{
//    assert(false);
    Console::puts("Constructed Page Table object\n");
	unsigned long base_page_add=0;
	unsigned long kernelFramePoolNum = PageTable::shared_size/PAGE_SIZE;
	page_directory = (unsigned long*) (kernel_mem_pool->get_frames(1) * PAGE_SIZE);     
	unsigned long* page_table = (unsigned long*) (kernel_mem_pool->get_frames(1) * PAGE_SIZE);
	

	
	for(int i=0;i<kernelFramePoolNum;i+=1)
	{
		Console::puts("executing\n");
		page_table[i] = base_page_add | 2 | 1;       
		base_page_add=base_page_add + PAGE_SIZE;
	}


	
	for(int i=0;i< 1024;i++)
	{
		if(i==0)
		{
			page_directory[i] = (unsigned long) page_table | 3;
		}
        else if(i==1023)
        {
            page_directory[i]= (unsigned long) page_directory | 3;                 
        }
		else
		{
			page_directory[i] = 2;
		}
	}
}


void PageTable::load()
{
//    assert(false);
    Console::puts("Loaded page table\n");
	current_page_table=this;
	write_cr3( (unsigned long) page_directory );

}

void PageTable::enable_paging()
{
 //   assert(false);
    Console::puts("Enabled paging\n");
   	paging_enabled=1;
	write_cr0(read_cr0() | 0x80000000);
}

static unsigned long *PDE_address(unsigned long addr)
{
    unsigned long pde_addrs = (addr >> 20);
	//Console::puts(pde_addrs);
    pde_addrs |= 0xFFFFF000;
	
    pde_addrs &= 0xFFFFFFFC;
    return (unsigned long*) pde_addrs;
}



static unsigned long *PTE_address(unsigned long addr)
{
    unsigned long pte_addrs = (addr >> 10);
    pte_addrs |= 0xFFC00000;
    pte_addrs &= 0xFFFFFFFC;
    return (unsigned long*) (pte_addrs);
}


unsigned long PageTable::allocate_page(unsigned long page_count)
{
    unsigned long temp_addrs = (unsigned long) process_mem_pool->get_frames(page_count)*PAGE_SIZE ;
    temp_addrs |=3;
    return temp_addrs;

}
void PageTable::handle_fault(REGS * _r)
{   
//    assert(false);
	Console::puts("handled page fault\n");
	unsigned long error_msg = _r->err_code;

	if ( (error_msg & 1) ==1 )
	{
		Console::puts("No page Fault\n");
		return;
	}
	else
	{
		unsigned long DirMask = 0xffc00000;
		unsigned long pageMask = 0x003ff000;
		unsigned long virtualAddrs = read_cr2();
		unsigned long *pte_addr = PTE_address(virtualAddrs);
		unsigned long *pde_addr = PDE_address(virtualAddrs);
	
	

		unsigned long test=0;
		bool validPageIndicator = false;
		for(int i=0;i<512;i++)
		{
		    if(vm[i]==0)
		    {
		        test++;
		    }
		}
		test=512-test;

		for(int i=0;i<test && !validPageIndicator;i++)
		{
		    if(vm[i]->is_legitimate(virtualAddrs))
		    {
		        validPageIndicator=true;
		    }
		}

		if( (!validPageIndicator) && (test>0)  )
		{
		    Console::puts("Invalid address\n");
		    assert(false);
		}


		if( (*pde_addr & 0x1) !=1  )
		{
		    *pde_addr = PageTable::allocate_page(1);
		    *pte_addr = PageTable::allocate_page(1);
		}
		else
		{
		    *pte_addr = PageTable::allocate_page(1);
		}
	}
}

void PageTable::register_pool(VMPool * _vm_pool)
{
//    assert(false);


	Console::puts("registered VM pool\n");
    if(pool_count == 512)
    {
        Console::puts("Virtual memory is full. Can not register\n");
        assert(false);       
    }
    vm[pool_count]= _vm_pool;      
    pool_count+=1;



}

void PageTable::free_page(unsigned long _page_no) {
//    assert(false);
    Console::puts("freed page\n");
    unsigned long *pte_addr = PTE_address(_page_no);
    if( *pte_addr & 1==1)
    {
        process_mem_pool->release_frames(*pte_addr >>12);
        *pte_addr = 2;
    }

    write_cr3( (unsigned long) page_directory );

}
