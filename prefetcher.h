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
	u_int32_t last_miss;
	u_int32_t last_hit;
	u_int32_t stride;
	u_int32_t stream_buffer[10];
	std::vector<u_int32_t> stride_calc;
	u_int32_t l2_stream_array[20];
	std::vector<Request> holding_references;
	int count;
	std::unordered_map<u_int32_t, u_int32_t> map; 
	Request last_reference;
	bool hit_in_map;
	// int count; 
	u_int32_t dist_array[200];
	std::vector<std::vector<u_int32_t> > pre_array;
	u_int32_t global_stride;  
	int global_state; 
	
	

  public:
	Prefetcher();
	int count_nums(){ return count; }
	// should return true if a request is ready for this cycle
	bool hasRequest(u_int32_t cycle);

	// request a desired address be brought in
	Request getRequest(u_int32_t cycle);
	int print_stride(void); 
	// this function is called whenever the last prefetcher request was successfully sent to the L2
	void completeRequest(u_int32_t cycle);
	int stride_check(void);
	void print_references(void);
	/*
	 * This function is called whenever the CPU references memory.
	 * Note that only the addr, pc, load, issuedAt, and HitL1 should be considered valid data
	 */
	void cpuRequest(Request req); 
	// void stateMachine(int state);
};

#endif
