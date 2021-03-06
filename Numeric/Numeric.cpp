// Numeric.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<iomanip>
#include<random>
#include<functional>
#include<complex>
#include "./Matrix.h"
#include "./MatrixIO.h"

using namespace std;
using namespace Numeric_lib;

void f1() {
	cout << "sizeof(int): " << sizeof(int) << '\t' << "sizeof(float): " << sizeof(float) << endl;
	//int x = 2100000009;
	int x = 2100009;
	float f = x;
	cout << x << '\t' << f << endl;
	cout << setprecision(15) << x << '\t' << f << endl;
}

void f2() {
	cout << "largest int: " << INT_MAX << endl;
	cout << "smallest int: " << numeric_limits<int>::min() << endl;

	if (numeric_limits<char>::is_signed)
		cout << "char is signed" << endl;
	else
		cout << "char is unsigned" << endl;

	char ch = numeric_limits<char>::min();
	cout << "the char with the smallest positive value: " << ch << endl;
	cout << "the int value of the char with the smallest positive value: " << int(ch) << endl;
}

void f3_1(int n1, int n2, int n3) {
	Matrix<double, 1> ad1(n1);
	Matrix<int, 1> ai1(n1);
	ad1(7) = 0;
	ad1[7] = 8;

	Matrix<double, 2> ad2(n1, n2);
	Matrix<double, 3> ad3(n1, n2, n3);
	ad2(3, 4) = 7.5;
	ad3(3, 4, 5) = 9.2;
}

void f3() {
	f3_1(40, 58, 63);
}

namespace NS1 {
	void init(Matrix<int, 2>& a) {
		for (int i = 0; i < a.dim1(); i++)
			for (int j = 0; j < a.dim2(); j++)
				a(i, j) = 10 * i + j;
	}

	void print(const Matrix<int, 2>& a) {
		for (int i = 0; i < a.dim1(); i++) {
			for (int j = 0; j < a.dim2(); j++)
				cout << a(i, j) << '\t';
			cout << endl;
		}
	}

	void m2(int& x) {
		x *= 2;
	}
	void add(int& x, const int y) {
		x += y;
	}
}

void f4() {
	Matrix<int, 2> mtx(3, 4);
	NS1::init(mtx);
	NS1::print(mtx);

	cout << mtx.size() << endl;
	int* p = mtx.data();
	cout << *p << endl;
	
	Matrix<int> a(8);
	for (int i = 0; i < 8; i++) a(i) = i;
	for (int i = 0; i < 8; i++) cout << a(i) << '\t';
	cout << endl;
	a.apply(NS1::m2);
	for (int i = 0; i < 8; i++) cout << a(i) << '\t';
	cout << endl;
	a.apply(NS1::add, 5);
	for (int i = 0; i < 8; i++) cout << a(i) << '\t';
	cout << endl;
	
	double val[] = { 1.2, 2.3, 3.4, 4.5 };
	Matrix<double> data(val);

}

/*
ostream& operator<<(ostream& os, const Matrix<double>& mtx) {
	os << "{\t";
	for (int i = 0; i < mtx.size(); i++) os << mtx(i) << '\t';
	os << "}" << endl;
	return os;
}
*/

// chessboard
void f5() {
	enum Piece { none, pawn, knight, queen, king, bishop, rook };
	Matrix<Piece, 2> board(8, 8);

	const int white_start_row = 0;
	const int black_start_row = 7;

	Piece tmp[] = { rook, knight, bishop, queen, king, bishop, knight, rook };
	Matrix<Piece> start_row(tmp);
	Matrix<Piece> clear_row(8);

	board[white_start_row] = start_row;
	for (int i = 1; i < 7; i++) board[i] = clear_row;
	board[black_start_row] = start_row;

	Matrix<double> a(4);
	cout << a;
}

namespace NS2 {
	typedef Numeric_lib::Matrix<double, 2> Matrix;
	typedef Numeric_lib::Matrix<double, 1> Vector;

	void classical_elimination(Matrix& A, Vector& b) {
		const Index n = A.dim1();

		// traverse from 1st column to the next-to-last
		// filling zeros into all elements under the diagonal
		for (Index j = 0; j < n - 1; j++) {
			const double pivot = A(j, j);
			if (pivot == 0) cerr << "can't solve: pivot == 0";

			// fill zeros into each element under the diagonal of the ith row
			for (Index i = j + 1; i < n; i++) {
				const double mult = A(i, j) / pivot;
				A[i].slice(j) = scale_and_add(A[j].slice(j), -mult, A[i].slice(j));
				b(i) -= mult * b(j);
			}
		}
	}

	void elim_with_partial_pivot(Matrix& A, Vector& b) {
		const Index n = A.dim1();

		for (Index j = 0; j < n; j++) {
			Index pivot_row = j;

			// look for a suitable pivot
			for (Index k = j + 1; k < n; k++)
				if (abs(A(k, j)) > abs(A(pivot_row, j))) pivot_row = k;

			// swap the rows if we found a better pivot
			if (pivot_row != j) {
				A.swap_rows(j, pivot_row);
				swap(b(j), b(pivot_row));
			}

			// elimination
			for (Index i = j + 1; i < n; i++) {
				const double pivot = A(j, j);
				if (pivot == 0) {
					cerr << "can't solve: pivot == 0" << endl;
					exit(-1);
				}
				const double mult = A(i, j) / pivot;
				A[i].slice(j) = scale_and_add(A[j].slice(j), -mult, A[i].slice(j));
				b(i) -= mult * b(j);
			}
		}
	}

	Vector back_substitution(const Matrix& A, const Vector&b) {
		const Index n = A.dim1();
		Vector x(n);

		for (Index i = n - 1; i >= 0; i--) {
			double s = b(i) - dot_product(A[i].slice(i + 1), x.slice(i + 1));

			if (double m = A(i, i))
				x(i) = s / m;
			else {
				cerr << "can't solve";
				exit(-1);
			}
				
		}
		return x;
	}

	Vector classical_gaussian_elimination(Matrix A, Vector b) {
		//classical_elimination(A, b);
		elim_with_partial_pivot(A, b);
		return back_substitution(A, b);
	}

	void solve_random_system() {
		Matrix A(2,2);
		Vector b(2);

		double d0[] = { 2,3 };
		double d1[] = { 4, 9 };
		double b0[] = { 6, 20 };

		A[0] = d0;
		A[1] = d1;

		b = b0;

		Vector x = classical_gaussian_elimination(A, b);

		cout << x << endl;
	}

	
	Vector random_vector(Index n) {
		double max = 100.0;
		Vector v(n);
		default_random_engine ran{};
		uniform_real_distribution<> ureal{ 0, max };
		for (Index i = 0; i < n; i++)
			v(i) = ureal(ran);
		return v;
	}
	

}

namespace NS4 {
	void f1() {
		auto gen = bind(normal_distribution<double>{15, 4.0},
			default_random_engine{});

		vector<int> hist(2 * 15);
		for (int i = 0; i < 500; i++)
			++hist[int(round(gen()))];

		for (int i = 0; i != hist.size(); i++) {
			cout << i << '\t';
			for (int j = 0; j != hist[i]; j++)
				cout << '*';
			cout << endl;
		}
	}
}

namespace NS5 {
	void f1() {
		errno = 0;
		double s2 = sqrt(-1);
		if (errno) cerr << "something went wrong with something somewhere" << endl;
		if (errno == EDOM)
			cerr << "sqrt() not defined for negative argument" << endl;
		
	}
}

namespace NS6 {
	using cmplx = complex<double>;

	void f1() {
		cmplx z1(1, 2);
		cmplx z2 = pow(z1, 2);
		cmplx z3 = z2 * 9.3 + z2 * 3.9;
		cmplx z4 = z1 * 3.0;

		cout << z1 << endl;
		cout << z2 << endl;
		cout << z3 << endl;
		cout << z4 << endl;
	}
}

int main()
{
	NS6::f1();
	//NS5::f1();
	//NS4::f1();
	//NS2::solve_random_system();
	//f5();
	//f4();
	//f3();
	//f2();
	//f1();

	return 0;
}

