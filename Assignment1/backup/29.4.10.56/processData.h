#include "main.h"

template <typename T>
class Link
{
private:
	T data;
	Link<T> *next;

public:
	Link<T>() { next = nullptr; }
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
	Exchange() { nextCandle = nullptr; };
	void SetExchange(string, string);
	void SetNextCandle(Link<Candle> *NextCandle);
	void GetExchange(string *&p);
	Link<Candle> *GetNextCandle();
	int AddCandleIntoExistExchange(Link<Exchange> *, Candle);
};
// template <typename T>
// class List {
//   // Return: The number of elements in the list.
//   	virtual int length() const = 0;
// 	virtual void insert(const T& ele) = 0;
// };
template <typename T>
class LList
{ //: public List<T> {
private:
	Link<T> *head;
	// Link<T>* last;
	Link<T> *curr;
	int cnt;

public:
	LList() { 
		head = curr = nullptr; 
		cnt = 0;
	} //last = curr =
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
	//int del(const T& link, const T& ele);
	void Setlength(int);
	void Sethead(Link<T> *firstLink);
	Link<T> *Gethead();
	// void Setlast(T * lastLink);
	// T * Getlast();
	void Setcurr(Link<T> *currLink);
	Link<T> *Getcurr();
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
	int insert(const string *sp, const int n);
	int update(const string *sp, const int n);
	int del(const string *sp, const int n);
	int st(const string *sp, const int n);
	int mb(const string *sp, const int n);
	int dj(const string *sp, const int n);
	int eg(const string *sp, const int n);
};