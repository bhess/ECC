/*
 * ECBinary.cpp
 *
 *  Created on: Nov 22, 2009
 *      Author: bhess
 */

#include "ecbinary.h"

ECBinary::ECBinary() {
	// TODO Auto-generated constructor stub

}

Coordinate ECBinary::addition(Coordinate P, Coordinate Q) {
	if (P.isInfinite()) return P;
	if (Q.isInfinite()) return Q;

	GFE x0(P.X, mod);
	GFE y0(P.Y, mod);
	GFE x1(Q.X, mod);
	GFE y1(Q.Y, mod);
	GFE a(ECC_a, mod);
	GFE b(ECC_b, mod);
	GFE x2;
	GFE lambda;

	if (!(x0 == x1)) {
		lambda = (y0 + y1) * !(x0 + x1);
		x2 = a + lambda*lambda + lambda + x0 + x1;
	} else {
		if (!(y0 == y1)) return Coordinate::infinity();
		if (x1.isZero()) return Coordinate::infinity();

		lambda = x1 + y1*(!x1);
		x2 = a + lambda*lambda + lambda;
	}
	GFE y2 = (x1 + x2)*lambda + x2 + y1;
	return Coordinate(x2.element, y2.element);

	//return toCoordinate(addition(LD(P), Q));
}

Coordinate ECBinary::subtraction(Coordinate P, Coordinate Q) {
	return addition(P, getNegative(Q));
	//return toCoordinate(subtraction(LD(P), Q));
}

Coordinate ECBinary::getNegative(Coordinate P) {
	GFE my = GFE(P.X, mod) + GFE(P.Y, mod);
	return Coordinate(P.X, my.element);
}

Coordinate ECBinary::doubling(Coordinate P) {
	return addition(P, P);
	//return toCoordinate(doubling(LD(P)));
}

Coordinate ECBinary::repeatedDoubling(Coordinate P, int m) {
	// not implemented...
}

Coordinate ECBinary::pointMultiplication(Coordinate P, mpz_class k) {

	if (k == 0) return Coordinate::infinity();

	Coordinate Q = (k < 0 ? getNegative(P) : P);
	mpz_class n = (k < 0 ? mod - k : k);
	mpz_class h = 3 * n;

	int l = mpz_sizeinbase(h.get_mpz_t(), 2) - 1;
	mpz_class c1 = 1;
	mpz_class mask = c1 << (l - 1);
	Coordinate S = Q;
	for (int i = l - 1; i >= 1; --i) {
		S = doubling(S);
		if ((h & mask) != 0 && (n & mask) == 0) S = addition(S, Q);
		if ((h & mask) == 0 && (n & mask) != 0) S = subtraction(S, Q);
		mask >>= 1;
	}
	return S;
	/*
	std::vector<int> naf = getNAF(k);
	Coordinate Q = Coordinate::infinity();
	for (int i = naf.size() - 1; i >= 0; --i) {
		//Coordinate QQ = toCoordinate(Q);
		cout << "2*" << Q;
		Q = doubling(Q);
		//QQ = toCoordinate(Q);
		cout << "=" << Q << endl;
		if (naf[i] == 1) {
			//Coordinate QQ = toCoordinate(Q);
			cout << Q << " + " << P << " = ";
			Q = addition(Q, P);
			//QQ = toCoordinate(Q);
			cout << Q << endl;
		} else if (naf[i] == -1) {
			//Coordinate QQ = toCoordinate(Q);
			cout << Q << " + " << P << " = ";
			Q = subtraction(Q, P);
			//QQ = toCoordinate(Q);
			cout << Q << endl;
		}
	}
	return Q;
	*/

	/*
	// implementation according to p.99
	// actually same implementation as with prime curves,
	//  but using LD coordinates

	//cout << "Cord: " << P << endl << "pt: " << k << endl;
	std::vector<int> naf = getNAF(k);
	LD Q = LD::infinity();
	for (int i = naf.size() - 1; i >= 0; --i) {
		Coordinate QQ = toCoordinate(Q);
		cout << "2*" << QQ;
		Q = doubling(Q);
		QQ = toCoordinate(Q);
		cout << "=" << QQ << endl;
		if (naf[i] == 1) {
			Coordinate QQ = toCoordinate(Q);
			cout << QQ << " + " << P << " = ";
			Q = addition(Q, P);
			QQ = toCoordinate(Q);
			cout << QQ << endl;
		} else if (naf[i] == -1) {
			Coordinate QQ = toCoordinate(Q);
			cout << QQ << " + " << P << " = ";
			Q = subtraction(Q, P);
			QQ = toCoordinate(Q);
			cout << QQ << endl;
		}
	}
	return toCoordinate(Q);
	*/

}

/**
 * Optimization when a\in {0,1}
 * according p.94
 *
 * TODO: test
 */
LD ECBinary::addition(LD P, Coordinate Q) {

	// identity property
	if (Q.isInfinite()) {
		return P;
	}
	if (P.isInfinite()) {
		return LD(Q);
	}
	//mpz_class x3, y3, z3;
	GFE PX = GFE(P.X, mod), PY = GFE(P.Y, mod), PZ = GFE(P.Z, mod);
	GFE QX = GFE(Q.X, mod), QY = GFE(Q.Y, mod);
	GFE x3, y3, z3;
	mpz_class const_2 = 2;
	if (ECC_a == 0 || ECC_a == 1) {
		GFE t1, t2, t3, y3;

		t1 = PZ * QX;
		t2 = PZ * PZ;
		x3 = PX + t1;
		t1 = PZ * x3;
		t3 = t2 * QY;
		y3 = PY + t3;
		if (x3 == 0) {
			return (y3 == 0 ? doubling(LD(Q)) : LD::infinity());
		}
		z3 = t1 * t1;
		t3 = t1 * y3;
		if (ECC_a == 1) {
			t1 = t1 + t2;
		}
		t2 = x3 * x3;
		x3 = t2 * t1;
		t2 = y3 * y3;
		x3 = x3 + t2;
		x3 = x3 + t3;
		t2 = QX * z3;
		t2 = t2 + x3;
		t1 = z3 * z3;
		t3 = t3 + z3;
		y3 = t3 * t2;
		t2 = QX + QY;
		t3 = t1 * t2;
		y3 = y3 + t3;
	} else {
		GFE a, b, c, d, e, f, g;
		a = (QY * PZ * PZ) + PY;
		b = QX * PZ + PX;
		c = PZ * b;
		d = b * b * (c + GFE(ECC_a, mod) * PZ * PZ);
		z3 = c * c;
		e = a * c;
		x3 = a * a + d + e;
		f = x3 + QX * z3;
		g = (QX + QY) * z3 * z3;
		y3 = (e + z3) * f + g;
	}
	return LD(x3.get_element(), y3.get_element(), z3.get_element());
}

LD ECBinary::subtraction(LD P, Coordinate Q) {
	// TODO: really?
	return addition(P, Q);
}

/*
 * Optimized for the case when a=1 or 2
 * according to p.94
 *
 * TODO: test...
 */
LD ECBinary::doubling(LD P) {
	if (P.isInfinite()) {
		return P;
	}

	GFE x3, y3, z3;
	GFE PX = GFE(P.X, mod), PY = GFE(P.Y, mod), PZ = GFE(P.Z, mod);
	mpz_class const_2 = 2;
	if (ECC_a == 0 || ECC_a == 1) {
		GFE t1, t2;
		t1 = PZ * PZ;
		t2 = PX * PX;
		z3 = t1 * t2;
		x3 = t2 * t2;
		t1 = t1 * t1;
		t2 = t1 * GFE(ECC_b, mod);
		x3 = x3 + t2;
		t1 = PY * PY;
		if (ECC_a == 1) {
			t1 = t1 + z3;
		}
		t1 = t1 + t2;
		y3 = x3 * t1;
		t1 = t2 * z3;
		y3 = y3 + t1;
	} else {
		// TODO: intermediate results...
		z3 = PX * PX * PZ * PZ;
		x3 = PX * PX * PX * PX + GFE(ECC_b, mod) * PZ * PZ * PZ * PZ;
		y3 = GFE(ECC_b, mod) * PZ * PZ * PZ * PZ * z3 + x3 * (GFE(ECC_a, mod) * z3 + PY * PY
				+ GFE(ECC_b, mod) * PZ * PZ * PZ * PZ);
	}
	return LD(x3.get_element(), y3.get_element(), z3.get_element());
}

Coordinate ECBinary::toCoordinate(const LD& ld) {

	if (ld.X == 1 && ld.Y == 0 && ld.Z == 0) {
		return Coordinate::infinity();
	}

	GFE ldX = GFE(ld.X, mod), ldY = GFE(ld.Y, mod), ldZ = GFE(ld.Z, mod);
	GFE resY = ldZ * ldZ;
	GFE resX = !ldZ;
	resY = !resY;
	resX = ldX * resX;
	resY = ldY * resY;

	return Coordinate(resX.get_element(), resY.get_element());

}

vector<GFE> ECBinary::get_jinvariants() {
	GFE a = GFE(ECC_b, mod);
	a.print();
	// j(E)=j0=1/b
	GFE j0 = !a;
	// We create cyclic curves, therefore: jd=j0
	GFE jd = j0;
	int d = a.mod_deg();
	cout << d << endl;
	std::vector<GFE> res(d + 1);
	//res[0] = j0;
	res[d] = jd;
	for (int i = d - 1; i >= 0; --i) {
		res[i] = res[i + 1] * res[i + 1];
	}

	return res;
}


std::vector<GFE> ECBinary::update_js(int n, std::vector<GFE> Jinvs) {
	int d = Jinvs.size();
	vector<GFE> D(d - 1);
	vector<GFE> P(d);
	vector<GFE> J(d);
	for (int i = 0; i < d - 2; ++i) {
		GFE t = !phi_2_x(Jinvs[i], Jinvs[i+1]);

	}


	return J;
}

/*
 * Lifts j-invariants given by 'jinvs' to precision n
 * jinvs: j_0,...,j_d
 * returns J_0,...,J_{d-1}
 */
std::vector<GFE> ECBinary::lift_jinvariants(int n, std::vector<GFE> jinvs) {
	if (n == 1) {
		return jinvs;
	}
	int nn = (n % 2 == 0 ? n / 2 : n / 2 + 1);
	vector<GFE> res = lift_jinvariants(nn, jinvs);
	return update_js(n, res);
	// phi_2(X,Y)=X^3+Y^3-X^2Y^2+c1(XY^2+X^2Y)-c2(X^2+Y^2)+c3XY+c4(X+Y)-c5
	// c1=1488 c2=162000 c3=40773375 c4=87448000000 c5=157464000000000
}

mpz_class ECBinary::satohfgh_point_counting() {
	vector<GFE> jinvs = get_jinvariants();
	int d = jinvs[0].mod_deg();
	int n = (d % 2 == 0 ? d / 2 + 1 : d / 2 + 2);

	cout << "n: " << n << endl;
	return n;
}

inline GFE ECBinary::phi_2(GFE x, GFE y) {
	mpz_class c4t;
	c4t.set_str("87448000000", 2);
	mpz_class c5t;
	c5t.set_str("157464000000000", 2);
	GFE c1 = GFE(1488, x.mod);
	GFE c2 = GFE(162000, x.mod);
	GFE c3 = GFE(40773375, x.mod);
	GFE c4 = GFE(c4t, x.mod);
	GFE c5 = GFE(c5t, x.mod);
	return x*x*x+y*y*y-x*x*y*y+c1*(x*y*y+x*x*y)-c2*(x*x+y*y)+c3*x*y+c4*(x+y)-c5;
}

inline GFE ECBinary::phi_2_x(GFE x, GFE y) {
	GFE co2 = GFE(2, x.mod);
	GFE co3 = GFE(3, x.mod);
	GFE c1 = GFE(1488, x.mod);
	GFE c2 = GFE(162000, x.mod);
	GFE c3 = GFE(40773375, x.mod);
	return co3*x*x - co2*x*(y*y - c1*y + c2) + c1*y*y + c3*y;
}

Coordinate ECBinary::getPoint_interface(mpz_class xmpz, bool negative_value) {
	GFE y_tilde = (negative_value ? GFE(0, mod) : GFE(1, mod));
	GFE x(xmpz, mod);
    GFE alpha = x*x*x + GFE(ECC_a, mod)*x*x + GFE(ECC_b, mod);
    GFE beta = alpha * !(x*x);
    GFE z = GFE::solve_quad_eq(beta);
    if (!(z*z + z == beta)) return Coordinate::infinity();
    GFE z_tilde = GFE(z.element.get_ui() % 2, mod);
    GFE res = (z + z_tilde + y_tilde) * x;
    return Coordinate(xmpz, res.element);
    /*
	GFE gfex = GFE(x, mod);
	GFE gfea = GFE(ECC_a, mod);
	GFE gfeb = GFE(ECC_b, mod);
	GFE right_side = gfex*gfex*gfex + gfea*gfex*gfex + gfeb;
	GFE y = GFE::solve_quad_eq(gfex, right_side);
	return Coordinate(x, y.element);
	*/
}

Coordinate ECBinary::getPointCompressedForm(string form) {
    bool negval;
    switch (form[0]){
        case '+': negval = false; break;
        case '-': negval = true; break;
        default: throw "form is not legal";
    }
    form.erase(0,1); //removing the "+" or "-" in the beginning and remaining with the x-coordinate
    mpz_class mx;
    mpz_set_str(mx.get_mpz_t(), form.c_str(), 10);
    GFE x = GFE(mx, mod);
    return getPoint_interface(mx, negval);
    /*
    GFE alpha = x*x*x + GFE(ECC_a, mod)*x*x + GFE(ECC_b, mod);
    GFE beta = alpha * !(x*x);
    GFE z = GFE::solve_quad_eq(beta);
    GFE z_tilde = GFE(z.element.get_ui() % 2, mod);
    GFE res = (z + z_tilde + y_tilde) * x;
    return Coordinate(mx, res.element);
    */

    /*
    Coordinate result = getPoint_interface(x);
    return result;
    GFE z = GFE(result.Y, mod);
    GFE z_tilde = GFE(z.element % 2, mod);
    GFE y = (z + z_tilde + y_mod_2) * GFE(x, mod);
    return Coordinate(result.X, y.element);
    */
}
string ECBinary::toCompressedForm(Coordinate c) {
    string result;
    GFE e = GFE(c.Y, mod) * !GFE(c.X, mod);
    mpz_class temp = e.element % 2;
    switch (temp.get_ui()){
        case 0: result += '-'; break;
        case 1: result += '+'; break;
    }
    result += c.X.get_str(10);
    return result;
}

void ECBinary::check_coordinate(Coordinate c) {
	GFE a = GFE(ECC_a, mod);
	GFE b = GFE(ECC_b, mod);
	GFE x = GFE(c.X, mod);
	GFE y = GFE(c.Y, mod);
	GFE left = y*y + x*y;
	GFE right = x*x*x + a*x*x + b;
	cout << "l: " << left.element << endl;
	cout << "r: " << right.element << endl;
}
