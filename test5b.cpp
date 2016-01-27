#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "street_map.hpp"

using namespace std;

int failures = 0;

int main (int argc, char *argv[]) {
  street_map m("southbend.v2.map");
  int su, sv, tu, tv;
  float spos, tpos;
  vector<pair<string, float>> steps;

  cout << "computing routes for points from random.txt\n";
  ifstream r("random.txt");
  if (!r) {
    cerr << "could not open random.txt\n";
    exit(1);
  }
  string line;
  while (getline(r, line)) {
    istringstream iss(line);
    iss >> su >> sv >> spos >> tu >> tv >> tpos;
    m.route(su, sv, spos, tu, tv, tpos, steps);
  }

}
