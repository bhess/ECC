#include "hcp.h"

using std::cout;
using std::endl;

static int read_degree(string& poly_string){
    //assumes the string is of the form *x^i*, returns i
    int degree = 0;
    int pos = 0;
//    cout << "searching for x... pos = " << pos << endl;
    while (pos < poly_string.length() && poly_string[pos] != 'x')
        pos++;
//    cout << "stopped in pos = " << pos << endl;
    if (pos + 1 >= poly_string.length() || poly_string[pos+1] != '^')
        degree = 1;
    if (pos >= poly_string.length())
        degree = 0;
    if (poly_string[pos+1] == '^'){
        pos += 2;
        while (pos < poly_string.length() && poly_string[pos] <= '9' && poly_string[pos] >= '0'){
            degree *= 10;
            degree += (poly_string[pos] - '0');
            pos++;
        }
    }
    poly_string.erase(0,pos);
    return degree;
}

static mpz_class read_number(string& poly_string){
    mpz_class number;
    int pos = 0;
    int start;
    char buf[80];
    memset( buf, '\0', 80 );

    while (pos < poly_string.length() && (poly_string[pos] > '9' || poly_string[pos] < '0')){
        if (poly_string[pos] == 'x') //abort! abort! (since there is no number explicitly written
            return 1;
        pos++;
    }
        //reached the first digit
//    cout << "first digit" << endl;
    start = pos;
    while (pos < poly_string.length() && poly_string[pos] <= '9' && poly_string[pos] >= '0')
        pos++;
    poly_string.copy(buf, pos-start, start);
    mpz_set_str(number.get_mpz_t(), buf, 10);
    poly_string.erase(0,pos);
    return number;
}

static int read_sign(string& poly_string){
    int pos = 0;
    int sign = 0;
    while (pos < poly_string.length() && poly_string[pos] != '+' && poly_string[pos] != '-')
        pos++;
    if (poly_string[pos] == '+')
        sign = 1;
    if (poly_string[pos] == '-')
        sign = -1;
    poly_string.erase(0,pos+1); //might cause bug if pos == poly_string.length?
    return sign;
}
ModularPolynomial::ModularPolynomial(string poly_string, mpz_class p):modulus(p),degree(0){
    string temp_string = poly_string;
    int sign = 1;

    while (!temp_string.empty()){
//        cout << "reading number... string is " << temp_string << endl;
        mpz_class number = read_number(temp_string);
//        cout << "number read = " << number << endl;
//        cout << "reading degree... string is " << temp_string << endl;
        int temp_degree = read_degree(temp_string);
        if (degree < temp_degree)
            degree = temp_degree;
//        cout << "degree found = " << temp_degree << endl;
//        cout << "putting " <<((sign*number) % p) << " in coefficients[" << temp_degree << "]" << endl;
        coefficients[temp_degree] = ((sign*number) % p);
//        cout << "reading sign... string is " << temp_string << endl;
        sign = read_sign(temp_string);
//        cout << "sign is " << sign << endl;
    }
//    cout << "degree = " <<degree << endl;
//    cout << coefficients[2] << endl;
}

string ModularPolynomial::to_string() const{
    string o = "";
    //first we deal with two extreme cases
    if (degree == 0 && coefficients[0] == 0)
        return "0";
    if (degree == 0 && coefficients[0] == 1)
        return "1";
    for (int i = degree; i>=0; i--){
        mpz_class number = coefficients[i];
        if (number == 0)
            continue;
        if (i < degree)
            o += " + ";
        if (number != 1)
            o += number.get_str(10);
        if (i >= 2)
            o += ("x^" + mpz_class(i).get_str(10));
        if (i == 1)
            o += "x";
    }
    return o;

}

ModularPolynomial::ModularPolynomial(const ModularPolynomial& lhs):modulus(lhs.modulus),degree(lhs.degree){
    for (int i=degree; i>=0; i--){
        coefficients[i] =  lhs.coefficients[i];
    }
}

static inline int max(int a, int b){
    return (a<b)?(b):(a);
}
ModularPolynomial& ModularPolynomial::operator+=(const ModularPolynomial& lhs){
    int max_degree = max(degree, lhs.degree);
    for (int i = max_degree; i>=0; i--){
        coefficients[i] += lhs.coefficients[i];
        coefficients[i] %= modulus;
        if (i == max_degree && coefficients[i] == 0 && max_degree > 0)
            max_degree--;
    }
    degree = max_degree;
    return *this;
}
ModularPolynomial& ModularPolynomial::operator-=(const ModularPolynomial& lhs){
    int max_degree = max(degree, lhs.degree);
    for (int i = max_degree; i>=0; i--){
        coefficients[i] -= lhs.coefficients[i];
        coefficients[i] %= modulus;
        if (coefficients[i] < 0)
            coefficients[i] += modulus ;
        if (i == max_degree && coefficients[i] == 0 && max_degree > 0)
            max_degree--;
    }
    degree = max_degree;
    return *this;
}
ModularPolynomial& ModularPolynomial::operator=(const ModularPolynomial& lhs){
    degree = lhs.degree;
    for (int i=0; i<= degree; i++)
        coefficients[i] = lhs.coefficients[i];
    return *this;
}
ModularPolynomial& ModularPolynomial::operator*=(const ModularPolynomial& lhs){
    ModularPolynomial temp("",modulus);
    //naive multiplication - no optimizations yet
    int max_degree = degree + lhs.degree;
    temp.degree = max_degree;
    for (int i = max_degree; i>=0; i--){
        temp.coefficients[i] = 0;
        for (int j=0; j<=i; j++)
            if (i-j <= degree && j <= lhs.degree)
                temp.coefficients[i] += coefficients[i-j]*lhs.coefficients[j];
        if (temp.degree == i && temp.coefficients[i] == 0 && temp.degree > 0)
            temp.degree--;
    }
    *this = temp;
    return *this;
}

ModularPolynomial& ModularPolynomial::normalize(){
    mpz_class leading_coefficient_inverse;
    mpz_invert(leading_coefficient_inverse.get_mpz_t(), coefficients[degree].get_mpz_t(),modulus.get_mpz_t());
    for (int i=0; i<=degree; i++)
        coefficients[i] = ((coefficients[i] * leading_coefficient_inverse) % modulus);
    return *this;
}

ModularPolynomial ModularPolynomial::operator%(const ModularPolynomial& lhs){
    ModularPolynomial R = *this;
    mpz_class lhs_leading_coefficient_inverse;
    mpz_invert(lhs_leading_coefficient_inverse.get_mpz_t(), lhs.coefficients[lhs.degree].get_mpz_t(),modulus.get_mpz_t());
//    cout << "R.degree = " << R.degree << ", lhs.degree = "<<lhs.degree << endl;
//    cout << "got here with " << lhs.to_string() << endl;
//    cout << "and this = " << this->to_string() << endl;
    while (R.degree >= lhs.degree && !R.is_zero()){
        int R_old_degree = R.degree;
        mpz_class R_leading_coefficient = R.coefficients[R.degree];
        for (int i=R.degree; i>=R.degree - lhs.degree; i--){
            R.coefficients[i] -= (R_leading_coefficient*lhs_leading_coefficient_inverse*lhs.coefficients[lhs.degree - R_old_degree + i]);
            R.coefficients[i] %= R.modulus;
            if (R.coefficients[i] < 0)
                R.coefficients[i] += R.modulus;
            if (i == R.degree && R.coefficients[i] == 0 && R.degree > 0)
                R.degree--;
//            cout << "i = " << i << " coefficients[i] = " << R.coefficients[i] << endl;
        }
    }
    return R;
}

ModularPolynomial ModularPolynomial::modular_exponent(mpz_class exp, const ModularPolynomial& mod) const{
    ModularPolynomial result("1",modulus);
//    cout << "result = " << result << endl;
    ModularPolynomial y = *this;    
    mpz_class n = exp;
    while (n > 0){
        if (n % 2 == 1)
            result = ((result * y) % mod);
//        cout << "prev y = " << y << " degree = " << y.degree << endl;
        y = ((y * y) % mod);
//        cout << "next y = " << y << " degree = " << y.degree << endl;
        n /= 2;
//        cout << "result = " << result << endl;
    }
//    cout << "result = " << result << endl;
    return result;
}

bool ModularPolynomial::is_zero() const{
    return (degree == 0 && coefficients[0] == 0);
}
ModularPolynomial operator+(const ModularPolynomial& rhs, const ModularPolynomial& lhs){
    ModularPolynomial temp = rhs;
    temp += lhs;
    return temp;
}
ModularPolynomial operator-(const ModularPolynomial& rhs, const ModularPolynomial& lhs){
    ModularPolynomial temp = rhs;
    temp -= lhs;
    return temp;
}

ModularPolynomial operator*(const ModularPolynomial& rhs, const ModularPolynomial& lhs){
    ModularPolynomial temp = rhs;
    temp *= lhs;
    return temp;
}

ModularPolynomial gcd(const ModularPolynomial& rhs, const ModularPolynomial& lhs){
    ModularPolynomial A = rhs;
    ModularPolynomial B = lhs;
    ModularPolynomial R = lhs;
    while (!B.is_zero()){
        R = A % B;
        A = B;
        B = R;
    }
    return A.normalize();
}

bool ModularPolynomial::operator==(const ModularPolynomial& lhs) const{
    if (degree != lhs.degree)
        return false;
    for (int i=degree; i>=0; i--){
        if (coefficients[i] != lhs.coefficients[i])
            return false;
    }
    return true;    
}

ostream& operator<<(ostream& o, const ModularPolynomial& lhs){
    o << lhs.to_string();
    return o;
}

vector<mpz_class> ModularPolynomial::find_roots(){
//pg. 37 in Cohen's book
//    ModularPolynomial A = gcd(*this, ModularPolynomial("x^"))

}

HCP::HCP()

{

  // Prestore polynomials sorted by D

  H[ -7 ]   = "x + 3375";

  H[ -8 ]   = "x - 8000";

  H[ -11 ]  = "x + 32768";

  H[ -12 ]  = "x - 54000"; // NFD

  H[ -15 ]  = "x^2 + 191025x - 121287375";

  H[ -16 ]  = "x - 287496"; // NFD

  H[ -19 ] = "x + 884736";

  H[ -20 ]  = "x^2 - 1264000x - 681472000";

  H[ -23 ]  = "x^3 + 3491750x^2 - 5151296875x + 12771880859375";

  H[ -24 ]  = "x^2 - 4834944x + 14670139392";

  H[ -27 ]  = "x + 12288000"; // NFD

  H[ -28 ]  = "x - 16581375"; // NFD

  H[ -31 ]  = "x^3 + 39491307x^2 - 58682638134x + 1566028350940383";

  H[ -32 ]  = "x^2 - 52250000x + 12167000000"; // NFD

  H[ -35 ]  = "x^2 + 117964800x - 134217728000";

  H[ -36 ]  = "x^2 - 153542016x - 1790957481984"; // NFD

  H[ -39 ]  = "x^4 + 331531596x^3 - 429878960946x^2 + 109873509788637459x + 20919104368024767633";

  H[ -40 ]  = "x^2 - 425692800x + 9103145472000";

  H[ -43 ]  = "x + 884736000";

  H[ -44 ]  = "x^3 - 1122662608x^2 + 270413882112x - 653249011576832";  // NFD

  H[ -47 ]  = "x^5 + 2257834125x^4 - 9987963828125x^3 + 5115161850595703125x^2 - 14982472850828613281250x + 16042929600623870849609375";

  H[ -48 ]  = "x^2 - 2835810000x + 6549518250000"; // NFD

  H[ -51 ]  = "x^2 + 5541101568x + 6262062317568";

  H[ -52 ]  = "x^2 - 6896880000x - 567663552000000";

  H[ -55 ]  = "x^4 + 13136684625x^3 - 20948398473375x^2 + 172576736359017890625x - 18577989025032784359375";

  H[ -56 ]  = "x^4 - 16220384512x^3 + 2059647197077504x^2 + 2257767342088912896x + 10064086044321563803648";

  H[ -59 ]  = "x^3 + 30197678080x^2 - 140811576541184x + 374643194001883136";

  H[ -60 ]  = "x^2 - 37018076625x + 153173312762625"; // NFD

  H[ -63 ]  = "x^4 + 67515199875x^3 - 193068841781250x^2 + 4558451243295023437500x - 6256903954262253662109375"; // NFD

  H[ -64 ]  = "x^2 - 82226316240x - 7367066619912"; // NFD

  H[ -67 ]  = "x + 147197952000";

  H[ -68 ]  = "x^4 - 178211040000x^3 - 75843692160000000x^2 - 318507038720000000000x - 2089297506304000000000000";

  H[ -71 ]  = "x^7 + 313645809715x^6 - 3091990138604570x^5 + 98394038810047812049302x^4 - 823534263439730779968091389x^3 + 5138800366453976780323726329446x^2 - 425319473946139603274605151187659x + 737707086760731113357714241006081263";

  H[ -72 ]  = "x^2 - 377674768000x + 232381513792000000"; // NFD

  H[ -75 ]  = "x^2 + 654403829760x + 5209253090426880";  // NFD

  H[ -76 ]  = "x^3 - 784074438864x^2 + 1128678666363648x - 827237892283232256";  // NFD

  H[ -79 ]  = "x^5 + 1339190283240x^4 - 6366718450945836x^3 + 1793441424178093483069839x^2 - 5859423003994491322155950334x + 5458041030919737322344464663391";

  H[ -80 ]  = "x^4 - 1597177172000x^3 - 13028555239824000x^2 - 171263969177632000000x + 422286883970526784000000"; // NFD

  H[ -83 ]  = "x^3 + 2691907584000x^2 - 41490055168000000x + 549755813888000000000";

  H[ -84 ]  = "x^4 - 3196800946944x^3 - 5663679223085309952x^2 + 88821246589810089394176x - 5133201653210986057826304";

  H[ -87 ]  = "x^6 + 5321761711875x^5 + 85585228375218750x^4 + 28321090578679361484375000x^3 + 497577733884372638735595703125x^2 + 432181202257616392838287353515625x + 549806430204864490157810211181640625";

  H[ -88 ]  = "x^2 - 6294842640000x + 15798135578688000000";

  H[ -91 ]  = "x^2 + 10359073013760x - 3845689020776448";

  H[ -92 ]  = "x^3 - 12207823849750x^2 - 263033266852296875x - 6267542200571287109375"; // NFD

  H[ -95 ]  = "x^8 + 19874477919500x^7 - 688170786018119250x^6 + 395013575867144519258203125x^5 - 13089776536501963407329479984375x^4 + 352163322858664726762725228294921875x^3 - 1437415939871573574572839010971248046875x^2 + 2110631639116675267953915424764056884765625x + 107789694576540010002976771996177148681640625";

  H[ -96 ]  = "x^4 - 23340144296736x^3 + 670421055192156288x^2 + 447805364111967209472x - 984163224549635621646336"; // NFD

  H[ -99 ]  = "x^2 + 37616060956672x - 56171326053810176"; // NFD

  H[ -100 ] = "x^2 - 44031499226496x - 292143758886942437376"; // NFD

  H[ -103 ] = "x^5 + 70292286280125x^4 + 85475283659296875x^3 + 4941005649165514137656250000x^2 + 13355527720114165506172119140625x + 28826612937014029067466156005859375";

  H[ -104 ] = "x^6 - 82028232174464x^5 + 739545196164376195072x^4 + 31013571054009020830449664x^3 + 1378339984770204584193868955648x^2 - 25735039642229334200564710375424x + 65437179730333545242323676123103232";

  H[ -107 ] = "x^3 + 129783279616000x^2 - 6764523159552000000x + 337618789203968000000000";

  H[ -108 ] = "x^3 - 151013228706000x^2 + 224179462188000000x - 1879994705688000000000"; // NFD

  H[ -111 ] = "x^8 + 236917342626795x^7 + 12257744369763349962x^6 + 56129700127461627298044206619x^5 + 2987537813865962860773420720531252x^4 - 25675269514993965918445147228203062874x^3 + 88953282358528708595648019437144660946708x^2 - 64773995403104720702864091375403035855442761x + 27524793815819191410861831167197250556510894417";

  H[ -112 ] = "x^2 - 274917323970000x + 1337635747140890625"; // NFD

  H[ -115 ] = "x^2 + 427864611225600x + 130231327260672000";

  H[ -123 ] = "x^2 + 1354146840576000x + 148809594175488000000";

  H[ -139 ] = "x^3 + 12183160834031616x^2 - 53041786755137667072x + 67408489017571610198016";

  H[ -148 ] = "x^2 - 39660183801072000x - 7898242515936467904000000";

  H[ -163 ] = "x + 262537412640768000";

  H[ -187 ] = "x^2 + 4545336381788160000x - 3845689020776448000000";

  H[ -211 ] = "x^3 + 65873587288630099968x^2 + 277390576406111100862464x + 5310823021408898698117644288";

  H[ -232 ] = "x^2 - 604729957849891344000x + 14871070713157137145512000000000";

  H[ -235 ] = "x^2 + 823177419449425920000x + 11946621170462723407872000";

  H[ -251 ] = "x^7 + 4128446190315309498368x^6 - 66204185373144403998280777728x^5 + 1062008880270126105976008028408774656x^4 + 7966552994949346594041401247164174172160x^3 + 416131608793437401577832999781610387970981888x^2 - 1791911545705841840084320427251134859220759871488x + 1937587239465703269672056660685864050152464252403712";

  H[ -267 ] = "x^2 + 19683091854079488000000x + 531429662672621376897024000000";

  H[ -283 ] = "x^3 + 89611323386832801792000x^2 + 90839236535446929408000000x + 201371843156955365376000000000";

  H[ -307 ] = "x^3 + 805016812009981390848000x^2 - 5083646425734146162688000000x + 8987619631060626702336000000000";

  H[ -331 ] = "x^3 + 6647404730173793386463232x^2 + 368729929041040103875232661504x + 56176242840389398230218488594563072";

  H[ -379 ] = "x^3 + 364395404104624239018246144x^2 - 121567791009880876719538528321536x + 15443600047689011948024601807415148544";

  H[ -403 ] = "x^2 + 2452811389229331391979520000x - 108844203402491055833088000000";

  H[ -427 ] = "x^2 + 15611455512523783919812608000x + 155041756222618916546936832000000";

  H[ -499 ] = "x^3 + 3005101108071026200706725969920x^2 - 6063717825494266394722392560011051008x + 4671133182399954782798673154437441310949376";

  H[ -547 ] = "x^3 + 81297395539631654721637478400000x^2 - 139712328431787827943469744128000000x + 83303937570678403968635240448000000000";

  H[ -587 ] = "x^7 + 1138212574651782271861893763072000x^6 - 118840621090042353846268441242275676160000000x^5 + 12408121464739840095494810222119511810092564480000000000x^4 - 1196192801910538190537501166807952753551892021248000000000000x^3 + 35495224444423948749828541418253640332882575622144000000000000000x^2 - 19523831231348384917508345284946898016410494042112000000000000000000x + 748765079750903678495365866324569504346756859559936000000000000000000000";

  H[ -643 ] = "x^3 + 39545575162726134099492467011584000x^2 - 6300378505047247876499651797450752000000x + 308052554652302847380880841299197952000000000";

  H[ -883 ] = "x^3 + 34903934341011819039224295011933392896000x^2 - 151960111125245282033875619529124478976000000x + 167990285381627318187575520800123387904000000000";

  H[ -907 ] = "x^3 + 123072080721198402394477590506838687744000x^2 + 39181594208014819617565811575376314368000000x + 149161274746524841328545894969274007552000000000";

}

