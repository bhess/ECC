#include "zp_int.h"
using std::cout;
using std::endl;

zp_int& zp_int::operator+=(const zp_int& rhs){
    val += rhs.val;
    return normalize();
}

zp_int& zp_int::operator-=(const zp_int& rhs){
//    cout << "val = " << val << " rhs.val = " << rhs.val << endl;
    val -= rhs.val;
    return normalize();
}

zp_int& zp_int::operator*=(const zp_int& rhs){
    val *= rhs.val;
    return normalize();
}

zp_int& zp_int::operator/=(const zp_int& rhs){
    if (p == rhs.p)
        val *= rhs.inverse().val;
    else
        val *= zp_int(rhs.val,p).inverse().val;
    return normalize();
}

zp_int& zp_int::invert(){
    if (val == 0)
        throw "Division by 0";
    mpz_invert(val.get_mpz_t(), val.get_mpz_t(),p.get_mpz_t());
    return normalize();
}

zp_int zp_int::inverse() const{
    zp_int temp = *this;
    return temp.invert();
}

zp_int& zp_int::normalize(){
    if (p == 0)
        return *this;
    val %= p;
    if (val < 0)
        val += p;
    return *this;
}
bool zp_int::is_equal(const zp_int& rhs) const{
    return (val == rhs.val);
}

bool zp_int::is_smaller(const zp_int& rhs) const{
    return (val < rhs.val);
}

zp_int zp_int::operator-() const{
    return zp_int(-val,p).normalize();
}
zp_int& zp_int::operator=(const zp_int& rhs){
    val = rhs.val;
    p = rhs.p;
}

zp_int& zp_int::operator^=(const mpz_class& exp){
//    cout << "val = " << val << ", exp = " << exp << ", p = " << p << endl;
    mpz_powm(val.get_mpz_t(),val.get_mpz_t(),exp.get_mpz_t(),p.get_mpz_t());
    return *this;
}

ostream& zp_int::print(ostream& o) const{
    o << val;
    return o;
}

ostream& zp_int::full_print(ostream& o) const{
    o << "("<<val<<", "<<p<<")";
    return o;
}

string zp_int::to_s(int base) const{
    return val.get_str(base);
}

zp_int operator+(const zp_int& lhs, const zp_int& rhs){
    zp_int temp = lhs;
    return (temp += rhs);
}
zp_int operator-(const zp_int& lhs, const zp_int& rhs){
    zp_int temp = lhs;
    return (temp -= rhs);
}
zp_int operator*(const zp_int& lhs, const zp_int& rhs){
    zp_int temp = lhs;
    return (temp *= rhs);
}
zp_int operator/(const zp_int& lhs, const zp_int& rhs){
    zp_int temp = lhs;
    return (temp /= rhs);
}

zp_int operator^(const zp_int& lhs, const mpz_class& rhs){
    zp_int temp = lhs;
    return (temp ^= rhs);
}

bool operator==(const zp_int& lhs, const zp_int& rhs){
    return (lhs.is_equal(rhs));
}

bool operator!=(const zp_int& lhs, const zp_int& rhs){
    return (!lhs.is_equal(rhs));
}
bool operator<(const zp_int& lhs, const zp_int& rhs){
    return (lhs.is_smaller(rhs));
}
ostream& operator<<(ostream& o, const zp_int& rhs){
    return rhs.print(o);
}


ZpCoordinate::ZpCoordinate(const ZpJacobian& jac):X(0),Y(0),p(jac.p) {
    if (jac.isInfinite()){
        *this = ZpCoordinate::infinity();
    }
    else{
        int const_2 = 2, const_3 = 3;
        X = (jac.X) / (jac.Z^const_2);
        Y = (jac.Y) / (jac.Z^const_3);
    }
}

string ZpCoordinate::toCompressedForm(){
    string result;
    switch (Y % 2){
        case 0: result += '-'; break;
        case 1: result += '+'; break;
    }
    result += X.to_s();
    return result;
}

ostream& operator<<(ostream& out, const ZpCoordinate& rhs){
    out << "(" << rhs.X << ", "<<rhs.Y<<")";
    return out;
}

ostream& operator<<(ostream& out, const ZpJacobian& rhs){
    out << "(" << rhs.X << ", "<<rhs.Y<<", " <<rhs.Z<<")";
    return out;
}
