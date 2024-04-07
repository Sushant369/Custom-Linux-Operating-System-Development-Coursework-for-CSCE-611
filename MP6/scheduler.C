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

threadQueue * threadQueue::head;
threadQueue * threadQueue::tail;
/* -- (none) -- */

void threadQueue::enqueue(Thread *tid){
    threadQueue *temp = new threadQueue(tid);

    if(threadQueue::tail!=nullptr)
    {
        threadQueue::tail->next = temp;
        threadQueue::tail = threadQueue::tail->next;
        return;
    
    }
    threadQueue::head = temp;
    threadQueue::tail = threadQueue::head;

    
}

Thread * threadQueue::rem() {
    threadQueue *first = threadQueue::head;
    threadQueue::head = first->next;
    Thread * t = first->thread;
    delete (void *)first;
    return t;
}

bool threadQueue::empty() {
    if(threadQueue::head)
    {
        return false;
    }
    return true;
}

void threadQueue::del(Thread *t) {

    
    
    if (threadQueue::head->thread == t) 
    {
        threadQueue::head == threadQueue::head->next;
        return;
    }
    threadQueue *cur = threadQueue::head;
    threadQueue *prev = nullptr;
    bool found=false;
    while(cur && !found) 
    {
        if (t == cur->thread) 
        {    
            if (threadQueue::tail == cur) 
            {
                threadQueue::tail == prev;
            }
            prev->next = cur->next;
            delete (void *)cur;
            found=true;
        }
        prev = cur;
        cur = cur->next;
    }
}


/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() {
    Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
    if (!ready.empty()) 
    {
        Thread * t = ready.rem();
        Thread::dispatch_to(t);
    }
}

void Scheduler::resume(Thread * t) {
    add(t);
}

void Scheduler::add(Thread * t) {
    ready.enqueue(t);
}

void Scheduler::terminate(Thread * t) {
    ready.del(t);
}
