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
    cout << "FAILED (" << #x << " was " << (x) << ")" << endl;	\
    failures++; \
  } \
} while (0)

#define test_close(x, y) do { \
  cout << "testing " << #x << " == " << #y << ": "; \
  if (fabs((x)-(y)) <= 1e-3)			    \
    cout << "PASSED" << endl; \
  else { \
    cout << "FAILED (" << #x << " was " << (x) << ")" << endl;	\
    failures++; \
  } \
} while (0)

int main (int argc, char *argv[]) {
  street_map m("southbend.v2.map");
  int su, sv, tu, tv;
  float spos, tpos;
  vector<pair<string, float>> steps;
  
  m.geocode("1417 E Wayne St", su, sv, spos);
  m.geocode("1419 E Wayne St", tu, tv, tpos);
  test_equal(m.route(su, sv, spos, tu, tv, tpos, steps), true);
  test_equal(steps.size(), 1);
  if (steps.size() == 1) {
    test_equal(steps[0].first, "E Wayne St");
    test_close(steps[0].second, 0.00232296);
  }

  m.geocode("1417 E Wayne St", su, sv, spos);
  m.geocode("620 W Washington St", tu, tv, tpos);
  test_equal(m.route(su, sv, spos, tu, tv, tpos, steps), true);
  test_equal(steps.size(), 11);
  if (steps.size() == 11) {
    test_equal(steps[0].first, "E Wayne St");
    test_close(steps[0].second, 0.0185837);
    test_equal(steps[1].first, "E Wayne St S");
    test_close(steps[1].second, 0.342674);
    test_equal(steps[2].first, "E Wayne St");
    test_close(steps[2].second, 0.0356553);
    test_equal(steps[10].first, "W Washington St");
    test_close(steps[10].second, 0.0275086);
  }

  m.geocode("620 W Washington St", su, sv, tpos);
  m.geocode("4477 Progress Dr", tu, tv, tpos);
  test_equal(m.route(su, sv, spos, tu, tv, tpos, steps), true);
  test_equal(steps.size(), 7);
  if (steps.size() == 7) {
    test_equal(steps[0].first, "W Washington St");
    test_equal(steps[1].first, "Laporte Ave");
    test_equal(steps[2].first, "Wilber St");
    test_equal(steps[6].first, "Progress Dr");
  }

  m.geocode("4477 Progress Dr", su, sv, tpos);
  m.geocode("1417 E Wayne St", tu, tv, spos);
  test_equal(m.route(su, sv, spos, tu, tv, tpos, steps), true);
  test_equal(steps.size(), 16);
  if (steps.size() == 16) {
    test_equal(steps[0].first, "Progress Dr");
    test_equal(steps[1].first, "Commerce Dr");
    test_equal(steps[2].first, "Maplewood Ave");
    test_equal(steps[15].first, "E Wayne St");
  }

  if (failures == 1)
    cout << "One test FAILED" << endl;
  else if (failures)
    cout << failures << " tests FAILED" << endl;
  else
    cout << "All correctness tests PASSED" << endl;
}
