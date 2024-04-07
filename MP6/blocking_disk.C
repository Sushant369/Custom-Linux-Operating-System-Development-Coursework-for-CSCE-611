/*
     File        : blocking_disk.c

     Author      : 
     Modified    : 

     Description : 

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "blocking_disk.H"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/
concurrencyControl concur_control;
extern Scheduler * SYSTEM_SCHEDULER;



BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {
}

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/

void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {
  #ifdef _THREAD_SYNCHRONIZATION_
    concur_control.acquireLock();                   
  #endif
  SimpleDisk::read(_block_no, _buf);

  #ifdef _THREAD_SYNCHRONIZATION_
    concur_control.releaseLock();
  #endif
}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
  #ifdef _THREAD_SYNCHRONIZATION_
    concur_control.acquireLock();                  
  #endif
  SimpleDisk::write(_block_no, _buf);

  #ifdef _THREAD_SYNCHRONIZATION_
    concur_control.releaseLock();
  #endif

  SimpleDisk::write(_block_no, _buf);
}


bool BlockingDisk::block_ready()
{
  if(Machine::inportb(0x1F7) & 0x08 ==0 )
  {
    return false;
  }
  else
    return true;
}


void BlockingDisk::wait_until_ready()
{
  while(!block_ready())
  {
    #ifdef _HANDLE_INTERRUPTS_
    
      thread_queue.enqueue(Thread::CurrentThread());
      
    #else
      SYSTEM_SCHEDULER->resume(Thread::CurrentThread());
      
    #endif

    SYSTEM_SCHEDULER->yield();
  }
}

#ifdef _HANDLE_INTERRUPTS_
  void BlockingDisk::handle_interrupt(REGS *_r)
  {
    Thread* t = thread_queue.rem();
    SYSTEM_SCHEDULER->resume(t);      
  }
#endif

MirroredDisk::MirroredDisk(DISK_ID _disk_id, unsigned int _size)
  : BlockingDisk(_disk_id, _size) {
      primaryDisk = new BlockingDisk(DISK_ID::MASTER, _size);
      linkedDisk = new BlockingDisk(DISK_ID::DEPENDENT, _size);
}
/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/

void MirroredDisk::executeTask(DISK_OPERATION _op, unsigned long _block_no, DISK_ID disk) {

  Machine::outportb(0x1F1, 0x00); 
  Machine::outportb(0x1F2, 0x01); 
  Machine::outportb(0x1F3, (unsigned char)_block_no);
  Machine::outportb(0x1F4, (unsigned char)(_block_no >> 8));
  Machine::outportb(0x1F5, (unsigned char)(_block_no >> 16));
  unsigned int disk_no = disk == DISK_ID::MASTER ? 0 : 1;
  Machine::outportb(0x1F6, ((unsigned char)(_block_no >> 24)&0x0F) | 0xE0 | (disk_no << 4));

  Machine::outportb(0x1F7, (_op == DISK_OPERATION::READ) ? 0x20 : 0x30);
}
void MirroredDisk::read(unsigned long _block_no, unsigned char * _buf) {
  executeTask(DISK_OPERATION::READ, _block_no, DISK_ID::MASTER);
  executeTask(DISK_OPERATION::READ, _block_no, DISK_ID::DEPENDENT);
  wait_until_ready();

  unsigned short temp;
  for (int i = 0; i < 256; i++) {
    temp = Machine::inportw(0x1F0);
    _buf[i*2]   = (unsigned char)temp;
    _buf[i*2+1] = (unsigned char)(temp >> 8);
  }
}

//implemented bonus point

void MirroredDisk::write(unsigned long _block_no, unsigned char * _buf) 
{
  primaryDisk->write(_block_no, _buf);
  linkedDisk->write(_block_no, _buf);
}

void MirroredDisk::wait_until_ready() 
{
    while ((!primaryDisk->block_ready()) || (!linkedDisk->block_ready())) 
    {
        SYSTEM_SCHEDULER->resume(Thread::CurrentThread());
        SYSTEM_SCHEDULER->yield();
    }
}
