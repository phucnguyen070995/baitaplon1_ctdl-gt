#include "processData.h"

enum CodeValue
{
	insCode = 1,
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

// int Exchange::AddCandleIntoExistExchange(Link<Exchange> *FindedExchange, Candle obCandle)
// {
// 	Link<Candle> *Findptr = FindedExchange->GetDataLink().GetNextCandle();
// 	Link<Candle> *Smallerptr;
// 	float *valueobCandle;
// 	obCandle.GetCandle(valueobCandle);
// 	while (true)
// 	{
// 		if (Findptr == nullptr)
// 		{
// 		}
// 		float *valueCandle;
// 		Findptr->GetDataLink().GetCandle(valueCandle);
// 		if (valueCandle[0] == valueobCandle[0])
// 		{
// 			return 0;
// 		}
// 		Findptr = Findptr->GetLinkNext();
// 	}
// }

//Ham object Candle
void Candle::SetCandle(string TIME, string OP, string HP, string LP, string CP)
{
	this->TIME = TIME;
	this->OP = OP;
	this->HP = HP;
	this->LP = LP;
	this->CP = CP;
}

void Candle::GetCandle(string *&p)
{
	p = new string[5];
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
void LList<T>::delFirstLink(Link<T> * link){
	if (cnt == 1){
		curr = link;
	}
	head = link;
	cnt--;
}

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
//Time1 be hon hoac bang Time2
bool ProcessData::compareTime(string Time1, string Time2)
{
	if (Time1.length() < Time2.length()){return true;}
	else if (Time1.length() > Time2.length()){return false;}
	else{
		for (int i = 0; i < Time1.length(); i++){
			if (Time1[i] > Time2[i]) {return false;}
			if (Time1[i] < Time2[i]) {return true;}
		}
		return true;
	}
}
/* 
	Split the command line into instruction and its parameters
	(This version is simplified to return just the instruction only; students should rewrite to make it return instruction's parameters)
	Input:
		line: the command line
		sp: pass-by-reference parameter used as an output: return the array of strings keeping instruction and its parameter
	Output: return the number of elements in array sp.	
*/
int ProcessData::split(string line, string *&sp){
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

int ProcessData::process(string line){
	string *p;
	int n = ProcessData::split(line, p);
	if (n <= 0){
		delete[] p;
		return -1;
	}
	int res = -1;
	try{
		switch (s_mapCodeValues[p[0]]){
		case insCode:
			res = insert(p, n);
			break;
		case updCode:
			res = update(p,n);
			break;
		// case delCode:
		// 	res = del(p,n);
		// 	break;
			// case stCode:
			// 	res = st(p,n);
			// 	break;
		case mbCode:
			res = mb(p,n);
			break;
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
	catch (invalid_argument iax){
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

Link<Exchange> *ProcessData::FindPreExchange(Link<Exchange> * link)
{
	Link<Exchange> *Findptr = data->Gethead();
	if (Findptr == link){return nullptr;}
	while (Findptr->GetLinkNext() != link)
	{
		Findptr = Findptr->GetLinkNext();
	}
	return Findptr;
}

//Lenh insert
int ProcessData::insert(const string *sp, const int n)
{
	Link<Exchange> *FindingExchange = FindExchange(sp[1], sp[2]);
	//Cac lenh xem nhu da valid
	//Tao Link<Candle>
	Candle obCandle;
	obCandle.SetCandle(sp[3], sp[4], sp[5], sp[6], sp[7]);
	Link<Candle> *newCandle = new Link<Candle>(obCandle);

	if (FindingExchange != nullptr)
	{
		int flag = 0;
		int n = 0;
		Link<Candle> *FindingCandle = FindingExchange->GetDataLink().GetNextCandle();
		while (FindingCandle != nullptr)
		{
			string *time;
			FindingCandle->GetDataLink().GetCandle(time);
			if (sp[3] == time[0])
			{
				flag = 1;
				break;
			}
			FindingCandle = FindingCandle->GetLinkNext();
		}

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
			delete newCandle;
			return n;
		}
	}
	else
	{
		Exchange obExchange;
		obExchange.SetExchange(sp[1], sp[2]);
		obExchange.SetNextCandle(newCandle);
		data->insert(obExchange);
		return 1;
	}
}


// int ProcessData::del(const string *sp, const int n)
// {
// 	Link<Exchange> *FindingExchange = FindExchange(sp[1], sp[2]);
// 	// Link<Exchange> *PreExchange;
// 	if (FindingExchange != nullptr)
// 	{
// 		//Dem Candle cua Exchange
// 		int count = 0;
// 		Link<Candle> *FindingCandle = FindingExchange->GetDataLink().GetNextCandle();
// 		while (FindingCandle->GetLinkNext() != nullptr){
// 			count++;
// 			FindingCandle = FindingCandle->GetLinkNext();
// 		}
// 		count++;
// 		int inicout = count;
// 		FindingCandle = FindingExchange->GetDataLink().GetNextCandle();
// 		cout<< count<<endl;
// 		if (n ==5){
// 			cout << "im here"<<endl;
// 			while (count > 0){	
// 				string* time;
// 				FindingCandle->GetDataLink().GetCandle(time);
// 				cout << time[0]<<endl;
// 				if (compareTime(sp[3],time[0]) && compareTime(time[0],sp[4])){
// 					Link<Candle> * delPtr = FindingCandle;
// 					Link<Candle> * p =nullptr;
// 					cout<<FindingExchange<<endl;
// 					cout<<delPtr<<endl;
// 					FindingExchange->GetDataLink().SetNextCandle(p);
// 					cout<<FindingCandle<<endl;
// 					cout<<FindingExchange->GetDataLink().GetNextCandle()<<endl;
// 					FindingCandle = FindingCandle->GetLinkNext();
// 					cout<<FindingCandle<<endl;
// 					count--;
// 					delete delPtr;
// 					cout<<"minus"<<endl;
// 				}
// 				else {
// 					cout<<FindingCandle<<endl;
// 					cout<<"breaking"<<endl;
// 					break;
// 				}
// 				cout<<count<<endl;
// 				cout<<FindingCandle<<endl;
// 			}
// 			cout<<"Hello"<<endl;
// 			cout<<count<<endl;
// 			while (FindingCandle->GetLinkNext() != nullptr){
// 				cout << "im here"<<endl;
// 				cout<<FindingCandle<<endl;
// 				string* time;
// 				FindingCandle->GetLinkNext()->GetDataLink().GetCandle(time);
// 				cout<<time[0]<<endl;
// 				if (compareTime(sp[3],time[0]) && compareTime(time[0],sp[4])){
// 					Link<Candle> * delPtr = FindingCandle->GetLinkNext();
// 					FindingCandle->SetLink(delPtr->GetLinkNext());
// 					count--;
// 					delete delPtr;
// 					cout << "im in"<<endl;
// 				}
// 				else {
// 					FindingCandle = FindingCandle->GetLinkNext();
// 				}
// 			}
// 			cout<<count<<endl;
// 			if (count == 0) {
// 				cout << "im here"<<endl;
// 				Link<Exchange> * delExchange = FindingExchange;
// 				Link<Exchange> * Predel = FindPreExchange(FindingExchange);
// 				Predel->SetLink(FindingExchange->GetLinkNext());
// 				delete delExchange;
// 				return inicout;
// 			}
// 			else {return inicout - count;}
// 		}
// 		else if (n == 4){
// 			while (true){
// 				string* time;
// 				FindingCandle->GetDataLink().GetCandle(time);
// 				if (time[0] == sp[3]){
// 					Link<Candle> * delPtr = FindingCandle;
// 					FindingExchange->GetDataLink().GetNextCandle()->SetLink(FindingCandle->GetLinkNext());
// 					FindingCandle = FindingCandle->GetLinkNext();
// 					delete delPtr;
// 					return 1;
// 				}
// 				else {break;}
// 			}
// 			while (true){
// 				string* time;
// 				FindingCandle->GetLinkNext()->GetDataLink().GetCandle(time);
// 				if (time[0] == sp[3]){
// 					Link<Candle> * delPtr = FindingCandle->GetLinkNext();
// 					FindingCandle->SetLink(delPtr->GetLinkNext());
// 					delete delPtr;
// 					return 1;
// 				}
// 				else if (FindingCandle->GetLinkNext() != nullptr){
// 					FindingCandle = FindingCandle->GetLinkNext();
// 				}
// 			}
// 			return 0;
// 		}
// 		else if (n == 3){
// 			Link<Exchange> * delExchange = FindingExchange;
// 			Link<Exchange> * Predel = FindPreExchange(FindingExchange);
// 			if (Predel == nullptr) {data->delFirstLink(FindingExchange->GetLinkNext());}
// 			else{
// 				Predel->SetLink(FindingExchange->GetLinkNext());
// 			}
			
// 			cout<<delExchange<<endl;
// 			cout<<Predel<<endl;
// 			delete delExchange;
// 			return count;
// 		}
// 	}
// 	else {
// 		return 0;
// 	}
// }

int ProcessData::update(const string *sp, const int n){
	Link<Exchange> *FindingExchange = FindExchange(sp[1], sp[2]);
	if (FindingExchange != nullptr){
		Link<Candle> *FindingCandle = FindingExchange->GetDataLink().GetNextCandle();
		while (FindingCandle != nullptr)
		{
			string *time;
			FindingCandle->GetDataLink().GetCandle(time);
			if (sp[3] == time[0]) {break;}
			FindingCandle = FindingCandle->GetLinkNext();
		}
		if (FindingCandle == nullptr) {return 0;}
		else {
			Candle obCandle;
			obCandle.SetCandle(sp[3], sp[4], sp[5], sp[6], sp[7]);
			FindingCandle->SetDataLink(obCandle);
			string * test;
			FindingCandle->GetDataLink().GetCandle(test);
			return 1;
		}
	}
	else {return 0;}
}

int ProcessData::mb(const string *sp, const int n){
	Link<Exchange> *FindingExchange = FindExchange(sp[1], sp[2]);
	if (FindingExchange != nullptr){
		Link<Candle> *FindingCandle = FindingExchange->GetDataLink().GetNextCandle();
		int count = 0;
		if (n==5){
			while (FindingCandle != nullptr){
				string *time;
				FindingCandle->GetDataLink().GetCandle(time);
				if (compareTime(sp[3],time[0]) && compareTime(time[0],sp[4])) {
					if ((stof(time[1]) == stof(time[2]) && stof(time[3]) == stof(time[4])) || (stof(time[1]) == stof(time[3]) && stof(time[2]) == stof(time[4]))) {count++;}
				}
				FindingCandle = FindingCandle->GetLinkNext();
			}
			return count;
		}
		else if (n==4){
			while (FindingCandle != nullptr){
				string *time;
				FindingCandle->GetDataLink().GetCandle(time);
				if (sp[3] == time[0]) {
					if ((stof(time[1]) == stof(time[2]) && stof(time[3]) == stof(time[4])) || (stof(time[1]) == stof(time[3]) && stof(time[2]) == stof(time[4]))) {count++;}
				}
				FindingCandle = FindingCandle->GetLinkNext();
			}
			return count;
		}
		else {
			while (FindingCandle != nullptr){
				string *time;
				FindingCandle->GetDataLink().GetCandle(time);
				if ((stof(time[1]) == stof(time[2]) && stof(time[3]) == stof(time[4])) || (stof(time[1]) == stof(time[3]) && stof(time[2]) == stof(time[4]))) {count++;}
				FindingCandle = FindingCandle->GetLinkNext();
			}
			return count;
		}
	}
	else {return 0;}
}
