
#include <Rcpp.h>
using namespace Rcpp;

struct Comp{
  Comp(const Rcpp::NumericVector& v ) : _v(v) {}
  // Inverted comparison!
  bool operator ()(int a, int b) { return _v[a] > _v[b]; }
  const Rcpp::NumericVector& _v;
};

template <class T>
T _divide_by_rexp(T t) { return t / Rf_rexp(1.0); }

template <class T>
T _add_one(T t) { return t + 1; }

// [[Rcpp::export(sample.int.crank)]]
IntegerVector sample_int_crank(int n, int size, NumericVector prob) {
  // We need the last "size" elements of
  // U ^ (1 / prob) ~ log(U) / prob
  //                ~ -Exp(1) / prob
  //                ~ prob / Exp(1)
  // Here, ~ means "doesn't change order statistics".
  NumericVector rnd = NumericVector(prob.begin(), prob.end(),
    &_divide_by_rexp<double>);
  
  // Find the indexes of the first "size" elements under inverted
  // comparison.  Here, vx is zero-based.
  IntegerVector vx = seq(0, n - 1);
  std::partial_sort(vx.begin(), vx.begin() + size, vx.end(), Comp(rnd));
  
  // Initialize with elements vx[1:size], applying transform "+ 1" --
  // we return one-based values.
  return IntegerVector(vx.begin(), vx.begin() + size, &_add_one<int>);
}
