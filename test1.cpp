#include <iostream>
#include <cmath>
#include "street_map.hpp"

using namespace std;

int failures = 0;

#define test_equal(x, y) do { \
  cout << "testing " << #x << " == " << #y << ": "; \
  if (x == y) \
    cout << "PASSED" << endl; \
  else { \
    cout << "FAILED (" << #x << " was " << x << ")" << endl; \
    failures++; \
  } \
} while (0)

#define test_close(x, y) do { \
  cout << "testing " << #x << " == " << #y << ": "; \
  if (fabs(x-y) <= 1e-3) \
    cout << "PASSED" << endl; \
  else { \
    cout << "FAILED (" << #x << " was " << x << ")" << endl; \
    failures++; \
  } \
} while (0)

int main (int argc, char *argv[]) {
  street_map m("southbend.v2.map");
  int u, v;
  float pos;

  // Repeat all tests from PG4

  // Test some famous addresses
  test_equal(m.geocode("1417 E Wayne St", u, v, pos), true);
  test_equal(u, 11395595); test_equal(v, 11395596); test_close(pos, 0.0185837);
  
  test_equal(m.geocode("620 W Washington St", u, v, pos), true);
  test_equal(u, 11393680); test_equal(v, 11394939); test_close(pos, 0.0);

  test_equal(m.geocode("501 W South St", u, v, pos), true);
  test_equal(u, 11395074); test_equal(v, 422741021); test_close(pos, 0.118682);

  test_equal(m.geocode("4477 Progress Dr", u, v, pos), true);
  test_equal(u, 408927857); test_equal(v, 11392224); test_close(pos, 0.0514073);

  // Test boundary conditions
  test_equal(m.geocode("298 Bercliff Dr", u, v, pos), false);
  test_equal(m.geocode("299 Bercliff Dr", u, v, pos), false);
  test_equal(m.geocode("300 Bercliff Dr", u, v, pos), true);
  test_equal(u, 11397628); test_equal(v, 11397633); test_close(pos, 0.0);
  test_equal(m.geocode("301 Bercliff Dr", u, v, pos), true);
  test_equal(u, 11397628); test_equal(v, 11397633); test_close(pos, 0.0);

  test_equal(m.geocode("309 Bercliff Dr", u, v, pos), true);
  test_equal(u, 11397628); test_equal(v, 11397633); test_close(pos, 0.0289044);
  test_equal(m.geocode("311 Bercliff Dr", u, v, pos), true);
  test_equal(u, 11397633); test_equal(v, 11397632); test_close(pos, 0.0);

  test_equal(m.geocode("314 Bercliff Dr", u, v, pos), true);
  test_equal(u, 11397628); test_equal(v, 11397633); test_close(pos, 0.0316142);
  test_equal(m.geocode("316 Bercliff Dr", u, v, pos), true);
  test_equal(u, 11397633); test_equal(v, 11397632); test_close(pos, 0.0);

  test_equal(m.geocode("398 Bercliff Dr", u, v, pos), true);
  test_equal(u, 11397632); test_equal(v, 11397627); test_close(pos, 0.0379592);
  test_equal(m.geocode("399 Bercliff Dr", u, v, pos), true);
  test_equal(u, 11397632); test_equal(v, 11397627); test_close(pos, 0.0379941);
  test_equal(m.geocode("400 Bercliff Dr", u, v, pos), false);
  test_equal(m.geocode("401 Bercliff Dr", u, v, pos), false);

  test_equal(m.geocode("2416 Roger St", u, v, pos), true);
  test_equal(u, 11392712); test_equal(v, 11407827); test_close(pos, 0.0323839);
  test_equal(m.geocode("2418 Roger St", u, v, pos), false);
  test_equal(m.geocode("2448 Roger St", u, v, pos), false);
  test_equal(m.geocode("2450 Roger St", u, v, pos), true);
  test_equal(u, 11407827); test_equal(v, 11392595); test_close(pos, 0.0);

  if (failures == 1)
    cout << "One test FAILED" << endl;
  else if (failures)
    cout << failures << " tests FAILED" << endl;
  else
    cout << "All correctness tests PASSED" << endl;
}
