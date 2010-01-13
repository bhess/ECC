/*
 * adicops.cpp
 *
 *  Created on: Dec 15, 2009
 *      Author: bhess
 */

#include "adicops.h"
#include <iostream>
#include <cassert>

adicops::adicops() {

}

void adicops::do_s() {

	/*
	Poly M(8);
	M.set_coeff(0, 1);
	M.set_coeff(2, 1);
	M.set_coeff(3, 1);
	M.set_coeff(4, 1);
	M.set_coeff(8, 1);

	M.print();

	Poly TmM = get_teichmuller_modulus(M, 10);
	TmM.print();
	//TmM = TmM.reverse();
	//TmM.print();

	Poly A(14);
	A.set_coeff(14, 559);
	A.set_coeff(13, 781);
	A.set_coeff(12, 763);
	A.set_coeff(11, 684);
	A.set_coeff(10, 133);
	A.set_coeff(9, 375);
	A.set_coeff(8, 922);
	A.set_coeff(7, 776);
	A.set_coeff(6, 452);
	A.set_coeff(5, 214);
	A.set_coeff(4, 313);
	A.set_coeff(3, 148);
	A.set_coeff(2, 646);
	A.set_coeff(1, 428);
	A.set_coeff(0, 168);
	A.print();

	Poly rA = poly_remainder(A, TmM, 10);
	cout << "rA: "; rA.print();

	A = Poly(7);
	A.set_coeff(7, 982);
	A.set_coeff(6, 303);
	A.set_coeff(5, 724);
	A.set_coeff(4, 458);
	A.set_coeff(3, 918);
	A.set_coeff(2, 423);
	A.set_coeff(1, 650);
	A.set_coeff(0, 591);
	A.print();

	Poly invA = get_inverse(A, TmM, 10);
	invA.print();

	Poly AA(7);
	AA.set_coeff(7, 823);
	AA.set_coeff(6, 707);
	AA.set_coeff(5, 860);
	AA.set_coeff(4, 387);
	AA.set_coeff(3, 663);
	AA.set_coeff(2, 183);
	AA.set_coeff(1, 12);
	AA.set_coeff(0, 354);
	AA.print();

	Poly az0(7);
	az0.set_coeff(7, 2);
	az0.set_coeff(6, 1);
	az0.set_coeff(3, 3);
	az0.set_coeff(2, 1);
	az0.set_coeff(1, 1);
	az0.print();

	*/
	/*
	Poly newmod(7);
	newmod.set_coeff(7, 1);
	newmod.set_coeff(1, 1);
	newmod.set_coeff(0, 1);
	//newmod = get_teichmuller_modulus(newmod, 10);
	newmod.print();

	Poly bb(4);
	bb.set_coeff(4, 8);
	bb.set_coeff(0, 9);


	cout << " -------------------------" << endl;

	//bb = poly_remainder(bb, newmod, 10);
	//bb.print();
	Poly invbb = get_sqrt(bb, newmod, 5);
	invbb.print();
	invbb = invbb * invbb;
	invbb = poly_remainder(invbb, newmod, 5);
	invbb.print();
	//invbb = get_inverse(invbb, newmod, 5);
	//invbb = poly_remainder(invbb, newmod, 5);
	invbb.print();

	*/
	//cout << get_points(0b10001, 0x83, 7) << endl;

	int deg = 7;
	mpz_class orer = (1 << deg);
	mpz_class tmp;
	tmp.set_str("3", 16);
	tmp |= orer;
	cout << get_points(0b10001, tmp, deg) << endl;
	/*
	Poly M0 = M.PXpPmX();
	M0.print();
	M0 /= 2;
	M0 = M0.sqSubst();
	M0.print();

	Poly Msq = M0 * M0;
	Msq.print();

	Poly M1 = M.PXmPmX();
	//M1.print();
	M1 = (M1 >> 1);
	M1 /= 2;
	M1 = M1.sqSubst();
	M1.print();
	//M1 /= 2;
	//M1.print();
	*/
}

Poly adicops::get_teichmuller_modulus(Poly in, int N) {
	Poly M;
	if (N == 1) {
		M = in;
		M %= 2;

		cout << "----------------" << endl << "N = " << N << endl;

		cout << "M: ";
		M.print();
	} else {
		int newN = (N % 2 == 0 ? N / 2 : N / 2 + 1);
		M = get_teichmuller_modulus(in, newN);

		cout << "----------------" << endl << "N = " << N << ", N' = " << newN << endl;
		cout << "M: ";
		M.print();

		Poly M0 = M.PXpPmX();
		M0 /= 2;
		M0 %= (1 << N);
		M0 = M0.sqSubst();

		cout << "M0: ";
		M0.print();

		Poly M1 = M.PXmPmX();
		M1 = (M1 >> 1);
		M1 /= 2;
		M1 %= (1 << N);
		M1 = M1.sqSubst();

		cout << "M1: ";
		M1.print();

		Poly M0sq = M0 * M0;
		Poly M1sq = M1 * M1;
		Poly Xpoly = Poly(1);
		Xpoly.set_coeff(1, 1);
		Poly V = M - M0sq;
		// V %= (1 << (N - newN));
		V = V + (Xpoly * M1sq);
		V /= (1 << newN);
		V %= (1 << (N - newN));

		cout << "V: ";
		V.print();

		Poly delta = teichmuller_modulus_increment(M0, M1, V, N - newN);
		cout << "d: ";
		delta.print();

		delta *= (1 << newN);

		M = M + delta;
		M %= (1 << N);

		cout << "M: ";
		M.print();
	}
	return M;
}

Poly adicops::teichmuller_modulus_increment(const Poly& M0
		, const Poly& M1, const Poly& V, int N) {
	Poly delta;
	if (N == 1) {
		Poly nu(1);
		delta = nu - V;
		delta %= 2;
	} else {
		int newN = (N % 2 == 0 ? N / 2 : N / 2 + 1);
		delta = teichmuller_modulus_increment(M0, M1, V, newN);

		Poly delta0 = delta.PXpPmX();
		delta0 /= 2;
		delta0 %= (1 << N);
		delta0 = delta0.sqSubst();

		Poly delta1 = delta.PXmPmX();
		delta1 = (delta1 >> 1);
		delta1 /= 2;
		delta1 %= (1 << N);
		delta1 = delta1.sqSubst();

		Poly XPoly = Poly(1);
		XPoly.set_coeff(1, 1);
		Poly tmp = (delta0*M0) - (XPoly*M1*delta1);
		tmp *= 2;
		Poly Vnew = delta + V - tmp;
		Vnew /= (1 << newN);
		Vnew %= (1 << (N - newN));

		Poly bigDelta = teichmuller_modulus_increment(M0, M1, Vnew, N - newN);
		bigDelta *= (1 << newN);

		delta = bigDelta + delta;
		delta %= (1 << N);
	}

	return delta;
}

Poly adicops::poly_invert(Poly f, int N, int prec) {
	if (N == 1) {
		return Poly::one();
	} else {
		int newN = (N % 2 == 0 ? N / 2 : N / 2 + 1);
		Poly c = poly_invert(f, newN, prec);
		Poly one = Poly::one();
		//Poly fc = c*(one - (f * c));
		c = c + (c*(one - (f * c)));
		//cout << "N: " << N << endl;
		//c.print();
		c = c.modXpowm(N);
		c %= (1 << prec);
		//c.print();
		return c;
	}
}

Poly adicops::poly_remainder(Poly a, Poly b, int prec) {
	if (a.degree < b.degree) {
		a %= (1 << prec);
		return a;
	} else {
		int n = a.degree - b.degree + 1;
		//Poly powpoly = Poly(b.degree);
		//powpoly.set_coeff(b.degree, 1);
		Poly c = poly_invert(b.reverse(), n, prec);
		//c.print();
		Poly qq = a.reverse() * c;
		qq %= (1 << prec);
		qq = qq.modXpowm(n);
		Poly q = qq.reverse();
		Poly r = (a - b*q);
		//cout << "r before, "; r.print();
		r %= (1 << prec);
		//cout << "r after, "; r.print();
		r = r.modXpowm(b.degree);
		return r;
	}
}

Poly adicops::get_inverse(Poly a, Poly mod, int prec) {
	if (prec == 1) {
		GFE gfe = GFE(a.to_gfe_el(), mod.to_gfe_el());
		GFE invGfe = !gfe;
		Poly res = Poly(invGfe.element);
		//cout << "N = " << prec << ":"; res.print();
		return res;
	} else {
		Poly z = get_inverse(a, mod,
				(prec % 2 == 0 ? prec / 2 : prec / 2 + 1));
		Poly one = Poly::one();
		z = z + z*(one - (a*z));
		z = poly_remainder(z, mod, prec);
		//z = z.modXpowm(prec);
		//z %= (1 << prec);
		//cout << "N = " << prec << ":"; z.print();
		return z;
	}

}

Poly adicops::get_invsqrt(Poly a, Poly approx, Poly mod, int prec) {
	if (prec <= 2) {
		return approx;
	} else {
		int newN = ((prec + 1) % 2 == 0 ? (prec + 1) / 2 : (prec + 1) / 2 + 1);
		Poly z = get_invsqrt(a, approx, mod, newN);

		Poly one = Poly::one();

		Poly amazsq = one - (a*z*z);
		amazsq = poly_remainder(amazsq, mod, prec + 1);
		amazsq = z*amazsq;
		amazsq /= 2;
		z = z + amazsq;
		z = poly_remainder(z, mod, prec);
		return z;
	}
}

Poly adicops::get_sqrt(Poly a, Poly mod, int prec) {

	// binary inverse sqrt...
	GFE z = GFE(a.to_gfe_el(), mod.to_gfe_el());
	// TODO: ...
	z = z.get_sqrt(GFE::get_sqrtx(7, 1, mod.to_gfe_el()));
	z = !z;

	// Computing b=(1/a + z^2) / 4
	Poly inva = get_inverse(a, mod, prec);
	Poly polyz = Poly(z.element);
	Poly b = inva - (polyz * polyz);
	b = poly_remainder(b, mod, prec);
	b /= 4;

	// solving equation Delta^2+z*Delta=b
	GFE bgfe = GFE(b.to_gfe_el(), mod.to_gfe_el());
	GFE bigdelta = GFE::solve_quad_eq(z, bgfe);


	Poly polybigdelta = Poly(bigdelta.element);
	polybigdelta *= 2;

	// approx root to prec 2 is z+2*Delta
	polyz = polyz + polybigdelta;

	// comp. inverse square root with initial approximation polyz
	Poly invsqrt = get_invsqrt(a, polyz, mod, prec);

	// revover sqrt: 1/a^{-1} * a = sqrt(a)
	invsqrt = invsqrt * a;
	return poly_remainder(invsqrt, mod, prec);
}

mpz_class adicops::get_points(mpz_class _c, mpz_class _mod, int d) {

	int N = (d % 2 == 0 ? d / 2 + 3 : d / 2 + 4);
	Poly a = Poly::one();

	Poly c = Poly(_c);
	Poly mod = Poly(_mod);

	c *= 8;
	Poly b = a + c;

	c = poly_remainder(b, mod, 4);

	cout << "4 a: "; a.print();
	cout << "4 b: "; b.print();

	for (int i = 5; i <= N; ++i) {
		Poly olda = a;
		Poly oldb = b;
		a = a + b;
		a /= 2;
		Poly ab = olda * oldb;
		ab = poly_remainder(ab, mod, i);
		b = get_sqrt(ab, mod, i);
		assert(testsqrt(b, ab, mod, i));
		//Poly re = b * b;
		//re = poly_remainder(re, mod, i);
		cout << i << " a: "; a.print();
		cout << i << " b: "; b.print();
		//cout << i << " ab: "; ab.print();
		//cout << i << " re: "; re.print();
	}

	cout << "---" << endl;
	Poly a0 = a;

	for (int i = 0; i <= d - 1; ++i) {
		Poly olda = a;
		Poly oldb = b;
		a = olda + oldb;
		a /= 2;
		//a %= (1 << )
		Poly ab = olda * oldb;
		ab = poly_remainder(ab, mod, N);
		b = get_sqrt(ab, mod, N);
		Poly re = b * b;
		re = poly_remainder(re, mod, N);
		assert(testsqrt(b, ab, mod, N));
		cout << i << " a: "; a.print();
		cout << i << " b: "; b.print();
		//cout << i << " ab: "; ab.print();
		//cout << i << " re: "; re.print();
	}

	a0 %= (1 << (N - 1));
	a %= (1 << (N - 1));
	Poly inva = get_inverse(a, mod, N - 1);
	Poly t = a0 * inva;
	t = poly_remainder(t, mod, N - 1);

	mpz_class mt = t.coeffs[0];
	if ((mt * mt) > (1 << (d + 2))) {
		mt = mt - (1 << (N - 1));
	}
	a0.print();
	a.print();

	t.print();

	return (1 << d) + 1 - mt;
	//mod.print();
}

bool adicops::testsqrt(Poly sqrt, Poly in, Poly mod, int prec) {
	Poly t = sqrt * sqrt;
	t = poly_remainder(t, mod, prec);

	if (!(t == in)) {
		cout << "t1: ";
		t.print();
		cout << "t2: ";
		in.print();
	}
	return t == in;
}

adicops::~adicops() {
	// TODO Auto-generated destructor stub
}
