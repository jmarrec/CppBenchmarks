#include "TestClass.hpp"

namespace test {

  TestClass::TestClass() { }

  TestClass::TestClass(int i, int j)
    : m_i(i), m_j(j) { }

  int TestClass::getI() const {
    return m_i;
  }

  bool TestClass::setI(int i) {
    m_i = i;
    return true;
  }

  int TestClass::getJ() const {
    return m_j;
  }

  bool TestClass::setJ(int j) {
    m_j = j;
    return true;
  }
}

