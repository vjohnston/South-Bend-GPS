#include <iostream>
#include <boost/algorithm/string.hpp>
#include <utility>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <queue>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "street_map.hpp"

using namespace std;
using namespace boost;

street_map::street_map (const string &filename){
	// Declare and open file
	ifstream infile;
	infile.open(filename);

	// declare variables
	string type, line, url, street;
	vector<string> fields;
	int parity, start, end, startnode, endnode;
	float length;
	vector<segment> oddVec, evenVec;

	// read the file until the end is reached
	while(!infile.eof()){
		infile >> type; // get the type (N or R)
		getline(infile,line); // get the rest of the line after the type
		// if the type is N, the rest of the line is the street
		if (type=="N:"){
			street = line.erase(0,1);
			// clear vectors used to store the even and odd segments of the previous street
			oddVec.clear();
			evenVec.clear();
		} else if (type=="R:"){
			// split the rest of the line by spaces and store the parts of the line in the vector fields
			split(fields,line,is_any_of(" \n"));
			// get the correct variables from the vector
			parity = stoi(fields[1]);
			start = stoi(fields[2]);
			end = stoi(fields[3]);
			startnode = stoi(fields[4]);
			endnode = stoi(fields[5]);
			length = stof(fields[6]);
			url = fields[7];
			// create segment and add it to the list of odd or even address of street
			segment seg(start,end,startnode,endnode,length,url);
			if (parity==0){
				evenVec.push_back(seg);
			} else {
				oddVec.push_back(seg);
			}
			// add line to adj matrix
			addEdge(startnode,endnode,street,length);
			addEdge(endnode,startnode,street,length);
		} else if (type=="E:"){
			split(fields,line,is_any_of(" \n"));
			// get correct values for vector
			startnode = stoi(fields[1]);
			endnode = stoi(fields[2]);
			length = stof(fields[3]);
			url = fields[4];
			// add line to adj matrix
			addEdge(startnode,endnode,street,length);
			addEdge(endnode,startnode,street,length);
		}
		// if the odd or even vectors aren't empty, add them to the map
		if (!oddVec.empty()){
			side sd(street,1);
			map[sd] = oddVec;
		}
		if (!evenVec.empty()){
			side sd(street,0);
			map[sd] = evenVec;
		}
		// clear the vector storing all the fields
		fields.clear();
	}
}

bool street_map::geocode(const string &address, int &u, int &v, float &pos) const {
	// pre condition- adddress cannot be NULL
	if (address.empty()){
		return false;
	}

	// declare variables
	string n, street;
	vector<segment> segVec;
	int num;

	// declare sstream and read
	istringstream iss(address);
	iss >> n;
	getline(iss,street);
	street.erase(0,1);
	// get parity bit and create key (side)
	num = stoi(n);
	int parity = num%2;
	side s(street,parity);
	// find the key (side) in the map with an iterator
	unordered_map<side,vector<segment>,sideHash>::const_iterator it = map.find(s);
	if (it!=map.end()){
		// if the key is in the map, get the value (vector of segments)
		segVec = it->second;
		// iterate through the vector and see if the address is inbetween the range in segment
		for (int i=0; i<segVec.size(); i++){
			if (segVec[i].start<=num && num<=segVec[i].end){
				// if it is inbetween the range in segment: set u to starting node, v to ending node and calculate pos
				u = segVec[i].startnode;
				v = segVec[i].endnode;
				pos = (float) segVec[i].length * ((float)(num-segVec[i].start)/(float)(segVec[i].end-segVec[i].start+2));
				return true;
			}
		}
		return false;
	} else {
		return false;
	}
}

bool street_map::route3(int source, int target, float &distance) const {
	// create frontier and marked
	priority_queue<Node,vector<Node>,comp> frontier;
	unordered_map<int,int> marked;

	// add first element onto frontier
	Node n(source,0,source);
	frontier.push(n);

	// variable used to add up distances for each step
	float dis;

	// perform dijkstra
	while (frontier.size() > 0) {
		// get front of frontier and pop it
		Node u = frontier.top();
		frontier.pop();

		// check if u.dest is target. If it is end function
		if (u.dest==target){
			distance = u.length;
			return true;
		}

		// check if the destination is in marked
		auto got = marked.find(u.dest);
		if (got==marked.end()){
			// If it is, add u.dest and u.source to marked
			marked[u.dest] = u.source;
			// update distance
			dis = u.length;

			// find neigbours of u
			auto neighbours = adjList.find(u.dest);
			if (neighbours!=adjList.end()){
				// destinations is the unordered map of ints to edges
				auto destinations = neighbours->second;
				// for each neighbour create a Node and push it into the frontier
				for (auto v=destinations.begin(); v!=destinations.end(); ++v){
					// create node
					Node vnode(v->first,(dis+v->second.length),u.dest);
					// check that destination is not already on frontier
					auto got2 = marked.find(v->first);
					if (got2==marked.end()){
						// if it isnt push it on
						frontier.push(vnode);
					}
				}
			}
		}
	}
	return false;
}

float street_map::getDistance(int source, int dest) const{
	float len;
	// find source in the adjacency list
	auto adjSource = adjList.find(source);
	// if source is in the asjacency list, find dest in the unordered_map source maps to
	if (adjSource!=adjList.end()){
		auto sEdge = adjSource->second.find(dest);
		// if dest is in the unordered_map, retrieve the length from the edge class
		if (sEdge!=adjSource->second.end()){
			len = sEdge->second.length;
		}
	}
	// return the length. If not found- this will return zero
	return len;
}

bool street_map::route4(int su, int sv, float spos, int tu, int tv, float tpos, float &distance) const {
	// declare fake source and target ids
	int source = 11111111;
	int target = 00000000;
	// declare frontier and marked. frontier-priority queue and marked- unordered_map
	priority_queue<Node,vector<Node>,comp> frontier;
	unordered_map<int,int> marked;
	// get length of the edges using helper function
	float s1 = getDistance(su,sv);
	float t1 = getDistance(tu,tv);
	// check that the source and target are not on the same destination
	if (su==tu && sv==tv){
		distance = tpos-spos;
		return true;
	}
	// add source to marked- 'fake' source id is 11111111
	marked[source] = source;
	// add su and sv to frontier. Do this by creating a node
	Node sun(su,spos,source);
	Node suv(sv,s1-spos,source);
	frontier.push(sun);
	frontier.push(suv);
	// declare distance
	float dis;
	//while frontier is not empty
	while (frontier.size() > 0){
		// get front of frontier and pop it
		Node u = frontier.top();
		frontier.pop();

		// update distance
		dis = u.length;

		// check if u.dest is target. If it is end function
		if (u.dest==target){
			distance = u.length;
			return true;
		} else if (u.dest==tu){
			// if u.dest is tu then there will be one more step: tu->target. add that step to frontier
			Node tunode(target,dis+tpos,u.dest);
			frontier.push(tunode);
		} else if (u.dest==tv){
			// if u.dest is tv then there will be one more step: tv->target. add that step to frontier
			Node tvnode(target,dis+t1-tpos,u.dest);
			frontier.push(tvnode);
		}

		// check if the destination is in marked
		auto got = marked.find(u.dest);
		if (got==marked.end()){
			// If it is, add u.dest and u.source to marked
			marked[u.dest] = u.source;

			// find neigbours of u
			auto neighbours = adjList.find(u.dest);
			if (neighbours!=adjList.end()){
				// destinations is the unordered map of ints to edges
				auto destinations = neighbours->second;
				// for each neighbour create a Node and push it into the frontier
				for (auto v=destinations.begin(); v!=destinations.end(); ++v){
					// create node
					Node vnode(v->first,(dis+v->second.length),u.dest);
					// check that destination is not already on frontier
					auto got2 = marked.find(v->first);
					if (got2==marked.end()){
						// if it isnt push it on
						frontier.push(vnode);
					}
				}
			}
		}
	}
	return false;
}

// function that returns the streetname between two location ids
string street_map::getStreetname(int u, int v)const{
	// find the source id from the adjacent list
	auto map_it = adjList.find(u);
	if (map_it!=adjList.end()){
		// find dest id from adjacent list
		auto edge_it = map_it->second.find(v);
			// if the dest id exists, return the streetname from edge
			if (edge_it!=map_it->second.end()){
				return edge_it->second.name;
			}
	}
	// else, return a null streetname
	return NULL;
}

bool street_map::route(int su, int sv, float spos, int tu, int tv, float tpos, vector<pair<string, float>> &steps) const{
	steps.clear();
	// declare fake source and target ids
	int source = 11111111;
	int target = 00000000;
	// declare frontier and marked	
	priority_queue<Node,vector<Node>,comp> frontier;
	unordered_map<int,int> marked;
	// get length of the edges
	float s1 = getDistance(su,sv);
	float t1 = getDistance(tu,tv);
	// add source to marked- 'fake' source id is 11111111
	marked[source] = source;
	// add su and sv to frontier
	Node sun(su,spos,source);
	Node suv(sv,s1-spos,source);
	frontier.push(sun);
	frontier.push(suv);
	// get source and dest street names
	string source_street = getStreetname(su,sv);
	string dest_street = getStreetname(tu,tv);
	// make sure source and dest are not in the same segment
	if (su==tu && sv==tv){
		// if they are, add the distance between them to steps and return
		pair<string,float> step_pair;
		step_pair = make_pair(source_street,tpos-spos);
		steps.push_back(step_pair);
		return true;
        }
	// check that su or sv are not the starting node. if they are we don't want to start with the dummy source
	if (spos==0) source = su;
	else if (spos==s1) source = sv;
	// declare distance
	float dis;
	// declare step- this is used in the 2nd while loop to go through all the steps
	int step = target;
	//while frontier is not empty
	while (frontier.size() > 0){
		// get front of frontier and pop it
		Node u = frontier.top();
		frontier.pop();

		// update distance
		dis = u.length;
		// check if u.dest is target. If it is end function
		if (u.dest==target){
			marked[u.dest] = u.source;
			step = u.source;
			break;
		} else if (u.dest==tu){
			// if u.dest is tu and tu is the target-> treat this like the first case and break
			if (tpos==0) {
				target = tu;
				step = tu;
				marked[u.dest] = u.source;
				break;
			}
			// otherwise, there will be one more step from tu to dummy target. add that step to frontier
			Node tunode(target,dis+tpos,u.dest);
			frontier.push(tunode);
		} else if (u.dest==tv){
			// if u.dest is tv and tv is the target-> treat this like the first case and break
			if (tpos==t1) {
				marked[u.dest] = u.source;
				step = tv;
				target = tv;
				break;
			}
			// otherwise, there will be one more step from tv to dummy target. add that step to frontier
			Node tvnode(target,dis+t1-tpos,u.dest);
			frontier.push(tvnode);
		}

		// check if the destination is in marked
		auto got = marked.find(u.dest);
		if (got==marked.end()){
			// If it is, add u.dest and u.source to marked
			marked[u.dest] = u.source;

			// find neigbours of u
			auto neighbours = adjList.find(u.dest);
			if (neighbours!=adjList.end()){
				// destinations is the unordered map of ints to edges
				auto destinations = neighbours->second;
				// for each neighbour create a Node and push it into the frontier
				for (auto v=destinations.begin(); v!=destinations.end(); ++v){
					// create node
					Node vnode(v->first,(dis+v->second.length),u.dest);
					// check that destination is not already on frontier
					auto got2 = marked.find(v->first);
					if (got2==marked.end()){
						// if it isnt push it on
						frontier.push(vnode);
					}
				}
			}
		}
	}
	// declare the distance- used to add up the distance of all the segments on same street
	float distance = 0;
	// make sure marked has some directions in it
	if (marked.size()){
		// set the starting street name to the destination street name.
		// This part of the code starts will the target and uses marked to work back to the source.
		string streetname = dest_street;
		// from dijkstra's algorithm step starts as the next step. This has to be tu or tv.
		// if it is unchanged and is still target then no path was found.
		if (step!=0){
			// now start going through marked to find the path.
			while (step!=source){
				// find step in marked
				auto marked_it = marked.find(step);
				if (marked_it!=marked.end()){
					// if that step existed, find step in the adjacency list
					auto map_it = adjList.find(step);
					if (map_it!=adjList.end()){
						// if step was found in adjacency list, get the edge between step and the next step
						auto edge_it = map_it->second.find(marked_it->second);
						if (edge_it!=map_it->second.end() || marked_it->second==source){
							// if the next step is the source
							if (marked_it->second==source){
								// add the last pair from step to source into steps
								pair<string,float> step_pair;
								if (step==su) step_pair = make_pair(source_street,spos);
								else step_pair = make_pair(source_street,s1-spos);
								steps.push_back(step_pair);
							// if the next step is not the source
							} else {
								// find out what the step after the next step is
								auto next = marked.find(marked_it->second);
								// add the length between step and next step to the distance
								distance += edge_it->second.length;
								// find out what the nextstreet name is
								string nextstreet;
								if (next->second==source) nextstreet = source_street;
								else nextstreet = getStreetname(marked_it->second,next->second);
								// if the current streetname is not the same as the next streetname
								if (streetname != nextstreet){
									// create a pair and add that street and distance into steps
									pair<string,float> step_pair;
									step_pair = make_pair(streetname,distance);
									steps.push_back(step_pair);
									// set the starting distance for the nextstreet to 0
									distance = 0;
									// set the streetname to the name of the next street
									streetname = nextstreet;
								}
							}
						}
					}
					// set step equal to the next steps
					step = marked_it->second;
				}
			}
		}
		// Because we worked from target->source we need to reverse steps
		reverse(steps.begin(),steps.end());
		return true;
	}
	return false;
}
