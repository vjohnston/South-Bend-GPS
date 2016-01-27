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
  int su, sv, tu, tv;
  float spos, tpos;
  float distance;
  
  m.geocode("1417 E Wayne St", su, sv, spos);
  m.geocode("1419 E Wayne St", tu, tv, tpos);
  test_equal(m.route4(su, sv, spos, tu, tv, tpos, distance), true);
  test_close(distance, 0.00232296);

  m.geocode("1417 E Wayne St", su, sv, spos);
  m.geocode("620 W Washington St", tu, tv, tpos);
  test_equal(m.route4(su, sv, spos, tu, tv, tpos, distance), true);
  test_close(distance, 1.88365);

  m.geocode("620 W Washington St", su, sv, tpos);
  m.geocode("4477 Progress Dr", tu, tv, tpos);
  test_equal(m.route4(su, sv, spos, tu, tv, tpos, distance), true);
  test_close(distance, 3.38777);

  m.geocode("4477 Progress Dr", su, sv, tpos);
  m.geocode("1417 E Wayne St", tu, tv, spos);
  test_equal(m.route4(su, sv, spos, tu, tv, tpos, distance), true);
  test_close(distance, 5.14334);

  if (failures == 1)
    cout << "One test FAILED" << endl;
  else if (failures)
    cout << failures << " tests FAILED" << endl;
  else
    cout << "All correctness tests PASSED" << endl;
}
