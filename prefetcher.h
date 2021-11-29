/*
 *
 * File: prefetcher.h
 * Author: Sat Garcia (sat@cs)
 * Description: Header file for prefetcher implementation
 *
 */

#ifndef PREFETCHER_H
#define PREFETCHER_H

#include <sys/types.h>
#include "mem-sim.h"
#include <vector>
#include <unordered_map>

class Prefetcher {
  private:
	bool _ready;
	Request _nextReq;
	u_int32_t stride;
	u_int32_t stream_buffer[10];
	Request last_reference;
	std::vector<std::vector<u_int32_t> > pre_array;
	u_int32_t global_stride;  
	int global_state; 
	std::vector<int>global_table;
	std::vector<int>global_stride_table;
	
	

  public:
	Prefetcher();

	// should return true if a request is ready for this cycle
	bool hasRequest(u_int32_t cycle);

	// request a desired address be brought in
	Request getRequest(u_int32_t cycle);
	\
	// this function is called whenever the last prefetcher request was successfully sent to the L2
	void completeRequest(u_int32_t cycle);
	/*
	 * This function is called whenever the CPU references memory.
	 * Note that only the addr, pc, load, issuedAt, and HitL1 should be considered valid data
	 */
	void cpuRequest(Request req); 
	// void stateMachine(int state);
};

#endif
