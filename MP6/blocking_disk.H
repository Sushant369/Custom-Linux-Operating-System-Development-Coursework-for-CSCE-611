/*
     File        : blocking_disk.H

     Author      : 

     Date        : 
     Description : 

*/

#ifndef _BLOCKING_DISK_H_
#define _BLOCKING_DISK_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

#define _HANDLE_INTERRUPTS_
#define _THREAD_SYNCHRONIZATION_

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/
#include "thread.H"
#include "simple_disk.H"
#include "scheduler.H"
#include "interrupts.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* B l o c k i n g D i s k  */
/*--------------------------------------------------------------------------*/

struct concurrencyControl
{
   bool flag_control;
   concurrencyControl()
   {
      flag_control=false;
      
   }

   bool isSetValid()
   {
      if(flag_control)
      {
         return true;
      }
      else
      {
         flag_control=true;
         return false;
      }
   }
   void acquireLock()
   {
      while(isSetValid())
      {
         
      }
   }
   
   
   void releaseLock()
   {
      flag_control = false;
   }
};

class BlockingDisk : public SimpleDisk {


#ifdef _HANDLE_INTERRUPTS_    
private:
   threadQueue thread_queue;
   void handle_interrupt(REGS * _r);
#endif

protected:
   virtual void wait_until_ready();




public:
   BlockingDisk(DISK_ID _disk_id, unsigned int _size); 
   /* Creates a BlockingDisk device with the given size connected to the 
      MASTER or SLAVE slot of the primary ATA controller.
      NOTE: We are passing the _size argument out of laziness. 
      In a real system, we would infer this information from the 
      disk controller. */

   /* DISK OPERATIONS */
   bool block_ready();
   virtual void read(unsigned long _block_no, unsigned char * _buf);
   /* Reads 512 Bytes from the given block of the disk and copies them 
      to the given buffer. No error check! */

   virtual void write(unsigned long _block_no, unsigned char * _buf);
   /* Writes 512 Bytes from the buffer to the given block on the disk. */

};

//implemented MirroredDisk
class MirroredDisk : public BlockingDisk
{
   private:
      BlockingDisk* primaryDisk;
      BlockingDisk* linkedDisk;
      void executeTask(DISK_OPERATION _op, unsigned long _block_no, DISK_ID disk);
   protected:
      virtual void wait_until_ready();
   public:
      MirroredDisk(DISK_ID _disk_id, unsigned int _size);
      virtual void read(unsigned long _block_no, unsigned char * _buf);
      virtual void write(unsigned long _block_no, unsigned char * _buf);
};


#endif
