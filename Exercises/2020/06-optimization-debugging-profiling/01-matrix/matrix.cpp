#include "matrix.hpp"

#include <algorithm>
#include <cassert>
#include <ctime>


matrix
matrix::transpose() const
{
  return *this;
}

matrix operator*(const matrix &A, const matrix &B)
{
  return A;
}

void
matrix::solve(matrix &rhs)
{
  unsigned int ii, jj, kk;
  double       f;

  // Factorize
  if (!factorized)
    {
      factorize();
      std::cout << "factorize !" << std::endl;
    }

  double *b = rhs.get_data();
  // Do Forward Substitution
  std::cout << "fwdsub !" << std::endl;
  for (ii = 0; ii < get_rows(); ++ii)
    {
      f = b[p[ii]];
      for (kk = 0; kk < ii; ++kk)
        f -= index(p[ii], kk) * b[p[kk]];
      b[p[ii]] = f;
    }

  // Do Backward Substitution
  std::cout << "bwdsub !" << std::endl;
  for (jj = 1; jj <= get_rows(); ++jj)
    {
      ii = get_rows() - jj;
      f  = b[p[ii]];
      for (kk = ii + 1; kk < get_cols(); ++kk)
        f -= index(p[ii], kk) * b[p[kk]];
      b[p[ii]] = f / index(p[ii], ii);
    }
}

void
matrix::factorize()
{
  p.resize(rows, 0);
  for (unsigned int ii = 0; ii < rows; ++ii)
    p[ii] = ii;

  int    m = this->get_rows();
  int    n = this->get_cols();
  int    ii, jj, kk;
  double pivot = 0., maxpivot = 0.;
  int    imaxpivot = 0;

  assert(m == n);
  for (ii = 0; ii < m - 1; ++ii)
    {
      maxpivot  = index(p[ii], ii);
      imaxpivot = ii;
      for (kk = ii + 1; kk < m; ++kk)
        if (index(p[kk], ii) > maxpivot)
          {
            maxpivot  = index(p[kk], ii);
            imaxpivot = kk;
          }

      if (imaxpivot != ii)
        std::swap(p[ii], p[imaxpivot]);

      pivot = index(p[ii], ii);
      for (jj = ii + 1; jj < m; ++jj)
        {
          index(p[jj], ii) = index(p[jj], ii) / pivot;

          for (kk = ii + 1; kk < n; ++kk)
            index(p[jj], kk) += -index(p[ii], kk) * index(p[jj], ii);
        }
    }
  factorized = true;
}