#include "main.h"
const float EP = 0.1e-6;

template <typename T>
class Link
{
private:
	T data;
	Link<T> *next;

public:
	Link<T>(const T &ele)
	{
		this->data = ele;
		this->next = nullptr;
	}
	~Link<T>(){
		this->next = nullptr;
	}
	void SetDataLink(T);
	void SetLink(Link<T> *link);
	T &GetDataLink();
	Link<T> *GetLinkNext();
};

class Candle
{
private:
	long TIME;
	float OP;
	float HP;
	float LP;
	float CP;

public:
	long getTime(){
		return this->TIME;
	}
	float getOP(){
		return this->OP;
	}
	float getHP(){
		return this->HP;
	}
	float getLP(){
		return this->LP;
	}
	float getCP(){
		return this->CP;
	}
	void SetCandle(string TIME, string OP, string HP, string LP, string CP);
};

class Exchange
{
private:
	string BC;
	string QC;
	int numCandle;
	Link<Candle> *nextCandle;
	Link<Candle> *lastCandle;
public:
	Exchange(){
		this->nextCandle = nullptr;
		this->lastCandle = nullptr;
	}
	string getBC(){
		return this->BC;
	}
	string getQC(){
		return this->QC;
	}
	int getnumCandle(){
		return this->numCandle;
	}
	void setnumCandle(int n){
		this->numCandle += n;
	}
	void SetExchange(string, string);
	void SetNextCandle(Link<Candle> *NextCandle);
	void SetLastCandle(Link<Candle> *LastCandle);
	Link<Candle> *GetNextCandle();
	Link<Candle> *GetLastCandle();
};

template <typename T>
class LList
{
private:
	Link<T> *head;
	Link<T> *last;
	int cnt;
public:
	LList() { 
		head = last = nullptr; 
		cnt = 0;
	}
	~LList(){
		head = last = nullptr; 
		cnt = 0;
	}
	int length() const { return cnt;}
	void setLast(Link<T> * link);
	void setlength(int);
	void insert(const T &ele)
	{
		Link<T> *newExchange = new Link<T>(ele);
		if (head == nullptr){
			head = newExchange;
			last = newExchange;
		}
		else{
			last->SetLink(newExchange);
			last = last->GetLinkNext();
		}
		cnt++;
	}
	Link<T> *Gethead();
	void delFirstLink(Link<T> * link);
};

class ProcessData
{
private:
	LList<Exchange> *data;
	static const int MAXSIZECODE = 8;
	static int split(string line, string *&sp);

public:
	ProcessData();
	~ProcessData();
	int process(string line);
	//khai bao cac ham da tao
	Link<Exchange> *FindExchange(string, string);
	Link<Exchange> *FindPreExchange(Link<Exchange> *);
	float max(float,float);
	float min(float,float);
	bool isSplit(const string *, int);
	bool isTime(string);
	bool isValue(string);
	bool isCandle(string, string, string, string, string);
	bool isExchange(string, string);
	bool IsMB(float, float, float, float);
	bool IsST(float, float, float, float);
	bool IsBD_C1(float, float, float, float);
	bool IsCC_C2(float, float, float, float);
	bool IsBM_C3(float, float, float, float);
	bool IsBG_C4(float, float, float, float);
	bool IsDJ(float, float);
	bool IsNCT_C1(float, float, float, float);
	bool IsNCG_C2(float, float, float, float);
	int insert(const string *sp, const int n);
	int update(const string *sp, const int n);
	int del(const string *sp, const int n);
	int st(const string *sp, const int n);
	int mb(const string *sp, const int n);
	int dj(const string *sp, const int n);
	int eg(const string *sp, const int n);
};