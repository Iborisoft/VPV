#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <conio.h>
#include <vector>

using namespace std;
typedef unsigned int fix;
typedef float((*PFLOAT)(float));
typedef float((*PFIX)(fix));
#define STEP_COUNT 1000000
#define REPEATMIN 50 // число повторов фильтрации по MIN

const float accuracy = 1.0 / (1 << 22);
const float borderX = 0.65133;


int MAX_EFF = 5;
const float b = 0.1666667;

float arrE[] = { -0.167 , 0.00833333 ,-0.0019841369 , 0,00000275673,-0.00000002505 };
vector<float> tests;

void generateTests() {
	for (float i = 0; i < 1; i += accuracy) {
		tests.push_back(i);
	}
}

int doFactorial(int num) {
	if (num < 0) // если пользователь ввел отрицательное число
		return 0; // возвращаем ноль
	if (num == 0) // если пользователь ввел ноль,
		return 1; // возвращаем факториал от нуля - не удивляетесь, но это 1 =)
	else // Во всех остальных случаях
		return num * doFactorial(num - 1); // делаем рекурсию.
}
float ffabs(float arg) {
	if (arg < 0)
		return -arg;
	return arg;
}

float FlMath(float x) {
	return sin(x);
}
PFLOAT FlMathPoint = *FlMath;

float FlCycleNoGorner(float x) {
	float res = 0;
	int k = 1;
	
	for (int i = 0; i <=11; i+=2) {
		res += (pow(x,(2 * i)+k) / doFactorial(2 * i)+k);
	}
	return res;
}
PFLOAT FlCycleNoGornerPoint = *FlCycleNoGorner;

float FlCycleGorner(float x) {
	float x2 = x * x;
	float y = 0;
	for (int i = 5; i > 0; i--) {
		y = y * x2 + arrE[i];
	}
	return y;

}
PFLOAT FlCycleGornerPoint = *FlCycleGorner;

float FlNoCycleNoGorner(float x) {
	float res = pow(x, 0) / doFactorial(0) - pow(x, 3) / doFactorial(3) + pow(x, 5) / doFactorial(5) - pow(x, 7) / doFactorial(7) + pow(x, 9) / doFactorial(9) -
		pow(x, 11) / doFactorial(11);
	return res;
}
PFLOAT FlNoCycleNoGornerPoint = *FlNoCycleNoGorner;

float FlNoCycleGorner(float x) {
	float y = x * x;
	return ((((y*arrE[5] + arrE[4])*y + arrE[3])*y + arrE[2])*y + arrE[1])*y + 1 ;

}
PFLOAT FlNoCycleGornerPoint = *FlNoCycleGorner;


long long modul = 100000000;
inline long long fixMn(long long arg1, long long arg2) {
	long long rs = arg1 * arg2;
	rs = rs >> 23;
	return rs;
}
inline long long floatToFix(float arg) {
	return long long(arg * modul);
}
inline float fixToFloat(long long arg) {
	return float(arg) / modul;
}
long long fixA[5], fixB;
void generateFixA() {
	for (int i = 0; i < MAX_EFF; ++i) {
		fixA[i] = arrE[i] * modul;
	}
	fixB = b * modul;
}

float table[3000][4], val[3000];


float fS1(float x) { // ИСПОЛЬЗУЕТСЯ В ГЕНЕРАЦИИ ТАБЛИЦ
	return (-1.0 / sqrt(1 - x * x));
}
float fS2(float x) {
	return (-x / sqrt((1 - x * x) * (1 - x * x) * (1 - x * x))) / 2;
}
float fS3(float x) {
	return (((-2) * x * x - 1) / sqrt((1 - x * x) * (1 - x * x) * (1 - x * x) * (1 - x * x) * (1 - x * x))) / 6;
}
float delta;
int argTable = 2048;

void generateTable() {
	delta = 1.0 / argTable;
	float x = 0;
	for (int i = 0; i < argTable; ++i, x += delta) {
		val[i] = x;
		table[i][0] = FlNoCycleGorner(x);
		table[i][1] = fS1(x);
	}
}
float eTable(float x) {
	int ind = (argTable - 1) * x;
	float h = x - val[ind];
	return table[ind][0] + table[ind][1] * h;
}
PFLOAT eTablePoint = *eTable;


float fixCycleGorner(fix x_) {

	fix x2 = fixMn(x_, x_);
	fix last = fixMn(x2, x_);
	fix res = x_ + fixMn(last, fixB);

	for (int i = 0; i < MAX_EFF; ++i) {
		last = fixMn(last, x2);
		res += fixMn(last, fixA[i]);
	}
	return (res);
}
PFIX fixCycleGornerPoint = *fixCycleGorner;

float fixNoCycleGorner(fix x) {
	fix x2 = fixMn(x, x);
	fix res = fixMn(x, (1 +
		fixMn(x2, (fixB +
			fixMn(x2, (fixA[0] +
				fixMn(x2, (fixA[1] +
					fixMn(x2, (fixA[2] +
						fixMn(x2, (fixA[3] +
							fixMn(x2, (fixA[4] +
								fixMn(x2, (fixA[5]))))))))))))))));
	return (res);
}
PFIX fixNoCycleGornerPoint = *fixNoCycleGorner;


struct myFuncs {
	const char *name;
	PFLOAT fpoint;
} myfuncs[] = {
	{ "FlMath", FlMathPoint },
{ "FlCycleGorner", FlCycleGornerPoint },
{ "FlNoCycleGorner", FlNoCycleGornerPoint },
{ "FlCycleNoGorner", FlCycleNoGornerPoint },
{ "FlNoCycleNoGorner", FlNoCycleNoGornerPoint },
{ "eTable", eTablePoint }
};
struct myFuncs2 {
const char *name2;
	PFIX fpoint2;
} myfuncs2[] = {
	{ "fixCycleGorner", fixCycleGornerPoint },
{ "fixNoCycleGorner", fixNoCycleGornerPoint }
};

float exponential(int n, float x)
{
	float sum = 1.0f;

	for (int i = n - 1; i > 0; --i)
		sum = 1 + x * sum / i;

	return sum;
}
int flverify(float x, PFLOAT p) {
	return abs(p(x) - ((exp(x) + (1 / exp(x))) / 2)) < accuracy;
}
float flverifyabs(float x, PFLOAT p) {
	return abs(p(x) - ((exp(x) + (1 / exp(x))) / 2));
}
float flverifyp(float x, PFLOAT p) {
	return p(x);
}
long float_to_fixed(float value) {
	return (unsigned int)(value * (1 << 30));
}
fix xFixTable[STEP_COUNT];
float xFloatTable[STEP_COUNT];






float sm = 0;
long long getFullTime(PFLOAT func, long long cnt) {
	long long st = clock();
	for (long long i = 0; i < cnt; ++i) {
		sm += func(tests[i]); 
	}
	long long end = clock();
	return end - st;
}
long long getFullTime(PFIX func, long long cnt) {
	long long st = clock();
	for (long long i = 0; i < cnt; ++i) {
		sm += func(tests[i]); 
	}
	long long end = clock();
	return end - st;
}

double getTime(PFLOAT func) {
	long long cnt = 1000000;
	int tmin = 10000000;
	double rs = 0;
	for (int i = 0; i < REPEATMIN; i++) {
		rs = double(getFullTime(func, cnt) * 1000000) / cnt;
		if (rs < tmin) {
			tmin = rs;
		}
	}
	return tmin;
}

double getTime2(PFIX func) {
	long long cnt = 1000000;
	int tmin = 10000000;
	double rs = 0;
	for (int i = 0; i < REPEATMIN; i++) {
		rs = double(getFullTime(func, cnt) * 1000000) / cnt;
		if (rs < tmin) {
			tmin = rs;
		}
	}
	return tmin;
}



int main(int argc, char *argv[])
{
	do {
		generateTable();
		generateTests();
		generateFixA();
		int n = 6;

		for (int i = 0; i < n; i++) {
			printf("%s - %.1f\n", myfuncs[i].name, getTime(myfuncs[i].fpoint));
		}
		int m = 2;

		for (int i = 0; i < m; i++) {
			printf("%s - %.1f\n", myfuncs2[i].name2, getTime2(myfuncs2[i].fpoint2));
		}

		getchar();

	} while (_getch() != '-');
	return 0;
}

