
# __Task 03 Matrix__

## __Task statement__

### 1. First level - in review

Implement N x N matrix determinant calculation algorithm. Template solution recommended for matrix class, and std containers usage in matrix class implementation is forbidden.

---

## __Implemented features__

### 1. std::vector analog:

Exceptions safe template container which encapsulates dynamically allocated massive is implemented.

This container provides O(1) element access algorithm complexity, push amortized O(1) complexity, and pop O(1) complexity. Iterators are also implemented.

### 2. Linear classes hierarchy:

Linear objects classes hierarchy is implemented. This hierarchy provides IMathVector, IMatrix, and ISquareMatrix template interfaces for linear objects and matrixes.

There are common template implementations for linear objects interfaces: MathVector and SquareMatrix classes.

And all of these implementations are covered with handy and extendable unit tests.
