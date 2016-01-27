#ifndef STREET_MAP_HPP
#define STREET_MAP_HPP

#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <utility>
#include <cstdlib>
#include <functional>
#include <unordered_map>

using namespace std;
using namespace boost;

// struct for node- used by frontier. Stores the source,dest ids and the length
typedef struct Node {
	int dest;
	float length;
	int source;
	Node():dest(),length(),source(){};
	Node(int d,float l,int s):dest(d),length(l),source(s){};
	// equal operator needed for comparison
	bool operator==(const Node &other) const {
		return (dest==other.dest && length==other.length && source==other.source);
	}
} Node;
	
// comparison for Node class. Orders nodes in priority queue from smallest to largest length
struct comp:public std::binary_function<Node, Node, bool>
{
    	bool operator()(const Node lhs, const Node rhs) const
    	{
        	return lhs.length > rhs.length;
   	}
};

// Main class used. This class contains all the main functions used in street_map.cpp
class street_map {
	public:
		// Constructor- it takes the pathname of a file to be read into a map as an input
		explicit street_map (const std::string &filename) ;

		// Geocodes an address. Takes the address as a string as an input. 
		//Outputs are the starting node of edge(u), the ending node of edge(v) and the length of the edge (pos)
		bool geocode(const std::string &address, int &u, int &v, float &pos) const;

		// function finding the shortest distance from source node to target node
		bool route3(int source, int target, float &distance) const;

		// Helper function used to get the distance between two nodes from the adjacency list.
		float getDistance(int source,int dest) const;

		// function that finds the shortest distance from source (between su and sv) and target node (between tu and tv)
		bool route4(int su, int sv, float spos, int tu, int tv, float tpos, float &distance) const;

		// Helper function that gets the streetname between two nodes
		string getStreetname(int u, int v) const;

		// function similar to route4 but returns all the directions used to compute the distance in a vector of steps
		bool route(int su, int sv, float spos, int tu, int tv, float tpos, std::vector<std::pair<std::string, float>> &steps)const;

	private:
		// side of street that stores the street name and parity bit (indicates odd or even house #)
		typedef struct side {
			string streetname;
			int parity;
			side(string s,int p):streetname(s),parity(p){};
			bool operator==(const side &other) const {
				return (streetname==other.streetname && parity==other.parity);
			}
		} side;
		// street segment- contains the starting and ending house # in segment and URL of segment
		typedef struct segment {
			int start;
			int end;
			int startnode;
			int endnode;
			float length;
			string URL;
			segment(int s,int e,int sn,int en,float len,string u):start(s),end(e),startnode(sn),endnode(en),length(len),URL(u){};
		} segment;
		// hash function used for the struct side
		typedef struct sideHash {
			size_t operator()(const side& s) const {
				return ((hash<string>()(s.streetname))^(hash<int>()(s.parity)<<1));
			}
		} sideHash;
		// edge struct- stores length and name of street
		typedef struct edge {
			string name;
			float length;
			edge():name(),length(){};
			edge(string n,float l):name(n),length(l){};
		} edge;
		// adjecency list
		unordered_map<int,unordered_map<int,edge>> adjList;
		// function which adds edge to the adjacent list
		void addEdge(int source,int dest,string streetname, float len){
			edge newedge(streetname,len);
			auto it = adjList.find(source);
			if (it!=adjList.end()){
				// if there is already an unordered map,add to it
				it->second[dest] = newedge;
			} else {
				// if there is not, create one and add first valye
				unordered_map<int,edge> destinations;
				destinations[dest] = newedge;
				adjList[source] = destinations;
			}
		}
		// initialization of unordered map. key=side. value=vector<segment>. sideHash and == needed for this.
		unordered_map<side,vector<segment>,sideHash> map;
};

#endif
