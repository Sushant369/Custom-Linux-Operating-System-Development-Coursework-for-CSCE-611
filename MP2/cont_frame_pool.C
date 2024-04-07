/*
 File: ContFramePool.C
 
 Author:
 Date  : 
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/
#define FREE       0x00
#define USED       0x01
#define HOS       0x02
#define UNAVAILABLE       0x03

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

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
 ContFramePool* ContFramePool::start;
 ContFramePool* ContFramePool::end;
/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no)
{
    // TODO: IMPLEMENTATION NEEEDED!
    Console::puts("ContframePool::Constructor implemented!\n");

	baseFrameNumber=_base_frame_no;
	frames=_n_frames;
	numberOfFreeFrames=_n_frames;
	infoFrameNo=_info_frame_no;
	
    //assert(false);
	
	if(infoFrameNo ==0)
	{
		bitMap = (unsigned char *) (baseFrameNumber * FRAME_SIZE);
	}
	else
	{

		bitMap = (unsigned char *) (infoFrameNo * FRAME_SIZE);
	}
	
	for (int i=0;i<numberOfFreeFrames;i++)
	{
		set_state(i,FrameState::Free);
	}
		
	if(infoFrameNo==0)
	{
		set_state(0,FrameState::HoS);
		numberOfFreeFrames--;
	}

	if(ContFramePool::end!=NULL)
	{
		ContFramePool::end->next=this;
		ContFramePool::end=ContFramePool::end->next;
	}
	else
	{
		ContFramePool::start=this;
		ContFramePool::end=this;
	}

}

unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
    unsigned long strt=0;	
    unsigned int freeBlock=0;
    // TODO: IMPLEMENTATION NEEEDED!
    Console::puts("ContframePool::get_frames implemented!\n");
	//assert(false);
    bool flag1;

    for(unsigned long i=0; i<frames;i++)
    {
	//	Console::puts("free cont block");
	//  Console::puti(strt);
	// 	Console::puts("\n");
	    if(get_state(i)!=FrameState::Free)
	    {
		    freeBlock=0;
			flag1=false;
	    }
	    else
	    {
		    freeBlock++;
		    if(freeBlock==1)
		    {
			    strt=i;
				//Console::puti(strt);
		    }
		    if(freeBlock==_n_frames)
		    {
			    flag1=true;
			    break;
	}}}
	
	
    if(flag1)
    {
    	set_state(strt,FrameState::HoS);
	    numberOfFreeFrames-=_n_frames;
		
	for(unsigned long i=strt+1;i<strt+ _n_frames; i++)
	 {
		 set_state(i,FrameState::Used);
	 }
	 return strt+baseFrameNumber;
    }
    return 0;
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{

    // TODO: IMPLEMENTATION NEEEDED!
    Console::puts("ContframePool::mark_inaccessible implemented!\n");
   // assert(false);
	unsigned long idx=_base_frame_no;
	for(unsigned long i=0;i<_n_frames;i++)
	{
		//Console::puts("marking memory as Unavailable for use");
		set_state(i+_base_frame_no,FrameState::Unavailable);
	}

	numberOfFreeFrames = numberOfFreeFrames - _n_frames;
	//	Console::puts("updated free frames");
	//  Console::puti(numberOfFreeFrames);
	// 	Console::puts("\n");	
}

void ContFramePool::release_frames(unsigned long _first_frame_no)
{
    // TODO: IMPLEMENTATION NEEEDED!
   Console::puts("ContframePool::release_frames  implemented!\n");
    //assert(false);
	bool flag2;
    unsigned long indx;
    ContFramePool* frame_pool = ContFramePool::start;
		
    while(frame_pool!=NULL)
    {
            if(_first_frame_no>= frame_pool->baseFrameNumber && _first_frame_no < frame_pool->baseFrameNumber + frame_pool->frames)
            {
                indx= _first_frame_no - frame_pool->baseFrameNumber;
                flag2=true;
                break;
            }
			else
			{
				flag2=false;
			}
			
            frame_pool=frame_pool->next;
    }
	
	if(flag2)
	{
	assert(frame_pool->get_state(indx) == FrameState::HoS);
    
	frame_pool->set_state(indx,FrameState::Used);

    while(frame_pool->get_state(indx) == FrameState::Used)
    {
            frame_pool->set_state(indx++,FrameState::Free);
            frame_pool->numberOfFreeFrames++;
    }
    return;
	}
	else
	{
		return;
	}

}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
	unsigned long num=0;
    // TODO: IMPLEMENTATION NEEEDED!
    Console::puts("ContframePool::need_info_frames implemented!\n");
    //assert(false);
	if( (_n_frames % (16 K))!=0)
		num++;
	num = num + _n_frames/ (16 K);
	return num;
}


ContFramePool::FrameState ContFramePool::get_state(unsigned long _frame_no)
{
    // Calculate the index within the bitmap array
    unsigned int m = _frame_no / 4;
   
    // Calculate the bit shift to isolate the frame state within the byte
    unsigned int shift = (_frame_no % 4) * 2;
   
    // Create a mask to extract the frame state
    unsigned char state_mask = 0x3 << shift;
   
    // Initialize the frame state to Free
    FrameState state = FrameState::Free;
   
    // Retrieve the frame state from the bitmap
    unsigned char frame_state = (bitMap[m] & state_mask) >> shift;
   
    // Determine and set the corresponding FrameState
    switch (frame_state)
    {
        case USED:
            state = FrameState::Used;
            break;
        case FREE:
            state = FrameState::Free;
            break;
        case HOS:
            state = FrameState::HoS;
            break;
        case UNAVAILABLE:
            state = FrameState::Unavailable;
            break;
        default:
            // Handle an unknown frame state if necessary
            break;
    }
   
    return state;
}




void ContFramePool::set_state(unsigned long frame_no, FrameState frameState)
{
    // Calculate the index and shift values
    unsigned long frameNum = frame_no;
    unsigned int j = frameNum / 4;
    unsigned int shift = (frameNum % 4) * 2;
   
    // Calculate the frame state value based on frameState
    unsigned char frame_state = 0;
    switch (frameState)
    {
        case FrameState::Used:
            frame_state = (unsigned char)USED << shift;
            break;
        case FrameState::Free:
            frame_state = (unsigned char)FREE << shift;
            break;
        case FrameState::HoS:
            frame_state = (unsigned char)HOS << shift;
            break;
        case FrameState::Unavailable:
            frame_state = (unsigned char)UNAVAILABLE << shift;
            break;
        default:
            // Handle an invalid frameState
            break;
    }
   
    // Update the bitMap with the new frame state
    bitMap[j] = ((bitMap[j] & ~(0x3 << shift)) | frame_state);
}
