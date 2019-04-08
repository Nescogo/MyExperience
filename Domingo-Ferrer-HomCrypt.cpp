// LysikovaHom.cpp: РѕРїСЂРµРґРµР»СЏРµС‚ С‚РѕС‡РєСѓ РІС…РѕРґР° РґР»СЏ РєРѕРЅСЃРѕР»СЊРЅРѕРіРѕ РїСЂРёР»РѕР¶РµРЅРёСЏ.
// Domingo Ferrer
#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <io.h>
#include <math.h>
//for rand or srand
#include <cstdlib>
//перевод текста в десятичный код
#include <stdio.h>
#include <stdlib.h>
//files
#include "Keys.h"
// для преобразования текста в число и обратно
#include <sstream>
#define NTL_NO_MIN_MAX 1
#include <NTL/ZZ.h>
#include <NTL/ZZX.h>
//#include <C:/Users/nell9/source/repos/WinNTL-11_0_0/include/NTL/vector.h>

using namespace std;
using namespace NTL;


// read num
template <typename Stream>
bool read_num(Stream &stream, int &number) {
	char c = 0;
	std::string buf;
	while (true) {
		stream.read(&c, 1);
		if (c == ' ' || stream.eof()) {
			if (!buf.empty()) {
				number = atoi(buf.c_str());
				return true;
			}
		}
		else {
			buf += c;
		}
	}
	return false;
}

//primer test
bool primetest(long num) {
	bool answ = TRUE;
	for (int i = 2; i <= sqrt(num); i++) {
		if (num % i == 0) {
			answ = FALSE;
			break;
		}
	}
	return answ;
}

bool keygen(int d) {
	long p, q, n, rp, rq;
	// secret keys
	p = rand();
	while (primetest(p) == FALSE || p == 1) {
		p = rand();
	}
	q = rand();
	while (primetest(q) == FALSE || q > p || q == 1) {
		q = rand();
	}
	rp = rand() % p;
	while (primetest(rp) == FALSE || rp == 1 || rp == 0) {
		rp = rand() % p;
	}
	rq = rand() % q;
	while (primetest(rq) == FALSE || rq == 1 || rq == 0) {
		rq = rand() % q;
	}
	// записываем секретные параметры
	ofstream secret("secretkey.txt");
	secret << p << " " << q << " " << rp << " " << rq;
	std::cout << p << " " << q << " " << rp << " " << rq << std::endl;
	secret.close();
	// записывам публичные параметры
	n = p*q;
	std::cout << n << endl;
	ofstream pub("publickey.txt");
	pub << d << " " << n;
	pub.close();
	return TRUE;
}

vector <long> enc_char(long p, long q, long rp, long rq, long d, long txt) {
	long var, summ = 0, new_d, pq, step;
	ZZ num, new_var, elem, relem;
	long enc_code;
	vector <long> part;
	new_d = d - 1;
	enc_code = txt;
	//делим текст на d частей
	for (int i = 0; i < d; i++) {
		if (i != new_d) {
			var = rand();
			while (var == 0 || var == p || var == q) {
				var = rand();
			}
			summ += var;
		}
		else {
			var = txt - summ;
		}
		//cout << "chasti: " << var << ";" << endl;
		for (int j = 0; j < 2; j++) {
			conv(new_var, var);
			step = i + 1;
			if (j == 0) {
				conv(elem, p);
				conv(relem, rp);
				pq = p;
				//num = new_var*(pow(rp, i + 1)) % elem;
			}
			else {
				conv(elem, q);
				conv(relem, rq);
				pq = q;
			}
			PowerMod(num, relem, step, elem);
			num = MulMod(num, new_var, elem);
			conv(enc_code, num);
			if (enc_code < 0) {
				enc_code = pq + enc_code;
			}
			//cout << "ostat1: " << enc_code << endl;
			part.push_back(enc_code);
			//cout << "num: " << num << ";" << "ostat2: " << enc_code << endl;
		}
	}
	//cout << "-----------------------------------" << endl;
	return part;
}

vector <long> encrypt(long m) {
	int p, q, rp, rq, d;
	vector <int> sec, pub;
	ifstream keys;
	//считываем секретные данные
	keys.open("secretkey.txt");
	if (!keys.is_open()) {
		std::cout << "ERROR";
	}
	while (!keys.eof()) {
		int num;
		if (read_num(keys, num))
			sec.push_back(num); // тут можно складывать в список
	}
	p = sec[0];
	q = sec[1];
	rp = sec[2];
	rq = sec[3];
	keys.close();
	//считываем открытые данные
	keys.open("publickey.txt");
	if (!keys.is_open()) {
		std::cout << "ERROR";
	}
	while (!keys.eof()) {
		int num;
		if (read_num(keys, num)) {
			pub.push_back(num); // тут можно складывать в список
		}
	}
	d = pub[0];
	keys.close();
	sec.clear();
	pub.clear();
	//encrypt text
	ofstream entext("Encrypt.txt");
	vector <long> enctext;
	enctext = enc_char(p, q, rp, rq, d, m);
	for (int g = 0; g < enctext.size(); g++) {
		if (g == (enctext.size() - 1)) {
			entext << enctext[g];
			}
		else {
			entext << enctext[g] << " ";
		}
	}
	std::cout << endl;
	entext.close();
	return enctext;
}
//
long obratim(int num, int r) {
	long obraim = 1;
	while ((r*obraim) % num != 1) {
		obraim++;
		if (obraim == num) {
			obraim = 0;
			break;
		}
	}
	return obraim;
}

long chineese_theorem(long a, long b, long p, long q) {
	long n1 = 1, n2 = 1;
	long prov = 1;
	while (prov != 0) {
		prov = p * n1 + a - q * n2 - b;
		if (prov < 0) {
			n1++;
		}
		else if (prov > 0) {
			n2++;
		}
		else {
			break;
		}
	}
	prov = p*n1 + a;
	return prov;
}

bool decrypt(vector <long> text) {
	int p, q, rp, rq, d, n;
	vector <int> sec, pub;
	ifstream keys;
	//открываем файлы с ключами
	//считываем секретные данные
	keys.open("secretkey.txt");
	if (!keys.is_open()) {
		std::cout << "ERROR";
	}
	while (!keys.eof()) {
		int num;
		if (read_num(keys, num))
			sec.push_back(num); // тут можно складывать в список
	}
	p = sec[0];
	q = sec[1];
	rp = sec[2];
	rq = sec[3];
	keys.close();
	//считываем открытые данные
	keys.open("publickey.txt");
	if (!keys.is_open()) {
		std::cout << "ERROR";
	}
	while (!keys.eof()) {
		int num;
		if (read_num(keys, num)) {
			pub.push_back(num); // тут можно складывать в список
		}
	}
	d = pub[0];
	n = pub[1];
	keys.close();
	sec.clear();
	pub.clear();
	// decrypt
	vector <long> hom_sum, ostext;
	int numtxt = text.size();
	long pk = 0, qk = 0;
	ostext = text;
	/*
	// сокращем числа по остаткам, иначе идет переполнение компилятора
	std::cout << "new_enc: ";
	for (int i = 0; i < numtxt; i++) {
		if (i % 2 == 0) {
			ostext[i] = ostext[i] % p;
		}
		else {
			ostext[i] = ostext[i] % q;
		}
	std:cout << " " << ostext[i];
	}
	std::cout << endl;
	*/
	//обратимые элементы
	long pr, qr, prim;
	ZZ num, new_var, step, elem, relem;
	pr = obratim(p, rp);
	qr = obratim(q, rq);
	//cout << "rp^-1: " << pr << " rq^-1: " << qr << endl;
	//умножаем шифр на обратный элемент
	for (int i = 0; i < numtxt; i++) {
		conv(num, ostext[i]);
		if (i % 2 == 0) {
			conv(elem, p);
			conv(relem, pr);
			step = i / 2 + 1;
			//step = pow(pr, (i / 2 + 1));
			//num = ostext[i] * step % p;
		}
		else {
			conv(elem, q);
			conv(relem, qr);
			step = (i + 1) / 2;
		}
		PowerMod(new_var, relem, step, elem);
		num = MulMod(num, new_var, elem);
		conv(prim, num);
		hom_sum.push_back(prim);
		//"pq- " << elem << " rpq- " << relem <<
		//cout <<  " new_var- " << new_var;
		//cout << " num- " << num << " ostat- " << prim << endl;
		//cout << " " << prim;
	}
	//cout << "---------------------------------------------------------------" << endl;
	prim = 0;
	for (int i = 0; i < numtxt; i++) {
		if (i % 2 == 0) {
			pk += hom_sum[i];
			pk = pk % p;
		}
		else {
			qk += hom_sum[i];
			qk = qk % q;
		}
	}
	cout << pk << " " << qk << endl;
	hom_sum.clear();
	//Decrypt chineese theorem
	if (pk == qk) {
		prim = pk;
		}
	else {
		prim = chineese_theorem(pk, qk, p, q);
	}
	cout << prim << endl;
	//записываем расшифрованный текст
	ofstream detext("Decrypt.txt");
	detext << prim;
	detext.close();
	return TRUE;
}
//Add & Mult
vector <long> addiction(vector <long> ctxt1, vector <long> ctxt2) {
	cout << "Add" << endl;
	vector <long> new_add;
	long text;
	for (int i = 0; i < ctxt1.size(); i++) {
		text = ctxt1[i];
		if (ctxt2[0] != 0) {
			text += ctxt2[i];
		}
		new_add.push_back(text);
	}
	ofstream Add("Add.txt");
	for (int i = 0; i < new_add.size(); i++) {
		Add << new_add[i] << " ";
	}
	Add.close();
	return new_add;
}

vector <long> multiplication(vector <long> ctxt1, vector <long> ctxt2) {
	int num = 1, razm;
	if (ctxt1.size() > ctxt2.size()) {
		razm =ctxt1.size() * 2 - 1;
	}
	else {
		razm = ctxt2.size() * 2 - 1;
	}
	vector <long> mult(razm + 1);
	/*
	if (ctxt2[0] == 0) {
		ifstream text;
		text.open("Encrypt.txt");
		if (!text.is_open()) {
			std::cout << "ERROR";
		}
		while (!text.eof()) {
			int num;
			if (read_num(text, num)) {
				add.push_back(num);
			}
		}
		text.clear();
		text.close();
	}
	*/
	vector <long> add = ctxt2;
	int k = 0;
	//operation
	for (int i = 0; i < mult.size(); i++) {
			mult[i] = 0;
		}
	//std::cout << "counter mult: ";
	for (int i = 0; i < ctxt1.size(); i++) {
		for (int j = 0; j < add.size(); j ++) {
			if ((j % 2 == 0 && i % 2 == 0) || (j % 2 != 0 && i % 2 != 0)) {
				if ((j % 2 == 0 && i % 2 == 0)) {
					k = i + j + 2;
					mult[k] += ctxt1[i] * add[j];
				}
				else {
					k = i + j + 1;
					mult[k] += ctxt1[i] * add[j];
				}
				//std::cout << "-" << k << " " << mult[k];
			}
		}
	}
	std::cout << endl;
	ofstream Mult("Mult.txt");
	for (int i = 0; i < mult.size(); i++) {
		Mult << mult[i] << " ";
	}
	Mult.close();
	return mult;
}

vector <long> subtraction(vector <long> ctxt1, vector <long> ctxt2) {
	vector <long> add = ctxt2;
	int p, q;
	vector <int> sec;
	ifstream keys;
	//открываем файлы с ключами
	//считываем секретные данные
	keys.open("secretkey.txt");
	if (!keys.is_open()) {
		std::cout << "ERROR";
	}
	while (!keys.eof()) {
		int num;
		if (read_num(keys, num))
			sec.push_back(num); // тут можно складывать в список
	}
	p = sec[0];
	q = sec[1];
	keys.close();
	/*
	int num = 1; 
	if (ctxt1[0] == 0) {
		ifstream text;
		text.open("Encrypt.txt");
		if (!text.is_open()) {
			std::cout << "ERROR";
		}
		while (!text.eof()) {
			int num;
			if (read_num(text, num)) {
				add.push_back(num);
			}
		}
		text.clear();
		text.close();
	}
	else {
		vector <long> add = ctxt2;
	}
	
	for (int i = 0; i < ctxt1.size(); i++) {
		add[i] = ctxt2[i];
	}
	*/
	for (int i = 0; i < ctxt1.size(); i++) {
		if (ctxt1[i] == add[i]) {
			add[i] = 0;
		}
		else {
			add[i] = ctxt1[i] - add[i];
			if (add[i] < 0) {
				if (i % 2 == 0) {
					add[i] += p;
				}
				else {
					add[i] += q;
				}
			}
		}
	}
	ofstream Add("Add.txt");
	for (int i = 0; i < add.size(); i++) {
		Add << add[i] << " ";
	}
	Add.close();
	return add;
}


bool manual() {
	long var, m, d, n = 0;
	vector <long> ctxt, ctxt1;
	ctxt.push_back(0);
	clock_t time;
	ofstream crtext("Ctxt.txt");
	cout << "Выберите номер операции или 0:" << endl << "1 - генерация ключа" << endl << "2 - зашифровка текста" << endl << "3 - расшифровка текста" << endl;
	cout << "4 - суммирование зашифрованных данных" << endl << "5 - разность между зашифрованными данными" << endl << "6 - умножение зашифрованных данных" << endl;
	cin >> var;
	// 1 - keygen
	// 2 - encrypt
	// 3 - decrypt
	// 4 - суммирование
	// 5 - перемножение
	cout << var << endl;
	while (var != 0) {
		if (var < 0 || var > 6) {
			std::cout << "Выберите другое значение:" << endl;
		}
		else {
			std::cout << "right choise" << endl;
			if (var < 4) {
				if (var == 1) {
					cout << "keygen" << endl;
					cout << "Введите кол-во слагаемых: " << endl;
					cin >> d;
					time = clock();
					keygen(d);
					time = clock() - time;
					ofstream timer("time.txt");
					timer << "keygen: " << time << "sec" << endl;
					timer.close();
				}
				else if (var == 2) {
					d = 5;
					cout << "encrypt" << endl;
					cout << "text: ";
					cin >> m;
					time = clock();
					ctxt1 = encrypt(m);
					time = clock() - time;
					ofstream timer("time.txt", ios::app);
					timer << "d: " << d << endl;
					timer << "encrypt: " << time << "sec" << endl;
					timer.close();
					if (n == 0) {
						ctxt = addiction(ctxt1, ctxt);
						for (int i = 0; i < ctxt.size(); i++) {
							cout << ctxt[i] << " ";
						}
						cout << endl;
					}
				}
				else {
					cout << "decrypt" << endl;
					time = clock();
					decrypt(ctxt);
					time = clock() - time;
					ofstream timer("time.txt", ios::app);
					timer << "decrypt: " << time << "sec" << endl;
					timer.close();
				}
			}
			else {
				if (var == 4) {
					cout << "Add" << endl;
					ctxt = addiction(ctxt,ctxt1);
				}
				else if (var == 5) {
					cout << "Sub" << endl;
					ctxt = subtraction(ctxt, ctxt1);
				}
				else {
					cout << "Mult" << endl;
					ctxt = multiplication(ctxt, ctxt1);
				}
				for (int i = 0; i < ctxt.size(); i++) {
					cout << ctxt[i] << " ";
				}
				cout << endl;
			}
		}
		std::cout << "Выберите номер операции или 0" << endl;
		std::cin >> var;
		n++;
		for (int i = 0; i < ctxt.size(); i++) {
			crtext << ctxt[i];
		}
	}
	crtext.close();
	return 0;
}

int main()
{
	//Определяем язык cp1251, русифицируем
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	//
	int m, d, n = 0, version;
	vector <long> ctxt;
	clock_t time;
	ofstream crtext("Ctxt.txt");
	//
	version = 2;
	if (version == 1) {
		//operations (x1-x2)*(y1-y2)
		long x1, y1, x2, y2;
		//[x1 y1; x2 y2]
		long a = 1, b = 3, dd = 7, e = 5;
		vector <long> c1, c2, c3, c4;
		ofstream timer("time.txt", ios::app);
		//keygen
		d = 3;
		cin >> version;
		while (version == 1) {
			time = clock();
			keygen(d);
			time = clock() - time;
			timer << "------------------------" << endl;
			timer << "keygen: " << time << "sec" << endl;
			cin >> version;
		}
		std::cout << "message: " << "x1 = 1; y1 = 3; x2 = 7; y2 = 5" << endl;
		//encrypt
		time = clock();
		//x1
		c1 = encrypt(a);
		/*
		ifstream text;
		text.open("Encrypt.txt");
		if (!text.is_open()) {
			std::cout << "ERROR";
		}
		while (!text.eof()) {
			int num;
			if (read_num(text, num)) {
				c1.push_back(num);
			}
		}
		*/
		for (int i = 0; i < c1.size(); i++) {
			std::cout << c1[i] << " ";
		}
		std::cout << endl;
		//y1 
		c2 = encrypt(b);
		/*
		text.open("Encrypt.txt");
		if (!text.is_open()) {
			std::cout << "ERROR";
		}
		while (!text.eof()) {
			int num;
			if (read_num(text, num)) {
				c2.push_back(num);
			}
		}
		*/

		for (int i = 0; i < c2.size(); i++) {
			std::cout << c2[i] << " ";
		}
		std::cout << endl;
		//x2
		c3 = encrypt(dd);
		/*
		text.open("Encrypt.txt");
		if (!text.is_open()) {
			std::cout << "ERROR";
		}
		while (!text.eof()) {
			int num;
			if (read_num(text, num)) {
				c3.push_back(num);
			}
		}
		*/
		for (int i = 0; i < c3.size(); i++) {
			std::cout << c3[i] << " ";
		}
		std::cout << endl;
		//y2
		c4 = encrypt(e);
		/*
		text.open("Encrypt.txt");
		if (!text.is_open()) {
			std::cout << "ERROR";
		}
		while (!text.eof()) {
			int num;
			if (read_num(text, num)) {
				c4.push_back(num);
			}
		}
		*/
		for (int i = 0; i < c4.size(); i++) {
			std::cout << c4[i] << " ";
		}
		std::cout << endl;
		time = clock() - time;
		timer << "encrypt: " << time << "sec" << endl;
		//operations (x1-x2)*(y1-y2)
		/*
		vector <long> a, b, d, e;
		for (int i = 0; i < c1.size(); i++) {
			a.push_back(c1[i]);   //x1
			b.push_back(c2[i]);   //y1
			d.push_back(c3[i]);   //x2
			e.push_back(c4[i]);   //y2
		}
		*/
		vector <long> ctxt1, ctxt2;
		ctxt1 = subtraction(c1,c3);
		std::cout << "(x1-x2)= ";
		for (int i = 0; i < ctxt1.size(); i++) {
			std::cout << " " << ctxt1[i];
		}
		std::cout << endl;
		ctxt2 = subtraction(c2,c4);
		std::cout << "(y1-y2)= "; 
		for (int i = 0; i < ctxt1.size(); i++) {
			std::cout << " " << ctxt2[i];
		}
		std::cout << endl;
		ctxt = multiplication(ctxt1, ctxt2);
		std::cout << "S= ";
		for (int i = 0; i < ctxt.size(); i++) {
			std::cout << " " << ctxt[i];
		}
		std::cout << endl;
		//ok
		//decrypt
		time = clock();
		decrypt(ctxt);
		time = clock() - time;
		timer << "decrypt: " << time << "sec" << endl;
		timer.close();
	}
	else if (version == 2) {
		//x1, x2, x3 time-manager
		long a, b, c;
		vector <int> key;
		a = 23;
		b = 64;
		c = 19;
		ifstream keys;
		keys.open("publickey.txt");
		if (!keys.is_open()) {
			std::cout << "ERROR";
		}
		while (!keys.eof()) {
			int num;
			if (read_num(keys, num)) {
				key.push_back(num); // тут можно складывать в список
			}
		}
		d = key[0];
		vector <long> enc1, enc2, enc3;
		ofstream timer("time.txt", ios::app);
		//enc(a)
		time = clock();
		enc1 = encrypt(a);
		time = clock() - time;
		timer << "d: " << d << endl;
		timer << "encrypt1: " << time << "sec" << endl;
		//enc(b)
		time = clock();
		enc2 = encrypt(b);
		time = clock() - time;
		timer << "d: " << d << endl;
		timer << "encrypt2: " << time << "sec" << endl;
		//enc(c)
		time = clock();
		enc3 = encrypt(c);
		time = clock() - time;
		timer << "d: " << d << endl;
		timer << "encrypt3: " << time << "sec" << endl;
		//decrypt
		cout << "decrypt" << endl;
		//dec(a)
		time = clock();
		decrypt(enc1);
		time = clock() - time;
		timer << "decrypt1: " << time << "sec" << endl;
		//dec(b)
		time = clock();
		decrypt(enc2);
		time = clock() - time;
		timer << "decrypt2: " << time << "sec" << endl;
		//dec(c)
		time = clock();
		decrypt(enc3);
		time = clock() - time;
		timer << "decrypt3: " << time << "sec" << endl;
		//enc(a)+enc(b)
		cout << "Add" << endl;
		ctxt = addiction(enc1, enc2);

		time = clock();
		decrypt(ctxt);
		time = clock() - time;
		timer << "decryptAdd: " << time << "sec" << endl;
		//enc(a+b)*enc(c)
		cout << "Mult" << endl;
		ctxt = multiplication(ctxt, enc3);

		time = clock();
		decrypt(ctxt);
		time = clock() - time;
		timer << "decryptMult: " << time << "sec" << endl;
		timer.close();
	}
	else {
		manual();
	}


	return 0;
}