/*
 int first = 5;
 auto lambda = [=](int x, int z) {
 return x + z + first;
 };
 int(decltype(lambda)::*ptr)(int, int)const = &decltype(lambda)::operator();
 std::cout << "test = " << (lambda.*ptr)(2, 3) << std::endl;
 */

#include <iostream>

// OT => Object Type
// RT => Return Type
// A ... => Arguments
template<typename OT, typename RT, typename ... A>
struct lambda_expression {
  OT _object;
  RT(OT::*_function)(A...)const;

  lambda_expression(const OT & object)
    : _object(object), _function(&decltype(_object)::operator()) {}

  RT operator() (A ... args) const {
    return (_object.*_function)(args...);
  }
};

// With this you can now run captured, noncaptured lambdas, just like you are using the original:

auto capture_lambda() {
  int first = 5;
  auto lambda = [=](int x, int z) {
    return x + z + first;
  };
  return lambda_expression<decltype(lambda), int, int, int>(lambda);
}

auto noncapture_lambda() {
  auto lambda = [](int x, int z) {
    return x + z;
  };
  return lambda_expression<decltype(lambda), int, int, int>(lambda);
}

void refcapture_lambda() {
  int test;
  auto lambda = [&](int x, int z) {
    test = x + z;
  };
  lambda_expression<decltype(lambda), void, int, int>f(lambda);
  f(2, 3);

  std::cout << "test value = " << test << std::endl;
}

int main(int argc, char **argv) {
  auto f_capture = capture_lambda();
  auto f_noncapture = noncapture_lambda();

  std::cout << "main test = " << f_capture(2, 3) << std::endl;
  std::cout << "main test = " << f_noncapture(2, 3) << std::endl;

  refcapture_lambda();

  return 0;
}
