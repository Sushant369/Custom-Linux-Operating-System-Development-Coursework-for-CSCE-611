/*
 File: scheduler.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "simple_timer.H"

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
threadQueue* threadQueue::Head;
threadQueue* threadQueue::Tail;


/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/



void threadQueue::enqueueThread(Thread* t) {
    auto newElement = new threadQueue(); // Using 'auto' for type inference
    newElement->thread = t;

    if (Tail != nullptr) {
        Tail->next = newElement;
        Tail = newElement; // Directly updating 'Tail' without using 'Tail->next'
    } else {
        Head = Tail = newElement; // Merging two lines into one for the 'else' case
    }
}


Thread* threadQueue::dequeueThread() {
    // Check if the queue is empty before attempting to remove an item
    if (Head == nullptr) {
        return nullptr;
    }

    // Retrieve the thread from the head of the queue
    Thread* next_thread = Head->thread;
    // Save the next node to update the head after removal
    threadQueue* next_node = Head->next;

    // Delete the old head of the queue
    delete Head;
    // Update the head to the next node
    Head = next_node;

    // If after removal, the head is null, make sure the tail is also updated
    if (Head == nullptr) {
        Tail = nullptr;
    }

    // Return the thread from the removed queue node
    return next_thread;
}

bool threadQueue::isempty() {
    return Head == nullptr;
}



void threadQueue::removeThread(Thread* t) {
//  assert(false);
    // Check if the queue is not empty and the head thread is the one to be deleted
    if (Head != nullptr && Head->thread == t) {
        threadQueue* temp = Head; // Temporarily store the node to be deleted
        Head = Head->next; // Advance the head to the next node
        delete temp; // Delete the node
        return;
    }

    threadQueue *cur = Head, *prev = nullptr;
    while (cur != nullptr) {
        if (cur->thread == t) {
            if (prev != nullptr) {
                prev->next = cur->next; // Bypass the node to be deleted
            }
            delete cur; // Delete the current node
            return;
        }
        prev = cur; // Advance prev to current
        cur = cur->next; // Advance current to next node
    }
}



Scheduler::Scheduler() {
  Console::puts("Constructed Scheduler.\n");
}


// Returns true if interrupts are enabled, false otherwise.
bool interruptIsOn() {
    return Machine::interrupts_enabled();
}

// Enables or disables interrupts based on the input boolean 'enable'.
void setInterrupt(bool enable) {
    if (enable) {
        Machine::enable_interrupts();  // Enable interrupts if 'enable' is true.
    } else {
        Machine::disable_interrupts(); // Disable interrupts if 'enable' is false.
    }
}



void Scheduler::yield() {
//  assert(false);
    // Return immediately if there are no threads ready to run
    if (ready.isempty()) {
        return;
    }

    // Disable interrupts if they are currently enabled
    bool wereInterruptsEnabled = interruptIsOn();
    if (wereInterruptsEnabled) {
        setInterrupt(false);
    }
    
    // Remove the next thread from the ready queue and dispatch to it
    Thread* t = ready.dequeueThread();
    Thread::dispatch_to(t);

    // Restore the previous interrupt state
    if (wereInterruptsEnabled) {
        setInterrupt(true);
    }
}




void Scheduler::resume(Thread * _thread) {
//  assert(false);
  if(interruptIsOn())
  {
    setInterrupt(false);
  }
  ready.enqueueThread(_thread);
  if(!interruptIsOn())
  {
    setInterrupt(true);
  }
}



void Scheduler::add(Thread* _thread) {

//  assert(false);
    // Remember the original state of interrupts
    bool interruptsOriginallyEnabled = interruptIsOn();

    // Disable interrupts if they were originally enabled
    if (interruptsOriginallyEnabled) {
        setInterrupt(false);
    }

    // Add the thread to the ready queue
    ready.enqueueThread(_thread);

    // Re-enable interrupts only if they were enabled originally
    if (interruptsOriginallyEnabled) {
        setInterrupt(true);
    }
}


void Scheduler::terminate(Thread * _thread) {
//  assert(false);
  ready.removeThread(_thread);
}
