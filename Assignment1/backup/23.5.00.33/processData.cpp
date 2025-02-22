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
	this->numCandle = 1;
	this->nextCandle = nullptr;
	this->lastCandle = nullptr;
}

void Exchange::SetNextCandle(Link<Candle> *NextCandle)
{
	this->nextCandle = NextCandle;
}

void Exchange::SetLastCandle(Link<Candle> *LastCandle)
{
	this->lastCandle = LastCandle;
}

Link<Candle> *Exchange::GetFirstCandle()
{
	return this->nextCandle;
}

Link<Candle> *Exchange::GetLastCandle()
{
	return this->lastCandle;
}

//Ham object Candle
void Candle::SetCandle(string TIME, string OP, string HP, string LP, string CP)
{
	this->TIME = stol(TIME);
	this->OP = stof(OP);
	this->HP = stof(HP);
	this->LP = stof(LP);
	this->CP = stof(CP);
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
T *Link<T>::GetDataLink()
{
	return &this->data;
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
		last = link;
	}
	head = link;
	cnt--;
}

template <typename T>
Link<T> *LList<T>::Gethead()
{
	return this->head;
}

template <typename T>
void LList<T>::setLast(Link<T> * link){
	this->last = link;
}

template <typename T>
void LList<T>::setlength(int length){
	this->cnt += length;
}

//Ham object ProcessData
ProcessData::ProcessData()
{
	LList<Exchange> *myData = new LList<Exchange>;
	this->data = myData;
}

ProcessData::~ProcessData()
{
	Link<Exchange> * DelExchange = data->Gethead();
	while (DelExchange != nullptr){
		Link<Exchange> * tempExchange = DelExchange;
		Link<Candle> * DelCandle = DelExchange->GetDataLink()->GetFirstCandle();
		while (DelCandle != nullptr){
			Link<Candle> * tempCandle = DelCandle;
			DelCandle = DelCandle->GetLinkNext();
			delete tempCandle;
		}
		DelExchange = DelExchange->GetLinkNext();
		delete tempExchange;
	}
	delete data;
}

float ProcessData::max(float a,float b){
	if (a <= b) {return b;}
	else {return a;}
}

float ProcessData::min(float a ,float b){
	if (a <= b) {return a;}
	else {return b;}
}

bool ProcessData::IsMB(float OP, float HP, float LP, float CP){
	if ((max(CP, OP) - min(OP, CP) - 0.00005 > EP) && (max(CP, OP) == HP) && (min(CP, OP) == LP)) {return true;}
	else {return false;}
}

bool ProcessData::IsST(float OP, float HP, float LP, float CP){
	if (((max(OP, CP) - min(OP, CP) <= 0.00005) || (max(OP, CP) - min(OP, CP) - 0.00005 < EP)) && (HP - max(OP, CP) - 0.00005 > EP) && (min(OP, CP) - LP - 0.00005 > EP)) {return true;}
	else {return false;}
}

bool ProcessData::IsBD_C1(float OP, float HP, float LP, float CP){
	if (((max(OP, CP) - min(OP, CP) <= 0.00002) || (max(OP, CP) - min(OP, CP) - 0.00002 < EP)) && (HP - max(OP, CP) - 0.00005 > EP) && (min(OP, CP) - LP - 0.00005 > EP)) {return true;}
	else {return false;}
}

bool ProcessData::IsCC_C2(float OP, float HP, float LP, float CP){
	if (((max(OP, CP) - min(OP, CP) <= 0.00002) || (max(OP, CP) - min(OP, CP) - 0.00002 < EP)) && (HP - max(OP, CP) < EP) && (min(OP, CP) - LP - 0.00005 > EP)) {return true;}
	else {return false;}
}

bool ProcessData::IsBM_C3(float OP, float HP, float LP, float CP){
	if (((max(OP, CP) - min(OP, CP) <= 0.00002) || (max(OP, CP) - min(OP, CP) - 0.00002 < EP)) && (HP - max(OP, CP) - 0.00005 > EP) && (min(OP, CP) - LP < EP)) {return true;}
	else {return false;}
}

bool ProcessData::IsBG_C4(float OP, float HP, float LP, float CP){
	if (OP == HP && HP == LP && LP == CP) {return true;}
	else {return false;}
}

bool ProcessData::IsDJ(float OP, float CP){
	if ((max(CP, OP) - min(CP, OP) <= 0.00002) || (max(CP, OP) - min(CP, OP) - 0.00002 < EP)) {return true;}
	else {return false;}
}

bool ProcessData::IsNCT_C1(float OP1, float CP1, float OP2, float CP2){
	if ((OP1 >= CP1 || max(OP1, CP1) - min(OP1, CP1) < EP) && CP1 - OP2 > EP && CP2 - OP1 > EP) {return true;}
	else {return false;}
}

bool ProcessData::IsNCG_C2(float OP1, float CP1, float OP2, float CP2){
	if ((OP1 <= CP1 || max(OP1,CP1) - min(OP1,CP1) < EP) &&  OP2 - CP1 > EP && OP1 - CP2 > EP) {return true;}
	else {return false;}
}

bool ProcessData::isSplit(const string * p, int n){
	if (p[1] == p[2]) {return false;}
	for (int i = 0; i < n; i++){
		if (p[i] == "") {return false;}
	}
	return true;
}

bool ProcessData::isTime(string s){
	for (int i = 0; i < s.length(); i++)
		if (isdigit(s[i]) == false)
			return false;
	return true;
}

bool ProcessData::isValue(string s){
	int count = 0;
	for (int i = 0; i < s.length(); i++) {
		if (s[i] == '.') {
			count++;
			if (count > 1) {return false;}
		}
		if (isdigit(s[i]) == false && s[i] != '.') {
			return false;
		}
	}
	return true;
}

bool ProcessData::isCandle(string time, string OP, string HP, string LP, string CP){
	if (!isTime(time)) {return false;}
	if (!isValue(OP) || !isValue(HP) || !isValue(LP) || !isValue(CP)) {return false;}
	float OP1 = stof(OP);
	float HP1 = stof(HP);
	float LP1 = stof(LP);
	float CP1 = stof(CP);
	if (HP1 < LP1 || HP1 < max(OP1, CP1) || LP1 > min(OP1, CP1)) {return false;}
	if (OP1 == 0 || HP1 == 0 || LP1 == 0 || CP1 == 0) {return false;}
	return true;
}

int ProcessData::split(string line, string *&sp){
	sp = new string[MAXSIZECODE];
	const string SEP = " ";
	int pos, lastpos = 0, idx = 0;
	// Them parameters vao sp
	while (idx < MAXSIZECODE - 1 && line.find(SEP, lastpos) != std::string::npos)
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
			case egCode:
				res = eg(p,n);
				break;
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

Link<Exchange> *ProcessData::FindExchange(string BC, string QC){
	Link<Exchange> *Findptr = data->Gethead();
	while (true)
	{
		if (Findptr == nullptr)
		{
			return nullptr;
		}
		Exchange *dataExchange = Findptr->GetDataLink();
		if (dataExchange->getBC() == BC && dataExchange->getQC() == QC)
		{
			return Findptr;
		}
		Findptr = Findptr->GetLinkNext();
	}
}

Link<Exchange> *ProcessData::FindPreExchange(Link<Exchange> * link){
	Link<Exchange> *Findptr = data->Gethead();
	if (Findptr == link){return nullptr;}
	while (Findptr->GetLinkNext() != link)
	{
		Findptr = Findptr->GetLinkNext();
	}
	return Findptr;
}

int ProcessData::insert(const string *sp, const int n){
	if (n != 8) {return -1;}
	if (!isSplit(sp, n)) {return -1;}
	if (!isCandle(sp[3], sp[4], sp[5], sp[6], sp[7])) {return -1;}
	Link<Exchange> *FindingExchange = FindExchange(sp[1], sp[2]);
	//Tao Link<Candle>
	Candle obCandle;
	obCandle.SetCandle(sp[3], sp[4], sp[5], sp[6], sp[7]);
	Link<Candle> *newCandle = new Link<Candle>(obCandle);
	if (FindingExchange != nullptr)
	{
		Link<Candle> *FindingCandle = FindingExchange->GetDataLink()->GetFirstCandle();
		int numMyCandle = FindingExchange->GetDataLink()->getnumCandle();
		//Kiem tra Candle dau tien
		Candle *dataCandle = FindingCandle->GetDataLink();
		long timeIns = stol(sp[3]);
		if (timeIns < dataCandle->getTime()){
			newCandle->SetLink(FindingCandle);
			Exchange *obExchange = FindingExchange->GetDataLink();
			obExchange->SetNextCandle(newCandle);
			obExchange->setnumCandle(1);
			return numMyCandle + 1;
		}
		if (timeIns == dataCandle->getTime()) {
			delete newCandle;
			return numMyCandle;
		}
		//
		//Kiem tra Candle cuoi cung
		Link<Candle> *lastCandle = FindingExchange->GetDataLink()->GetLastCandle();
		Candle *dataLastCandle = lastCandle->GetDataLink();
		if (timeIns > dataLastCandle->getTime()){
			lastCandle->SetLink(newCandle);
			Exchange *obExchange = FindingExchange->GetDataLink();
			obExchange->setnumCandle(1);
			obExchange->SetLastCandle(newCandle);
			return numMyCandle + 1;
		}
		if (timeIns == dataLastCandle->getTime()) {
			delete newCandle;
			return numMyCandle;
		}
		//
		while (FindingCandle->GetLinkNext() != nullptr)
		{
			dataCandle = FindingCandle->GetLinkNext()->GetDataLink();
			if (timeIns < dataCandle->getTime()){
				newCandle->SetLink(FindingCandle->GetLinkNext());
				FindingCandle->SetLink(newCandle);
				Exchange *obExchange = FindingExchange->GetDataLink();
				obExchange->setnumCandle(1);
				return numMyCandle + 1;
			}
			if (timeIns == dataCandle->getTime()) {
				delete newCandle;
				return numMyCandle;
			}
			FindingCandle = FindingCandle->GetLinkNext();
		}
		return -1;
	}
	else
	{
		Exchange obExchange;
		obExchange.SetExchange(sp[1], sp[2]);
		obExchange.SetNextCandle(newCandle);
		obExchange.SetLastCandle(newCandle);
		data->insert(obExchange);
		return 1;
	}
}

int ProcessData::del(const string *sp, const int n){
	if (!isSplit(sp, n)) {return -1;}
	if (n == 4 && !isTime(sp[3])) {return -1;}
	if (n == 5 && (!isTime(sp[3]) || !isTime(sp[4]))) {return -1;}
	if (n < 3 || n > 5) {return -1;}
	if (isTime(sp[n-2]) && isTime(sp[n-1]) && stol(sp[n-2]) > stol(sp[n-1])) {return -1;}
	Link<Exchange> *FindingExchange = FindExchange(sp[1], sp[2]);
	if (FindingExchange != nullptr)
	{
		int count = FindingExchange->GetDataLink()->getnumCandle();
		Link<Candle> *FindingCandle = FindingExchange->GetDataLink()->GetFirstCandle();
		Link<Candle> *FirstCandle = FindingExchange->GetDataLink()->GetFirstCandle();
		Link<Candle> *LastCandle = FindingExchange->GetDataLink()->GetLastCandle();
		if (n == 5){
			long timeA = stol(sp[3]);
			long timeB = stol(sp[4]);
			if (timeB < FirstCandle->GetDataLink()->getTime() || timeA > LastCandle->GetDataLink()->getTime()) {return 0;}
			int num = 0;
			//Xoa cac candle dau tien thoa dk thoi gian
			while (FindingCandle != nullptr){
				Candle *dataCandle = FindingCandle->GetDataLink();
				long timedataCandle = dataCandle->getTime();
				if (timeA <= timedataCandle && timedataCandle <= timeB) {
					Link<Candle> *DelCandle = FindingCandle;
					Exchange *obExchange = FindingExchange->GetDataLink();
					obExchange->SetNextCandle(FindingCandle->GetLinkNext());
					obExchange->setnumCandle(-1);
					delete DelCandle;
					num++;
				}
				else{
					break;
				}
				FindingCandle = FindingExchange->GetDataLink()->GetFirstCandle();
			}
			//Neu xoa het candle luon roi
			if (FindingCandle == nullptr){
				Link<Exchange> * delExchange = FindingExchange;
				Link<Exchange> * Predel = FindPreExchange(FindingExchange);
				if (FindingExchange->GetLinkNext() == nullptr){
					data->setLast(Predel);
					data->setlength(-1);
				}
				if (Predel == nullptr) {data->delFirstLink(FindingExchange->GetLinkNext());}
				else{
					Predel->SetLink(FindingExchange->GetLinkNext());
				}
				delete delExchange;
				return count;
			}
			//Xoa chua het
			if (num != 0) {
				return num;
			}
			//Kiem tra candle cuoi cung
			bool flag = false;
			if (timeA <= LastCandle->GetDataLink()->getTime() && LastCandle->GetDataLink()->getTime() <= timeB) {flag = true;}
			//Kiem tra cac candle ben trong
			while (FindingCandle->GetLinkNext() != nullptr)
			{
				Candle *dataCandle = FindingCandle->GetLinkNext()->GetDataLink();
				long timedataCandle = dataCandle->getTime();
				if (timeA <= timedataCandle && timedataCandle <= timeB) {
					Link<Candle> * Del = FindingCandle->GetLinkNext();
					FindingCandle->SetLink(Del->GetLinkNext());
					FindingExchange->GetDataLink()->setnumCandle(-1);
					delete Del;
					num++;
					continue;
				}
				if (timedataCandle > timeB) {
					break;
				}
				FindingCandle = FindingCandle->GetLinkNext();
			}
			if (flag) {
				FindingExchange->GetDataLink()->SetLastCandle(FindingCandle);
			}
			return num;
		}
		else if (n == 4){
			long timeA = stol(sp[3]);
			//Kiem tra candle cuoi
			if (timeA < FirstCandle->GetDataLink()->getTime() || timeA > LastCandle->GetDataLink()->getTime()) {return 0;}
			//
			Candle *dataCandle = FindingCandle->GetDataLink();
			long timedataCandle = dataCandle->getTime();
			//Kiem tra candle dau tien
			if (timeA == timedataCandle) {
				if (count == 1){
					Link<Exchange> *delExchange = FindingExchange;
					Link<Exchange> *Predel = FindPreExchange(FindingExchange);
					if (FindingExchange->GetLinkNext() == nullptr){
						data->setLast(Predel);
						data->setlength(-1);
					}
					if (Predel == nullptr) {data->delFirstLink(FindingExchange->GetLinkNext());}
					else{
						Predel->SetLink(FindingExchange->GetLinkNext());
					}
					delete delExchange;
					return 1;
				}
				else {
					Exchange *obExchange = FindingExchange->GetDataLink();
					obExchange->SetNextCandle(FindingCandle->GetLinkNext());
					obExchange->setnumCandle(-1);
					delete FindingCandle;
					return 1;
				}
			}
			//Kiem tra candle cuoi cung
			bool flag = false;
			if (timeA == LastCandle->GetDataLink()->getTime()) {flag = true;}
			//
			bool flag1 = false;
			while (FindingCandle->GetLinkNext() != nullptr)
			{
				Candle *dataCandle = FindingCandle->GetLinkNext()->GetDataLink();
				long timedataCandle = dataCandle->getTime();
				if (timeA == timedataCandle) {
					Link<Candle> * Del = FindingCandle->GetLinkNext();
					FindingCandle->SetLink(Del->GetLinkNext());
					FindingExchange->GetDataLink()->setnumCandle(-1);
					delete Del;
					flag1 = true;
					break;
				}
				FindingCandle = FindingCandle->GetLinkNext();
			}
			if (flag) {
				FindingExchange->GetDataLink()->SetLastCandle(FindingCandle);
				return 1;
			}
			if (flag1) {return 1;}
			return 0;
		}
		else if (n == 3){
			Link<Exchange> * delExchange = FindingExchange;
			Link<Candle> * DelCandle = delExchange->GetDataLink()->GetFirstCandle();
			while (DelCandle != nullptr){
				Link<Candle> * tempCandle = DelCandle;
				DelCandle = DelCandle->GetLinkNext();
				delete tempCandle;
			}
			Link<Exchange> * Predel = FindPreExchange(FindingExchange);
			if (FindingExchange->GetLinkNext() == nullptr){
				data->setLast(Predel);
				data->setlength(-1);
			}
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
	if (!isSplit(sp, n)) {return -1;}
	if (!isCandle(sp[3], sp[4], sp[5], sp[6], sp[7])) {return -1;}
	if (n != 8) {return -1;}
	Link<Exchange> *FindingExchange = FindExchange(sp[1], sp[2]);
	long timeIns = stol(sp[3]);
	if (FindingExchange != nullptr){
		Link<Candle> *FindingCandle = FindingExchange->GetDataLink()->GetFirstCandle();
		Link<Candle> *LastCandle = FindingExchange->GetDataLink()->GetLastCandle();
		if (timeIns < FindingCandle->GetDataLink()->getTime() || timeIns > LastCandle->GetDataLink()->getTime()) {return 0;}
		//Kiem tra candle cuoi
		if (timeIns == LastCandle->GetDataLink()->getTime()) {
			Candle obCandle;
			obCandle.SetCandle(sp[3], sp[4], sp[5], sp[6], sp[7]);
			LastCandle->SetDataLink(obCandle);
			return 1;
		}
		//
		while (FindingCandle != nullptr)
		{
			Candle *dataCandle = FindingCandle->GetDataLink();
			long timedataCandle = dataCandle->getTime();
			if (timeIns == timedataCandle) {
				break;
			}
			FindingCandle = FindingCandle->GetLinkNext();
		}
		if (FindingCandle == nullptr) {return 0;}
		else {
			Candle obCandle;
			obCandle.SetCandle(sp[3], sp[4], sp[5], sp[6], sp[7]);
			FindingCandle->SetDataLink(obCandle);
			return 1;
		}
	}
	else {return 0;}
}

int ProcessData::mb(const string *sp, const int n){
	if (!isSplit(sp, n)) {return -1;}
	if (n < 3 || n > 5) {return -1;}
	if (n == 4 && !isTime(sp[3])) {return -1;}
	if (n == 5 && (!isTime(sp[3]) || !isTime(sp[4]))) {return -1;}
	if (isTime(sp[n-2]) && isTime(sp[n-1]) && stol(sp[n-2]) > stol(sp[n-1])) {return -1;}
	Link<Exchange> *FindingExchange = FindExchange(sp[1], sp[2]);
	if (FindingExchange != nullptr){
		Link<Candle> *FindingCandle = FindingExchange->GetDataLink()->GetFirstCandle();
		Link<Candle> *FirstCandle = FindingExchange->GetDataLink()->GetFirstCandle();
		Link<Candle> *LastCandle = FindingExchange->GetDataLink()->GetLastCandle();
		int count = 0;
		if (n==5){
			long timeA = stol(sp[3]);
			long timeB = stol(sp[4]);
			if (timeB < FirstCandle->GetDataLink()->getTime() || timeA > LastCandle->GetDataLink()->getTime()) {return 0;}
			while (FindingCandle != nullptr){
				Candle *dataCandle = FindingCandle->GetDataLink();
				long timedataCandle = dataCandle->getTime();
				float OP = dataCandle->getOP();
				float HP = dataCandle->getHP();
				float LP = dataCandle->getLP();
				float CP = dataCandle->getCP();
				if (timeA <= timedataCandle && timedataCandle <= timeB) {
					if (IsMB(OP, HP, LP, CP)) {count++;}
				}
				if (timedataCandle > timeB) {
					break;
				}
				FindingCandle = FindingCandle->GetLinkNext();
			}
			return count;
		}
		else if (n==4){
			long timeA = stol(sp[3]);
			//Kiem tra candle cuoi
			if (timeA < FirstCandle->GetDataLink()->getTime() || timeA > LastCandle->GetDataLink()->getTime()) {return 0;}
			if (timeA == LastCandle->GetDataLink()->getTime()) {
				if (IsMB(LastCandle->GetDataLink()->getOP(), LastCandle->GetDataLink()->getHP(), LastCandle->GetDataLink()->getLP(), LastCandle->GetDataLink()->getCP())) {return 1;}
			}
			//
			while (FindingCandle != nullptr){
				Candle *dataCandle = FindingCandle->GetDataLink();
				long timedataCandle = dataCandle->getTime();
				float OP = dataCandle->getOP();
				float HP = dataCandle->getHP();
				float LP = dataCandle->getLP();
				float CP = dataCandle->getCP();
				if (timeA == timedataCandle) {
					if (IsMB(OP, HP, LP, CP)) {return 1;}
				}
				if (timedataCandle > timeA) {break;}
				FindingCandle = FindingCandle->GetLinkNext();
			}
			return 0;
		}
		else if (n==3){
			while (FindingCandle != nullptr){
				if (IsMB(FindingCandle->GetDataLink()->getOP(), FindingCandle->GetDataLink()->getHP(), FindingCandle->GetDataLink()->getLP(), FindingCandle->GetDataLink()->getCP())) {count++;}
				FindingCandle = FindingCandle->GetLinkNext();
			}
			return count;
		}
		else {return -1;}
	}
	else {return 0;}
}

int ProcessData::st(const string *sp, const int n){
	if (!isSplit(sp, n)) {return -1;}
	if (n < 3 || n > 5) {return -1;}
	if (n == 4 && !isTime(sp[3])) {return -1;}
	if (n == 5 && (!isTime(sp[3]) || !isTime(sp[4]))) {return -1;}
	if (isTime(sp[n-2]) && isTime(sp[n-1]) && stol(sp[n-2]) > stol(sp[n-1])) {return -1;}
	Link<Exchange> *FindingExchange = FindExchange(sp[1], sp[2]);
	if (FindingExchange != nullptr){
		Link<Candle> *FindingCandle = FindingExchange->GetDataLink()->GetFirstCandle();
		Link<Candle> *FirstCandle = FindingExchange->GetDataLink()->GetFirstCandle();
		Link<Candle> *LastCandle = FindingExchange->GetDataLink()->GetLastCandle();
		int count = 0;
		if (n==5){
			long timeA = stol(sp[3]);
			long timeB = stol(sp[4]);
			if (timeB < FirstCandle->GetDataLink()->getTime() || timeA > LastCandle->GetDataLink()->getTime()) {return 0;}
			while (FindingCandle != nullptr){
				Candle *dataCandle = FindingCandle->GetDataLink();
				long timedataCandle = dataCandle->getTime();
				float OP = dataCandle->getOP();
				float HP = dataCandle->getHP();
				float LP = dataCandle->getLP();
				float CP = dataCandle->getCP();
				if (timeA <= timedataCandle && timedataCandle <= timeB) {
					if (IsST(OP, HP, LP, CP)) {count++;}
				}
				if (timedataCandle > timeB) {
					break;
				}
				FindingCandle = FindingCandle->GetLinkNext();
			}
			return count;
		}
		else if (n==4){
			long timeA = stol(sp[3]);
			//Kiem tra candle cuoi
			if (timeA < FirstCandle->GetDataLink()->getTime() || timeA > LastCandle->GetDataLink()->getTime()) {return 0;}
			if (timeA == LastCandle->GetDataLink()->getTime()) {
				if (IsST(LastCandle->GetDataLink()->getOP(), LastCandle->GetDataLink()->getHP(), LastCandle->GetDataLink()->getLP(), LastCandle->GetDataLink()->getCP())) {return 1;}
			}
			//
			while (FindingCandle != nullptr){
				Candle *dataCandle = FindingCandle->GetDataLink();
				long timedataCandle = dataCandle->getTime();
				float OP = dataCandle->getOP();
				float HP = dataCandle->getHP();
				float LP = dataCandle->getLP();
				float CP = dataCandle->getCP();
				if (timeA == timedataCandle) {
					if (IsST(OP, HP, LP, CP)) {return 1;}
				}
				if (timedataCandle > timeA) {
					break;
				}
				FindingCandle = FindingCandle->GetLinkNext();
			}
			return 0;
		}
		else if (n==3){
			while (FindingCandle != nullptr){
				Candle *dataCandle = FindingCandle->GetDataLink();
				float OP = dataCandle->getOP();
				float HP = dataCandle->getHP();
				float LP = dataCandle->getLP();
				float CP = dataCandle->getCP();
				if (IsST(OP, HP, LP, CP)) {count++;}
				FindingCandle = FindingCandle->GetLinkNext();
			}
			return count;
		}
		else {return -1;}
	}
	else {return 0;}
}

int ProcessData::dj(const string *sp, const int n){
	if (!isSplit(sp, n)) {return -1;}
	if (n < 3 || n > 6) {return -1;}
	if (n == 4 && !(sp[3] == "C1" || sp[3] == "C2" || sp[3] == "C3" || sp[3] == "C4" || isTime(sp[3]))) {return -1;}
	if (n == 5 && !((sp[3] == "C1" || sp[3] == "C2" || sp[3] == "C3" || sp[3] == "C4" || isTime(sp[3])) && isTime(sp[4]))) {return -1;}
	if (n == 6 && !((sp[3] == "C1" || sp[3] == "C2" || sp[3] == "C3" || sp[3] == "C4") && isTime(sp[4]) && isTime(sp[5]))) {return -1;}
	if (isTime(sp[n-2]) && isTime(sp[n-1]) && stol(sp[n-2]) > stol(sp[n-1])) {return -1;}
	Link<Exchange> *FindingExchange = FindExchange(sp[1], sp[2]);
	if (FindingExchange != nullptr){
		Link<Candle> *FindingCandle = FindingExchange->GetDataLink()->GetFirstCandle();
		Link<Candle> *FirstCandle = FindingExchange->GetDataLink()->GetFirstCandle();
		Link<Candle> *LastCandle = FindingExchange->GetDataLink()->GetLastCandle();
		int count = 0;
		if (n == 6){
			long timeA = stol(sp[4]);
			long timeB = stol(sp[5]);
			if (timeB < FirstCandle->GetDataLink()->getTime() || timeA > LastCandle->GetDataLink()->getTime()) {return 0;}
			if (sp[3] == "C1"){
				while (FindingCandle != nullptr){
					Candle *dataCandle = FindingCandle->GetDataLink();
					long timedataCandle = dataCandle->getTime();
					float OP = dataCandle->getOP();
					float HP = dataCandle->getHP();
					float LP = dataCandle->getLP();
					float CP = dataCandle->getCP();
					if (timeA <= timedataCandle && timedataCandle <= timeB) {
						bool one = IsBD_C1(OP, HP, LP, CP);
						if (one) {count++;}
					}
					if (timedataCandle > timeB) {
						break;
					}
					FindingCandle = FindingCandle->GetLinkNext();
				}
				return count;
			}
			else if (sp[3] == "C2"){
				while (FindingCandle != nullptr){
					Candle *dataCandle = FindingCandle->GetDataLink();
					long timedataCandle = dataCandle->getTime();
					float OP = dataCandle->getOP();
					float HP = dataCandle->getHP();
					float LP = dataCandle->getLP();
					float CP = dataCandle->getCP();
					if (timeA <= timedataCandle && timedataCandle <= timeB) {
						bool two = IsCC_C2(OP, HP, LP, CP);
						if (two) {count++;}
					}
					if (timedataCandle > timeB) {
						break;
					}
					FindingCandle = FindingCandle->GetLinkNext();
				}
				return count;
			}
			else if (sp[3] == "C3"){
				while (FindingCandle != nullptr){
					Candle *dataCandle = FindingCandle->GetDataLink();
					long timedataCandle = dataCandle->getTime();
					float OP = dataCandle->getOP();
					float HP = dataCandle->getHP();
					float LP = dataCandle->getLP();
					float CP = dataCandle->getCP();
					if (timeA <= timedataCandle && timedataCandle <= timeB) {
						bool three = IsBM_C3(OP, HP, LP, CP);
						if (three) {count++;}
					}
					if (timedataCandle > timeB) {
						break;
					}
					FindingCandle = FindingCandle->GetLinkNext();
				}
				return count;
			}
			else if (sp[3] == "C4"){
				while (FindingCandle != nullptr){
					Candle *dataCandle = FindingCandle->GetDataLink();
					long timedataCandle = dataCandle->getTime();
					float OP = dataCandle->getOP();
					float HP = dataCandle->getHP();
					float LP = dataCandle->getLP();
					float CP = dataCandle->getCP();
					if (timeA <= timedataCandle && timedataCandle <= timeB) {
						bool four = IsBG_C4(OP, HP, LP, CP);
						if (four) {count++;}
					}
					if (timedataCandle > timeB) {
						break;
					}
					FindingCandle = FindingCandle->GetLinkNext();
				}
				return count;
			}
			else {return -1;}
		}
		else if (n == 5){
			long timeA = stol(sp[4]);
			//Kiem tra candle cuoi
			if ((timeA < FirstCandle->GetDataLink()->getTime() || timeA > LastCandle->GetDataLink()->getTime()) && (sp[3] == "C1" || sp[3] == "C2" || sp[3] == "C3" || sp[3] == "C4")) {return 0;}
			if (timeA == LastCandle->GetDataLink()->getTime()) {
				if (sp[3] == "C1"){
					if (IsBD_C1(LastCandle->GetDataLink()->getOP(), LastCandle->GetDataLink()->getHP(), LastCandle->GetDataLink()->getLP(), LastCandle->GetDataLink()->getCP())) {return 1;}
				}
				else if (sp[3] == "C2"){
					if (IsCC_C2(LastCandle->GetDataLink()->getOP(), LastCandle->GetDataLink()->getHP(), LastCandle->GetDataLink()->getLP(), LastCandle->GetDataLink()->getCP())) {return 1;}
				}
				else if (sp[3] == "C3"){
					if (IsBM_C3(LastCandle->GetDataLink()->getOP(), LastCandle->GetDataLink()->getHP(), LastCandle->GetDataLink()->getLP(), LastCandle->GetDataLink()->getCP())) {return 1;}
				}
				else if (sp[3] == "C4"){
					if (IsBG_C4(LastCandle->GetDataLink()->getOP(), LastCandle->GetDataLink()->getHP(), LastCandle->GetDataLink()->getLP(), LastCandle->GetDataLink()->getCP())) {return 1;}
				}
			}
			//
			if (sp[3] == "C1"){
				while (FindingCandle != nullptr){
					Candle *dataCandle = FindingCandle->GetDataLink();
					long timedataCandle = dataCandle->getTime();
					float OP = dataCandle->getOP();
					float HP = dataCandle->getHP();
					float LP = dataCandle->getLP();
					float CP = dataCandle->getCP();
					if (timeA == timedataCandle) {
						bool one = IsBD_C1(OP, HP, LP, CP);
						if (one) {return 1;}
					}
					if (timedataCandle > timeA) {
						break;
					}
					FindingCandle = FindingCandle->GetLinkNext();
				}
				return 0;
			}
			else if (sp[3] == "C2"){
				while (FindingCandle != nullptr){
					Candle *dataCandle = FindingCandle->GetDataLink();
					long timedataCandle = dataCandle->getTime();
					float OP = dataCandle->getOP();
					float HP = dataCandle->getHP();
					float LP = dataCandle->getLP();
					float CP = dataCandle->getCP();
					if (timeA == timedataCandle) {
						bool two = IsCC_C2(OP, HP, LP, CP);
						if (two) {return 1;}
					}
					if (timedataCandle > timeA) {
						break;
					}
					FindingCandle = FindingCandle->GetLinkNext();
				}
				return 0;
			}
			else if (sp[3] == "C3"){
				while (FindingCandle != nullptr){
					Candle *dataCandle = FindingCandle->GetDataLink();
					long timedataCandle = dataCandle->getTime();
					float OP = dataCandle->getOP();
					float HP = dataCandle->getHP();
					float LP = dataCandle->getLP();
					float CP = dataCandle->getCP();
					if (timeA == timedataCandle) {
						bool three = IsBM_C3(OP, HP, LP, CP);
						if (three) {return 1;}
					}
					if (timedataCandle > timeA) {
						break;
					}
					FindingCandle = FindingCandle->GetLinkNext();
				}
				return 0;
			}
			else if (sp[3] == "C4"){
				while (FindingCandle != nullptr){
					Candle *dataCandle = FindingCandle->GetDataLink();
					long timedataCandle = dataCandle->getTime();
					float OP = dataCandle->getOP();
					float HP = dataCandle->getHP();
					float LP = dataCandle->getLP();
					float CP = dataCandle->getCP();
					if (timeA == timedataCandle) {
						bool four = IsBG_C4(OP, HP, LP, CP);
						if (four) {return 1;}
					}
					if (timedataCandle > timeA) {
						break;
					}
					FindingCandle = FindingCandle->GetLinkNext();
				}
				return 0;
			}
			else {
				long timeA_ = stol(sp[3]);
				if (timeA < FirstCandle->GetDataLink()->getTime() || timeA_ > LastCandle->GetDataLink()->getTime()) {return 0;}
				while (FindingCandle != nullptr){
					Candle *dataCandle = FindingCandle->GetDataLink();
					long timedataCandle = dataCandle->getTime();
					if (timeA_ <= timedataCandle && timedataCandle <= timeA) {
						if (IsDJ(dataCandle->getOP(), dataCandle->getCP())) {count++;}
					}
					if (timedataCandle > timeA) {
						break;
					}
					FindingCandle = FindingCandle->GetLinkNext();
				}
				return count;
			}
		}
		else if (n == 4){
			if (sp[3] == "C1"){
				while (FindingCandle != nullptr){
					Candle *dataCandle = FindingCandle->GetDataLink();
					bool one = IsBD_C1(dataCandle->getOP(), dataCandle->getHP(), dataCandle->getLP(), dataCandle->getCP());
					if (one) {count++;}
					FindingCandle = FindingCandle->GetLinkNext();
				}
				return count;
			}
			else if (sp[3] == "C2"){
				while (FindingCandle != nullptr){
					Candle *dataCandle = FindingCandle->GetDataLink();
					bool two = IsCC_C2(dataCandle->getOP(), dataCandle->getHP(), dataCandle->getLP(), dataCandle->getCP());
					if (two) {count++;}
					FindingCandle = FindingCandle->GetLinkNext();
				}
				return count;
			}
			else if (sp[3] == "C3"){
				while (FindingCandle != nullptr){
					Candle *dataCandle = FindingCandle->GetDataLink();
					bool three = IsBM_C3(dataCandle->getOP(), dataCandle->getHP(), dataCandle->getLP(), dataCandle->getCP());
					if (three) {count++;}
					FindingCandle = FindingCandle->GetLinkNext();
				}
				return count;
			}
			else if (sp[3] == "C4"){
				while (FindingCandle != nullptr){
					Candle *dataCandle = FindingCandle->GetDataLink();
					bool four = IsBG_C4(dataCandle->getOP(), dataCandle->getHP(), dataCandle->getLP(), dataCandle->getCP());
					if (four) {count++;}
					FindingCandle = FindingCandle->GetLinkNext();
				}
				return count;
			}
			else {
				long timeA = stol(sp[3]);
				//Kiem tra candle cuoi
				if (timeA < FirstCandle->GetDataLink()->getTime() || timeA > LastCandle->GetDataLink()->getTime()) {return 0;}
				if (timeA == LastCandle->GetDataLink()->getTime()) {
					if (IsDJ(LastCandle->GetDataLink()->getOP(), LastCandle->GetDataLink()->getCP())) {return 1;}
				}
				//
				while (FindingCandle != nullptr){
					Candle *dataCandle = FindingCandle->GetDataLink();
					long timedataCandle = dataCandle->getTime();
					if (timeA == timedataCandle) {
						if (IsDJ(dataCandle->getOP(), dataCandle->getCP())) {return 1;}
					}
					if (timedataCandle > timeA) {
						break;
					}
					FindingCandle = FindingCandle->GetLinkNext();
				}
				return 0;
			}
		}
		else {
			while (FindingCandle != nullptr){
				Candle *dataCandle = FindingCandle->GetDataLink();
				if (IsDJ(dataCandle->getOP(), dataCandle->getCP())) {count++;}
				FindingCandle = FindingCandle->GetLinkNext();
			}
			return count;
		}
	}
	else {return 0;}
}

int ProcessData::eg(const string *sp, const int n){
	if (!isSplit(sp, n)) {return -1;}
	if (n < 3 || n > 6) {return -1;}
	if (n == 4 && !(sp[3] == "C1" || sp[3] == "C2" || isTime(sp[3]))) {return -1;}
	if (n == 5 && !((sp[3] == "C1" || sp[3] == "C2" || isTime(sp[3])) && isTime(sp[4]))) {return -1;}
	if (n == 6 && !((sp[3] == "C1" || sp[3] == "C2") && isTime(sp[4]) && isTime(sp[5]))) {return -1;}
	if (isTime(sp[n-2]) && isTime(sp[n-1]) && stol(sp[n-2]) > stol(sp[n-1])) {return -1;}
	Link<Exchange> *FindingExchange = FindExchange(sp[1], sp[2]);
	if (FindingExchange != nullptr){
		Link<Candle> *FindingCandle = FindingExchange->GetDataLink()->GetFirstCandle();
		Link<Candle> *FirstCandle = FindingExchange->GetDataLink()->GetFirstCandle();
		Link<Candle> *LastCandle = FindingExchange->GetDataLink()->GetLastCandle();
		Link<Candle> *PostCandle = FindingCandle->GetLinkNext();
		if (PostCandle == nullptr) {return 0;}
		int count = 0;
		while (PostCandle != nullptr){
			Candle *Pre = FindingCandle->GetDataLink();
			long timedataCandle = Pre->getTime();
			float OPPre = Pre->getOP();
			float CPPre = Pre->getCP();
			Candle *Pos = PostCandle->GetDataLink();
			float OPPos = Pos->getOP();
			float CPPos = Pos->getCP();
			if (n == 6){
				long timeA = stol(sp[4]);
				long timeB = stol(sp[5]);
				if (timeB < FirstCandle->GetDataLink()->getTime() || timeA > LastCandle->GetDataLink()->getTime()) {return 0;}
				if (sp[3] == "C1"){
					if (timeA <= timedataCandle && timedataCandle <= timeB) {
						bool one = IsNCT_C1(OPPre, CPPre, OPPos, CPPos);
						if (one) {count++;}
					}
					if (timedataCandle > timeB) {
						break;
					}
				}
				else if (sp[3] == "C2"){
					if (timeA <= timedataCandle && timedataCandle <= timeB) {
						bool two = IsNCG_C2(OPPre, CPPre, OPPos, CPPos);
						if (two) {count++;}
					}
					if (timedataCandle > timeB) {
						break;
					}
				}
				else {return -1;}
			}
			else if (n == 5){
				long timeA = stol(sp[4]);
				if ((timeA < FirstCandle->GetDataLink()->getTime() || timeA > LastCandle->GetDataLink()->getTime()) && (sp[3] == "C1" || sp[3] == "C2")) {
					return 0;}
				if (sp[3] == "C1"){
					if (timeA == timedataCandle) {
						bool one = IsNCT_C1(OPPre, CPPre, OPPos, CPPos);
						if (one) {count++;}
					}
					if (timedataCandle > timeA) {
						break;
					}
				}
				else if (sp[3] == "C2"){
					if (timeA == timedataCandle) {
						bool two = IsNCG_C2(OPPre, CPPre, OPPos, CPPos);
						if (two) {count++;}
					}
					if (timedataCandle > timeA) {
						break;
					}
				}
				else {
					long timeA_ = stol(sp[3]);
					if (timeA < FirstCandle->GetDataLink()->getTime() || timeA_ > LastCandle->GetDataLink()->getTime()) {
						return 0;}
					if (timeA_ <= timedataCandle && timedataCandle <= timeA) {
						bool one = IsNCT_C1(OPPre, CPPre, OPPos, CPPos);
						bool two = IsNCG_C2(OPPre, CPPre, OPPos, CPPos);
						if (one || two) {count++;}
					}
					if (timedataCandle > timeA) {
						break;
					}
				}
			}
			else if (n == 4){
				if (sp[3] == "C1"){
					bool one = IsNCT_C1(OPPre, CPPre, OPPos, CPPos);
					if (one) {count++;}
				}
				else if (sp[3] == "C2"){
					bool two = IsNCG_C2(OPPre, CPPre, OPPos, CPPos);
					if (two) {count++;}
				}
				else {
					long timeA = stol(sp[3]);
					if (timeA < FirstCandle->GetDataLink()->getTime() || timeA > LastCandle->GetDataLink()->getTime()) {return 0;}
					if (timeA == timedataCandle) {
						bool one = IsNCT_C1(OPPre, CPPre, OPPos, CPPos);
						bool two = IsNCG_C2(OPPre, CPPre, OPPos, CPPos);
						if (one || two) {return 1;}
					}
					if (timedataCandle > timeA) {
						break;
					}
				}
			}
			else if (n == 3){
				bool one = IsNCT_C1(OPPre, CPPre, OPPos, CPPos);
				bool two = IsNCG_C2(OPPre, CPPre, OPPos, CPPos);
				if (one || two) {count++;}
			}
			else {
				return -1;
			}
			FindingCandle = PostCandle;
			PostCandle = PostCandle->GetLinkNext();
		}
		return count;
	}
	else {
		return 0;
	}
}