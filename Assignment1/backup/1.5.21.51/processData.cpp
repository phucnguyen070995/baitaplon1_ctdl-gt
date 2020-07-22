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

//Object LList
template <typename T>
void LList<T>::delFirstLink(Link<T> * link){
	if (cnt == 1){
		curr = link;
	}
	head = link;
	cnt--;
}

template <typename T>
Link<T> *LList<T>::Gethead()
{
	return this->head;
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

float ProcessData::max(float a,float b){
	if (a <= b) {return b;}
	else {return a;}
}

float ProcessData::min(float a ,float b){
	if (a <= b) {return a;}
	else {return b;}
}

bool ProcessData::IsBD_C1(float OP, float HP, float LP, float CP){
	if ((max(OP, CP) - min(OP, CP) <= 0.00005) && (HP - max(OP, CP) > 0.00005) && (min(OP, CP) - LP > 0.00005)) {return true;}
	else {return false;}
}

bool ProcessData::IsCC_C2(float OP, float HP, float LP, float CP){
	if ((max(OP, CP) - min(OP, CP) <= 0.00005) && (HP - max(OP, CP) <= 0.00005) && (min(OP, CP) - LP > 0.00005)) {return true;}
	else {return false;}
}

bool ProcessData::IsBM_C3(float OP, float HP, float LP, float CP){
	if ((max(OP, CP) - min(OP, CP) <= 0.00005) && (HP - max(OP, CP) > 0.00005) && (min(OP, CP) - LP <= 0.00005)) {return true;}
	else {return false;}
}

bool ProcessData::IsBG_C4(float OP, float HP, float LP, float CP){
	if (OP == HP && HP == LP && LP == CP) {return true;}
	else {return false;}
}

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
		case delCode:
			res = del(p,n);
			break;
		case stCode:
			res = st(p,n);
			break;
		case mbCode:
			res = mb(p,n);
			break;
		case djCode:
			res = dj(p,n);
			break;
			// case egCode:
			// 	res = eg(p,n);
			// 	break;
		default:
			res = -1;
		}
	}
	catch (invalid_argument iax){
		delete[] p;
		return res;
	}
	delete[] p;
	return res;
}

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
			delete[] dataExchange;
			return Findptr;
		}
		Findptr = Findptr->GetLinkNext();
		delete[] dataExchange;
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
		int n = 0;
		int flag = 0;
		Link<Candle> *FindingCandle = FindingExchange->GetDataLink().GetNextCandle();
		//Kiem tra Candle dau tien
		string *time;
		FindingCandle->GetDataLink().GetCandle(time);
		if (compareTime(sp[3],time[0]) && sp[3] != time[0]){
			newCandle->SetLink(FindingCandle);
			Exchange obExchange;
			obExchange.SetExchange(sp[1], sp[2]);
			obExchange.SetNextCandle(newCandle);
			FindingExchange->SetDataLink(obExchange);
			FindingCandle = FindingExchange->GetDataLink().GetNextCandle();
			flag = 1;
		}
		if (sp[3] == time[0]) {
			delete newCandle;
			flag = 1;
		}
		delete[] time;
		while (FindingCandle->GetLinkNext() != nullptr)
		{
			string *time;
			FindingCandle->GetLinkNext()->GetDataLink().GetCandle(time);
			if (compareTime(sp[3],time[0]) && sp[3] != time[0] && flag == 0){
				newCandle->SetLink(FindingCandle->GetLinkNext());
				FindingCandle->SetLink(newCandle);
				flag = 1;
			}
			if (sp[3] == time[0]) {
				delete newCandle;
				flag = 1;
			}
			n++;
			FindingCandle = FindingCandle->GetLinkNext();
			delete[] time;
		}
		if (flag == 0){
			FindingCandle->SetLink(newCandle);
			n++;
		}
		return ++n;
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

int ProcessData::del(const string *sp, const int n)
{
	Link<Exchange> *FindingExchange = FindExchange(sp[1], sp[2]);
	// Link<Exchange> *PreExchange;
	if (FindingExchange != nullptr)
	{
		//Dem Candle cua Exchange
		int count = 0;
		Link<Candle> *FindingCandle = FindingExchange->GetDataLink().GetNextCandle();
		while (FindingCandle != nullptr){
			count++;
			FindingCandle = FindingCandle->GetLinkNext();
		}
		int inicount = count;
		//Dem xong
		FindingCandle = FindingExchange->GetDataLink().GetNextCandle();
		if (n ==5){
			Link<Candle> *PreCandle = FindingExchange->GetDataLink().GetNextCandle();
			int idx1 = 0;
			int idx2 = 0;
			while (FindingCandle != nullptr){
				string *time;
				FindingCandle->GetDataLink().GetCandle(time);
				if (compareTime(time[0],sp[3]) && sp[3] != time[0]){
					idx1++;
					PreCandle = PreCandle->GetLinkNext();
				}
				if (compareTime(sp[4],time[0]) && sp[4] != time[0]){
					delete[] time;
					break;
				}
				delete[] time;
				FindingCandle = FindingCandle->GetLinkNext();
				idx2++;
			}
			if (idx1 == 0 && idx2 == 0) {return 0;}
			if (idx1 == 0 && idx2 != 0) {
				Exchange obExchange;
				obExchange.SetExchange(sp[1], sp[2]);
				obExchange.SetNextCandle(FindingCandle);
				FindingExchange->SetDataLink(obExchange);
				for (int i = 0; i < idx2; i++){
					Link<Candle> * Del = PreCandle;
					PreCandle = PreCandle->GetLinkNext();
					delete Del;
				}
				return idx2;
			}
			if (idx1 != 0 && idx2 != 0){
				for (int i = 0; i < idx2 - idx1; i++){
					Link<Candle> * Del = PreCandle->GetLinkNext();
					PreCandle = PreCandle->GetLinkNext();
					delete Del;
				}
				PreCandle->SetLink(FindingCandle);
				return idx2 - idx1;
			}
			else {return -1;}	
		}
		else if (n == 4){
			string *time;
			FindingCandle->GetDataLink().GetCandle(time);
			if (sp[3] == time[0]){
				if (count == 1){
					Link<Exchange> * delExchange = FindingExchange;
					Link<Exchange> * Predel = FindPreExchange(FindingExchange);
					if (Predel == nullptr) {data->delFirstLink(FindingExchange->GetLinkNext());}
					else{
						Predel->SetLink(FindingExchange->GetLinkNext());
					}
					delete delExchange;
					delete[] time;
					return 1;
				}
				else {
					Exchange obExchange;
					obExchange.SetExchange(sp[1], sp[2]);
					obExchange.SetNextCandle(FindingCandle->GetLinkNext());
					FindingExchange->SetDataLink(obExchange);
					delete FindingCandle;
					delete[] time;
					return 1;
				}
			}
			delete[] time;
			while (FindingCandle->GetLinkNext() != nullptr)
			{
				string *time;
				FindingCandle->GetLinkNext()->GetDataLink().GetCandle(time);
				if (sp[3] == time[0]){
					Link<Candle> * Del = FindingCandle->GetLinkNext();
					FindingCandle->SetLink(Del->GetLinkNext());
					delete[] time;
					delete Del;
					return 1;
				}
				FindingCandle = FindingCandle->GetLinkNext();
				delete[] time;
			}
			return 0;
		}
		else if (n == 3){
			Link<Exchange> * delExchange = FindingExchange;
			Link<Exchange> * Predel = FindPreExchange(FindingExchange);
			if (Predel == nullptr) {data->delFirstLink(FindingExchange->GetLinkNext());}
			else{
				Predel->SetLink(FindingExchange->GetLinkNext());
			}
			delete delExchange;
			return count;
		}
		else {return -1;}
	}
	else {
		return 0;
	}
}

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
			delete[] time;
		}
		if (FindingCandle == nullptr) {return 0;}
		else {
			Candle obCandle;
			obCandle.SetCandle(sp[3], sp[4], sp[5], sp[6], sp[7]);
			FindingCandle->SetDataLink(obCandle);
			string * test;
			FindingCandle->GetDataLink().GetCandle(test);
			delete[] test;
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
				delete[] time;
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
				delete[] time;
			}
			return count;
		}
		else {
			while (FindingCandle != nullptr){
				string *time;
				FindingCandle->GetDataLink().GetCandle(time);
				if ((stof(time[1]) == stof(time[2]) && stof(time[3]) == stof(time[4])) || (stof(time[1]) == stof(time[3]) && stof(time[2]) == stof(time[4]))) {count++;}
				FindingCandle = FindingCandle->GetLinkNext();
				delete[] time;
			}
			return count;
		}
	}
	else {return 0;}
}

int ProcessData::st(const string *sp, const int n){
	Link<Exchange> *FindingExchange = FindExchange(sp[1], sp[2]);
	if (FindingExchange != nullptr){
		Link<Candle> *FindingCandle = FindingExchange->GetDataLink().GetNextCandle();
		int count = 0;
		if (n==5){
			while (FindingCandle != nullptr){
				string *time;
				FindingCandle->GetDataLink().GetCandle(time);
				if (compareTime(sp[3],time[0]) && compareTime(time[0],sp[4])) {
					if ((max(stof(time[4]), stof(time[1])) - min(stof(time[4]), stof(time[1])) <= 0.00005) && (stof(time[2]) - max(stof(time[4]), stof(time[1])) > 0.00005) && (min(stof(time[4]), stof(time[1])) - stof(time[3]) > 0.00005)) {count++;}
				}
				FindingCandle = FindingCandle->GetLinkNext();
				delete[] time;
			}
			return count;
		}
		else if (n==4){
			while (FindingCandle != nullptr){
				string *time;
				FindingCandle->GetDataLink().GetCandle(time);
				if (sp[3] == time[0]) {
					if ((max(stof(time[4]), stof(time[1])) - min(stof(time[4]), stof(time[1])) <= 0.00005) && (stof(time[2]) - max(stof(time[4]), stof(time[1])) > 0.00005) && (min(stof(time[4]), stof(time[1])) - stof(time[3]) > 0.00005)) {count++;}
				}
				FindingCandle = FindingCandle->GetLinkNext();
				delete[] time;
			}
			return count;
		}
		else {
			while (FindingCandle != nullptr){
				string *time;
				FindingCandle->GetDataLink().GetCandle(time);
				if ((max(stof(time[4]), stof(time[1])) - min(stof(time[4]), stof(time[1])) <= 0.00005) && (stof(time[2]) - max(stof(time[4]), stof(time[1])) > 0.00005) && (min(stof(time[4]), stof(time[1])) - stof(time[3]) > 0.00005)) {count++;}
				FindingCandle = FindingCandle->GetLinkNext();
				delete[] time;
			}
			return count;
		}
	}
	else {return 0;}
}

int ProcessData::dj(const string *sp, const int n){
	Link<Exchange> *FindingExchange = FindExchange(sp[1], sp[2]);
	if (FindingExchange != nullptr){
		Link<Candle> *FindingCandle = FindingExchange->GetDataLink().GetNextCandle();
		int count = 0;
		if (n == 6){
			if (sp[3] == "C1"){
				while (FindingCandle != nullptr){
					string *time;
					FindingCandle->GetDataLink().GetCandle(time);
					if (compareTime(sp[4],time[0]) && compareTime(time[0],sp[5])) {
						bool one = IsBD_C1(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
						if (one) {count++;}
					}
					FindingCandle = FindingCandle->GetLinkNext();
					delete[] time;
				}
				return count;
			}
			else if (sp[3] == "C2"){
				while (FindingCandle != nullptr){
					string *time;
					FindingCandle->GetDataLink().GetCandle(time);
					if (compareTime(sp[4],time[0]) && compareTime(time[0],sp[5])) {
						bool two = IsCC_C2(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
						if (two) {count++;}
					}
					FindingCandle = FindingCandle->GetLinkNext();
					delete[] time;
				}
				return count;
			}
			else if (sp[3] == "C3"){
				while (FindingCandle != nullptr){
					string *time;
					FindingCandle->GetDataLink().GetCandle(time);
					if (compareTime(sp[4],time[0]) && compareTime(time[0],sp[5])) {
						bool three = IsBM_C3(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
						if (three) {count++;}
					}
					FindingCandle = FindingCandle->GetLinkNext();
					delete[] time;
				}
				return count;
			}
			else if (sp[3] == "C4"){
				while (FindingCandle != nullptr){
					string *time;
					FindingCandle->GetDataLink().GetCandle(time);
					if (compareTime(sp[4],time[0]) && compareTime(time[0],sp[5])) {
						bool four = IsBG_C4(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
						if (four) {count++;}
					}
					FindingCandle = FindingCandle->GetLinkNext();
					delete[] time;
				}
				return count;
			}
			else {return 0;}
		}
		else if (n == 5){
			if (sp[3] == "C1"){
				while (FindingCandle != nullptr){
					string *time;
					FindingCandle->GetDataLink().GetCandle(time);
					if (sp[4] == time[0]) {
						bool one = IsBD_C1(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
						if (one) {count++;}
					}
					FindingCandle = FindingCandle->GetLinkNext();
					delete[] time;
				}
				return count;
			}
			else if (sp[3] == "C2"){
				while (FindingCandle != nullptr){
					string *time;
					FindingCandle->GetDataLink().GetCandle(time);
					if (sp[4] == time[0]) {
						bool two = IsCC_C2(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
						if (two) {count++;}
					}
					FindingCandle = FindingCandle->GetLinkNext();
					delete[] time;
				}
				return count;
			}
			else if (sp[3] == "C3"){
				while (FindingCandle != nullptr){
					string *time;
					FindingCandle->GetDataLink().GetCandle(time);
					if (sp[4] == time[0]) {
						bool three = IsBM_C3(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
						if (three) {count++;}
					}
					FindingCandle = FindingCandle->GetLinkNext();
					delete[] time;
				}
				return count;
			}
			else if (sp[3] == "C4"){
				while (FindingCandle != nullptr){
					string *time;
					FindingCandle->GetDataLink().GetCandle(time);
					if (sp[4] == time[0]) {
						bool four = IsBG_C4(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
						if (four) {count++;}
					}
					FindingCandle = FindingCandle->GetLinkNext();
					delete[] time;
				}
				return count;
			}
			else {
				while (FindingCandle != nullptr){
					string *time;
					FindingCandle->GetDataLink().GetCandle(time);
					if (compareTime(sp[4],time[0]) && compareTime(time[0],sp[5])) {
						bool one = IsBD_C1(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
						bool two = IsCC_C2(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
						bool three = IsBM_C3(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
						bool four = IsBG_C4(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
						if (one || two || three || four) {count++;}
					}
					FindingCandle = FindingCandle->GetLinkNext();
					delete[] time;
				}
				return count;
			}
		}
		else if (n == 4){
			if (sp[3] == "C1"){
				while (FindingCandle != nullptr){
					string *time;
					FindingCandle->GetDataLink().GetCandle(time);
					bool one = IsBD_C1(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
					if (one) {count++;}
					FindingCandle = FindingCandle->GetLinkNext();
					delete[] time;
				}
				return count;
			}
			else if (sp[3] == "C2"){
				while (FindingCandle != nullptr){
					string *time;
					FindingCandle->GetDataLink().GetCandle(time);
					bool two = IsCC_C2(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
					if (two) {count++;}
					FindingCandle = FindingCandle->GetLinkNext();
					delete[] time;
				}
				return count;
			}
			else if (sp[3] == "C3"){
				while (FindingCandle != nullptr){
					string *time;
					FindingCandle->GetDataLink().GetCandle(time);
					bool three = IsBM_C3(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
					if (three) {count++;}
					FindingCandle = FindingCandle->GetLinkNext();
					delete[] time;
				}
				return count;
			}
			else if (sp[3] == "C4"){
				while (FindingCandle != nullptr){
					string *time;
					FindingCandle->GetDataLink().GetCandle(time);
					bool four = IsBG_C4(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
					if (four) {count++;}
					FindingCandle = FindingCandle->GetLinkNext();
					delete[] time;
				}
				return count;
			}
			else {
				while (FindingCandle != nullptr){
					string *time;
					FindingCandle->GetDataLink().GetCandle(time);
					if (sp[4] == time[0]) {
						bool one = IsBD_C1(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
						bool two = IsCC_C2(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
						bool three = IsBM_C3(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
						bool four = IsBG_C4(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
						if (one || two || three || four) {count++;}
					}
					FindingCandle = FindingCandle->GetLinkNext();
					delete[] time;
				}
				return count;
			}
		}
		else {
			while (FindingCandle != nullptr){
				string *time;
				FindingCandle->GetDataLink().GetCandle(time);
				bool one = IsBD_C1(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
				bool two = IsCC_C2(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
				bool three = IsBM_C3(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
				bool four = IsBG_C4(stof(time[1]), stof(time[2]), stof(time[3]), stof(time[4]));
				if (one || two || three || four) {count++;}
				FindingCandle = FindingCandle->GetLinkNext();
				delete[] time;
			}
			return count;
		}
	}
	else {return 0;}
}