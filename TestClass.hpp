#ifndef TESTCLASS_HPP
#define TESTCLASS_HPP

namespace test {
  class TestClass {
    public:
      TestClass();
      TestClass(int i, int j);

      int getI() const;
      bool setI(int i);

      int getJ() const;
      bool setJ(int j);

    private:
      int m_i = 0;
      int m_j = 0;
  };

} // namespace test

#endif // TESTCLASS_HPP
