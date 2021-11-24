/*
 *
 * File: prefetcher.C
 * Author: Sat Garcia (sat@cs)
 * Description: This simple prefetcher waits until there is a D-cache miss then 
 * requests location (addr + 16), where addr is the address that just missed 
 * in the cache.
 *
 */

#include "prefetcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>

Prefetcher::Prefetcher() { _ready = false; stride = 16;
	last_reference.addr = 0;
	global_stride = 0;  
	global_state = 0; 
}

bool Prefetcher::hasRequest(u_int32_t cycle) {  
	if (!_ready){
		if (stream_buffer[cycle%5] != 0){
			_nextReq.addr = stream_buffer[cycle%5];
			stream_buffer[cycle%5] = 0;
			_ready = true;
			}
	}
	
	return _ready;} 

Request Prefetcher::getRequest(u_int32_t cycle) {
	return _nextReq; }

void Prefetcher::completeRequest(u_int32_t cycle) {

	_ready = false; }



void Prefetcher::cpuRequest(Request req) {  
	bool found_entry = false;
	bool global_bool = false; 
	global_stride = last_reference.addr - req.addr; 

	if(!_ready && !req.HitL1) {
		if (req.addr == last_reference.addr + global_stride){
			printf("global stride correct %d", global_stride);
			if (global_state<4){
			global_state++;
			// global_stride = last_reference.addr - req.addr; 

			}
		}
		else {
			// printf("global stride wrong %d  ", global_stride);
			if(global_state>0){
				global_state--;
				// global_stride = req.addr - last_reference.addr; 
			}
		}
		
		for (int i = 0; i < pre_array.size(); i++){
		if(pre_array[i][0] == last_reference.addr){
			if (req.addr != last_reference.addr + pre_array[i][2]){  //check if current req is equal to the sride + last request
				pre_array[i][2] = req.addr - pre_array[i][1]; // if not update the stride
				if (pre_array[i][3] > 0){ //checks state
					pre_array[i][3]--;
				}
			}
			else{
				if (pre_array[i][3] < 4){
					pre_array[i][3]++;
				}
			}
		}
	}
	for (int i = 0; i < pre_array.size(); i++){
		if (pre_array[i][0] == req.pc){
			pre_array[i][1] = last_reference.addr;
			found_entry = true;
			if(pre_array[i][3] >= 2){
				_nextReq.addr = req.addr + pre_array[i][2];
				_ready = true; 
				// printf("take stride");
				for (int j = 0; j < 5; j++){
					stream_buffer[j] = req.addr + pre_array[i][2] * (j+1);
					}
			}
		}
	}
				if (global_state >= 2){
				_nextReq.addr = req.addr + global_stride;
				_ready = true; 
				for (int j = 0; j < 5; j++){
					stream_buffer[j] = req.addr + (global_stride * (j+1));
					}
				}
				else {
					_nextReq.addr = req.addr + stride;
					_ready = true; 
				for (int j = 0; j < 5; j++){
					stream_buffer[j] = req.addr + (stride * (j+1));
					}
				}
	if (!found_entry){
		std::vector<u_int32_t>temp_array;
		temp_array.push_back(req.pc);
		temp_array.push_back(last_reference.addr);
		temp_array.push_back(stride);
		temp_array.push_back(1);
		pre_array.push_back(temp_array);
		_nextReq.addr = req.addr + stride; 
		_ready = true; 
		// last_reference.addr = req.addr;
	}
	// global_stride = last_reference.addr - req.addr; 
	// last_reference.addr = req.addr; 
	}
	last_reference.addr = req.addr; 

	// global_stride = last_reference.addr - req.addr; 

	}
	
