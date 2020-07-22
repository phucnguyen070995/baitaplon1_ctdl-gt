#include "main.h"

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
	void SetDataLink(T);
	void SetLink(Link<T> *link);
	T GetDataLink();
	Link<T> *GetLinkNext();
};

class Candle
{
private:
	string TIME;
	string OP;
	string HP;
	string LP;
	string CP;

public:
	void SetCandle(string TIME, string OP, string HP, string LP, string CP);
	void GetCandle(string *&p);
};

class Exchange
{
private:
	string BC;
	string QC;
	Link<Candle> *nextCandle;

public:
	void SetExchange(string, string);
	void SetNextCandle(Link<Candle> *NextCandle);
	void GetExchange(string *&p);
	Link<Candle> *GetNextCandle();
};

template <typename T>
class LList
{
private:
	Link<T> *head;
	Link<T> *curr;
	int cnt;
public:
	LList() { 
		head = curr = nullptr; 
		cnt = 0;
	}
	int length() const { return cnt; }
	void insert(const T &ele)
	{
		Link<T> *newExchange = new Link<T>(ele);
		if (head == nullptr){
			head = newExchange;
			curr = newExchange;
		}
		else{
			curr->SetLink(newExchange);
			curr = curr->GetLinkNext();
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
	bool compareTime(string, string);
	float max(float,float);
	float min(float,float);
	bool IsBD_C1(float, float, float, float);
	bool IsCC_C2(float, float, float, float);
	bool IsBM_C3(float, float, float, float);
	bool IsBG_C4(float, float, float, float);
	bool IsNCT_C1(string, string, string, string);
	bool IsNCG_C2(string, string, string, string);
	int insert(const string *sp, const int n);
	int update(const string *sp, const int n);
	int del(const string *sp, const int n);
	int st(const string *sp, const int n);
	int mb(const string *sp, const int n);
	int dj(const string *sp, const int n);
	int eg(const string *sp, const int n);
};