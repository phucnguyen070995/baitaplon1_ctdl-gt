#include "processData.h"

enum CodeValue
{
	insCode,
	delCode,
	updCode,
	stCode,
	mbCode,
	djCode,
	egCode,
	comCode
};

static map<string, CodeValue> s_mapCodeValues = {
	{"INS", insCode},
	{"DEL", delCode},
	{"UPD", updCode},
	{"ST", stCode},
	{"MB", mbCode},
	{"DJ", djCode},
	{"EG", egCode}};

// int LList<Exchange>::del(const Exchange & ele){}

//Ham object Exchange
void Exchange::SetExchange(string BC, string QC)
{
	this->BC = BC;
	this->QC = QC;
}

void Exchange::SetNextCandle(Link<Candle> *NextCandle)
{
	this->nextCandle = NextCandle;
}

void Exchange::GetExchange(string *&p)
{
	p = new string[2];
	p[0] = this->BC;
	p[1] = this->QC;
}

Link<Candle> *Exchange::GetNextCandle()
{
	return this->nextCandle;
}

int Exchange::AddCandleIntoExistExchange(Link<Exchange> *FindedExchange, Candle obCandle)
{
	Link<Candle> *Findptr = FindedExchange->GetDataLink().GetNextCandle();
	Link<Candle> *Smallerptr;
	float *valueobCandle;
	obCandle.GetCandle(valueobCandle);
	while (true)
	{
		if (Findptr == nullptr)
		{
		}
		float *valueCandle;
		Findptr->GetDataLink().GetCandle(valueCandle);
		if (valueCandle[0] == valueobCandle[0])
		{
			return 0;
		}
		Findptr = Findptr->GetLinkNext();
	}
}

//Ham object Candle
void Candle::SetCandle(int TIME, float OP, float HP, float LP, float CP)
{
	this->TIME = TIME;
	this->OP = OP;
	this->HP = HP;
	this->LP = LP;
	this->CP = CP;
}

void Candle::GetCandle(float *&p)
{
	p = new float[5];
	p[0] = this->TIME;
	p[1] = this->OP;
	p[2] = this->HP;
	p[3] = this->LP;
	p[4] = this->CP;
}

//Ham object Link
template <typename T>
void Link<T>::SetDataLink(T object)
{
	this->data = object;
}

template <typename T>
void Link<T>::SetLink(Link<T> *link)
{
	this->next = link;
}

template <typename T>
T Link<T>::GetDataLink()
{
	return this->data;
}

template <typename T>
Link<T> *Link<T>::GetLinkNext()
{
	return this->next;
}

//Ham object LList
// template <typename T>
// int LList<T>::del(const T& link, const T& ele){
// 	Link<Exchange> *pPre = head;
// 	while (pPre->next->next == nullptr){

// 	}
// 	this->Setlength(-1);
// }

// template <typename T>
// int LList<T>::insert(const T* &ele){
// 	if (this->head == nullptr){
// 		this->head = this->curr = ele;
// 	}
// 	else{
// 		this->curr->next = ele;
// 		this->curr = ele;
// 	}
// 	this->Setlength(1);
// 	return 1;
// }

template <typename T>
void LList<T>::Setlength(int num)
{
	this->cnt = this->cnt + num;
}

template <typename T>
Link<T> *LList<T>::Gethead()
{
	return this->head;
}

// template <typename T>
// T * LList<T>::Getlast(){
// 	return this->last;
// }

template <typename T>
Link<T> *LList<T>::Getcurr()
{
	return this->curr;
}

//Ham object ProcessData
ProcessData::ProcessData()
{
	LList<Exchange> *myData = new LList<Exchange>;
	this->data = myData;
}

ProcessData::~ProcessData()
{
	delete data;
}
/* 
	Split the command line into instruction and its parameters
	(This version is simplified to return just the instruction only; students should rewrite to make it return instruction's parameters)
	Input:
		line: the command line
		sp: pass-by-reference parameter used as an output: return the array of strings keeping instruction and its parameter
	Output: return the number of elements in array sp.	
*/
int ProcessData::split(string line, string *&sp)
{
	sp = new string[MAXSIZECODE];
	const string SEP = " ";
	int pos, lastpos = 0, idx = 0;
	// Them parameters vao sp
	while (line.find(SEP, lastpos) != std::string::npos)
	{
		pos = line.find(SEP, lastpos);
		sp[idx] = line.substr(lastpos, pos - lastpos);
		lastpos = pos + 1;
		idx++;
	}
	sp[idx++] = line.substr(lastpos, line.length() - 1 - pos);
	return idx;
}

int ProcessData::process(string line)
{
	string *p;
	int n = ProcessData::split(line, p);
	if (n <= 0)
	{
		delete[] p;
		return -1;
	}
	int res = -1;
	try
	{
		switch (s_mapCodeValues[p[0]])
		{
		case insCode:
			res = insert(p, n);
			break;
			// case updCode:
			// 	res = update(p,n);
			// 	break;
			// case delCode:
			// 	res = del(p,n);
			// 	break;
			// case stCode:
			// 	res = st(p,n);
			// 	break;
			// case mbCode:
			// 	res = mb(p,n);
			// 	break;
			// case djCode:
			// 	res = dj(p,n);
			// 	break;
			// case egCode:
			// 	res = eg(p,n);
			// 	break;
			// default:
			// 	res = -1;
		}
	}
	catch (invalid_argument iax)
	{
		delete[] p;
		return res;
	}
	delete[] p;
	return res;
}
/*
	To process insert instruction
	(This is the simplified version)
	Input:
		sp: pointer to string array keeping instruction and its parameters
		n: the number of elements in sp
	Output:
		return the result as required
*/
Link<Exchange> *ProcessData::FindExchange(string BC, string QC)
{
	Link<Exchange> *Findptr = data->Gethead();
	while (true)
	{
		if (Findptr == nullptr)
		{
			return nullptr;
		}
		string *dataExchange;
		Findptr->GetDataLink().GetExchange(dataExchange);
		if (dataExchange[0] == BC && dataExchange[1] == QC)
		{
			return Findptr;
		}
		Findptr = Findptr->GetLinkNext();
	}
}

int ProcessData::insert(const string *sp, const int n)
{
	Link<Exchange> *FindingExchange = FindExchange(sp[1], sp[2]);
	//Cac lenh xem nhu da valid
	//Tao Link<Candle>
	Candle obCandle;
	obCandle.SetCandle(stoi(sp[3]), stof(sp[4]), stof(sp[5]), stof(sp[6]), stof(sp[7]));
	Link<Candle> *newCandle = new Link<Candle>(obCandle);

	if (FindingExchange != nullptr)
	{
		cout <<1 <<endl;
		int flag = 0;
		int n = 0;
		Link<Candle> *FindingCandle = FindingExchange->GetDataLink().GetNextCandle();
		while (FindingCandle != nullptr)
		{
			float *time;
			FindingCandle->GetDataLink().GetCandle(time);
			cout<<stoi(sp[3]) <<" "<< time[0]<<(stoi(sp[3]) == time[0]?"true":"false")<<endl;
			if (stoi(sp[3]) == time[0])
			{
				flag = 1;
				break;
			}
			FindingCandle = FindingCandle->GetLinkNext();
		}


		cout<<flag<<endl;


		FindingCandle = FindingExchange->GetDataLink().GetNextCandle();
		while (FindingCandle->GetLinkNext() != nullptr)
		{
			n++;
			FindingCandle = FindingCandle->GetLinkNext();
		}
		n++;
		if (flag == 0)
		{
			FindingCandle->SetLink(newCandle);
			return ++n;
		}
		else
		{
			return n;
			delete newCandle;
		}
	}
	else
	{
		cout <<2 <<endl;
		Exchange obExchange;
		obExchange.SetExchange(sp[1], sp[2]);
		obExchange.SetNextCandle(newCandle);

		data->insert(obExchange);
		string *p1;
		data->Gethead()->GetDataLink().GetExchange(p1);
		cout << p1[0] << " " << p1[1] << endl;
		float *num;
		data->Gethead()->GetDataLink().GetNextCandle()->GetDataLink().GetCandle(num);
		cout << num[0] << " " << num[1] << " " << num[2] << " " << num[3] << " " << num[4] << endl;
		return 1;
	}
}
