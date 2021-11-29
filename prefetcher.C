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
	for (int i = 0; i < 16; i++){
		global_table.push_back(1);
		global_stride_table.push_back(stride);
	}
}

bool Prefetcher::hasRequest(u_int32_t cycle) {  
	if (!_ready){
			_nextReq.addr = stream_buffer[cycle%5];
			_ready = true;
			}

	return _ready;} 

Request Prefetcher::getRequest(u_int32_t cycle) {
	return _nextReq; }

void Prefetcher::completeRequest(u_int32_t cycle) {
	_ready = false; }



void Prefetcher::cpuRequest(Request req) {  
	bool found_entry = false;
	if (req.addr == last_reference.addr + global_stride_table[global_state]){
			if (global_table[global_state] < 4){ //check if global state needs t be updated
				global_table[global_state]++;
			} //decrement and increment accordingly 
			// if (global_state<15){
			// global_state++;
			// }
			if (global_state >= 8){
				global_state -= 8;
			}
			global_state *= 2;
			global_state += 1;
			
		}
	else {//if not correct the global state is updated
			global_stride_table[global_state] = req.addr - last_reference.addr;
			if (global_table[global_state] > 0){ // 
				global_table[global_state]--;
			}
			// if(global_state>0){
			// 	global_state--;
			// }
			if (global_state >= 8){
				global_state -= 8;
			}
			global_state *= 2;
			// global_state += 1;
		}
	if(!_ready && !req.HitL1) {		
		for (int i = 0; i < pre_array.size(); i++){
		if(pre_array[i][0] == last_reference.addr){
			if (req.addr != last_reference.addr + pre_array[i][2]){   //check if current req is equal to the sride + last request
				pre_array[i][2] = req.addr - pre_array[i][1]; // if not update the stride
				if (pre_array[i][3] > 0){ //checks state
					pre_array[i][3]--;
				}
			}//updates the local stride
			else{
				if (pre_array[i][3] < 4){
					pre_array[i][3]++;
				}
			}
		}
	}
	for (int i = 0; i < pre_array.size(); i++){
		if (pre_array[i][0] == req.addr){
			pre_array[i][1] = last_reference.addr;
			found_entry = true;
			if(pre_array[i][3] >= 2){
				_nextReq.addr = req.addr + pre_array[i][2];//actuall does the prefetching
				_ready = true; 
				for (int j = 0; j < 5; j++){
					stream_buffer[j] = req.addr + pre_array[i][2] * (j+1);  //creating a stream buffer using the local stride
					}
			}
		}
	}			//if not found in references global fetcher is used
				if (global_table[global_state] >= 2 && !found_entry)
				{
				_nextReq.addr = req.addr + global_stride_table[global_state];//global_stride;
				_ready = true; 
					for (int j = 0; j < 5; j++){
						stream_buffer[j] = req.addr + (global_stride_table[global_state] * j+1);//(global_stride * (j+1));
					}
				}
				else {//if global state is Not taken then a standard stride is used
					_nextReq.addr = req.addr + stride;
					_ready = true; 
					for (int j = 0; j < 5; j++){
						stream_buffer[j] = req.addr + (stride * (j+1)); //stream_buffer is populated
					}
				}
	if (!found_entry){ //push new reference to the vector if not found
		if (pre_array.size() > 200){
			pre_array.erase(pre_array.begin());
		}
		std::vector<u_int32_t>temp_array;
		temp_array.push_back(req.addr);
		temp_array.push_back(last_reference.addr);
		temp_array.push_back(stride);
		temp_array.push_back(0);
		pre_array.push_back(temp_array);
		}

	}
	last_reference.addr = req.addr; //last_reference is updated. 
	}
	
