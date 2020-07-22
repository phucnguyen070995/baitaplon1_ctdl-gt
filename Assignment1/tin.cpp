#include "processData.h"
// #define MIN(a, b) (((a) < (b)) ? (a) : (b))
// #define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define EPS 0.000001
float PIP = 0.0001;

enum CodeValue
{
	insCode,
	delCode,
	updCode,
	stCode,
	mbCode,
	djCode,
	egCode
};

static map<string, CodeValue> s_mapCodeValues = {
	{"INS", insCode},
	{"DEL", delCode},
	{"UPD", updCode},
	{"ST", stCode},
	{"MB", mbCode},
	{"DJ", djCode},
	{"EG", egCode}};

enum DojiCode
{
	C1,
	C2,
	C3,
	C4
};

static map<string, DojiCode> s_mapDoji = {
	{"C1", C1},
	{"C2", C2},
	{"C3", C3},
	{"C4", C4}};

// Check TIME có phải Integer hay ko
bool isInteger(string s)
{
	for (int i = 0; i < s.length(); i++)
		if (isdigit(s[i]) == false)
			return false; // nếu chứa kí tự ko phải là số thì ko được
	if (stof(s) < 0)
	{
		return false; // nếu là số âm thì ko được
	}
	return true;
}

// Check OP, CP, HP, LP có phải float hay ko
bool isFloat(string s)
{
	// Đếm số lượng floating point
	int count_floating_point = 0;
	for (int i = 0; i < s.length(); i++)
	{
		// Không phải number (0-9), cũng ko phải '.'
		if (s[i] == '.')
		{
			count_floating_point++;
			if (count_floating_point > 1)
			{
				return false;
			}
		}
		if (isdigit(s[i]) == false && s[i] != '.')
		{
			return false;
		}
	}

	if (stof(s) < 0)
	{
		return false; // nếu là số âm thì ko được
	}

	return true;
}

ProcessData::ProcessData()
{
	data->head = nullptr;
}
ProcessData::~ProcessData()
{
	Node<Exchange> *ptr1; // ptr1 Dùng để traverse outer LinkedList
	ptr1 = data->head;

	while (ptr1 != nullptr)
	{
		Node<Candle> *ptr2; // ptr2 Dùng để traverse inner LinkedList
		ptr2 = ptr1->data.details->head;

	repeat4:

		// Nếu cặp tiền tệ này ko còn gì để xóa
		if (ptr2 == nullptr)
		{
			//cout << "Deleted Exchange: " << ptr1->data.BC << " " << ptr1->data.QC << "\n";
			Node<Exchange> *temp1 = ptr1; // ptr1 Dùng để traverse outer LinkedList
			data->head = ptr1->link;
			ptr1 = data->head;
			delete temp1->data.details;
			delete temp1;

			continue;
		}

		//cout << "Delete Candle: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << "\n\n";
		Node<Candle> *temp2 = ptr2;
		ptr1->data.details->head = ptr2->link;
		ptr2 = ptr2->link;
		ptr1->data.details->count--; // Giảm số lượng nến xuống 1
		delete temp2;
		goto repeat4;
	}

	//cout << "Deleted ProcessData\n";
	delete data;
}
/*

*/
int ProcessData::split(string line, string *&sp)
{
	sp = new string[MAXSIZECODE];
	const string SEP = " ";
	int pos, lastpos = 0, idx = 0;
	while (line.find(SEP, lastpos) != -1 && idx < 7)
	{
		pos = line.find(SEP, lastpos);
		sp[idx] = line.substr(lastpos, pos - lastpos);
		idx++;
		lastpos = pos + 1;
		//cout << "CMD: " << sp[idx-1] <<" Found a space character at: " << pos  << " " << lastpos << endl;
	}
	sp[idx] = line.substr(lastpos);
	//cout << "CMD: " << sp[idx] <<" Found a space character at: " << pos  << " " << lastpos << endl;
	return idx;
}

int ProcessData::process(string line)
{
	string *p;
	int n = ProcessData::split(line, p);
	for (int i = 0; i <= n; i++)
	{
		//cout << p[i] << "|";
		if (p[i] == "")
		{
			delete[] p;
			return -1;
		}
	}
	//cout << n << endl;
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
			//printAllCandle();
			break;
		case updCode:
			res = updateCandle(p, n);
			break;
		case delCode:
			res = deleteCandle(p, n);
			break;
		case stCode:
			res = spinningTop(p, n);
			break;
		case mbCode:
			res = marubozu(p, n);
			break;
		case djCode:
			res = doji(p, n);
			break;
		case egCode:
			res = engulfing(p, n);
			break;
		default:
			res = -1;
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

// INSERT Candle into ProcessData
int ProcessData::insert(const string *sp, const int n)
{
	//cout << "Number of agruments: " << n << "\n\n";
	if (n != 7)
	{
		return -1; //Khong du so luong arguments = 8
	}

	// Tạo một nến mới
	Node<Candle> *temp = new Node<Candle>;

	// Check TIME và OP, CP, HP, LP có đúng định dạng ko
	if (isInteger(sp[3]) && isFloat(sp[4]) && isFloat(sp[5]) && isFloat(sp[6]) && isFloat(sp[7]))
	{
		temp->data.TIME = stol(sp[3]);
		temp->data.OP = stof(sp[4]);
		temp->data.HP = stof(sp[5]);
		temp->data.LP = stof(sp[6]);
		temp->data.CP = stof(sp[7]);
		if (temp->data.HP < temp->data.OP || temp->data.HP < temp->data.CP || temp->data.HP < temp->data.LP || temp->data.LP > temp->data.OP || temp->data.LP > temp->data.CP)
		{
			delete temp;
			return -1;
		}
	}
	else
	{
		delete temp;
		return -1;
	}

	Node<Exchange> *ptr1; // ptr1 Dùng để traverse outer LinkedList
	ptr1 = data->head;

	while (ptr1 != nullptr)
	{
		// Nếu đã có cặp tiền tệ trong outer LinkedList
		if (ptr1->data.BC == sp[1] && ptr1->data.QC == sp[2])
		{

			Node<Candle> *ptr2; // ptr2 Dùng để traverse inner LinkedList
			ptr2 = ptr1->data.details->head;

			// Nếu cặp tiền tệ ko có nến nào => chèn vào head
			if (ptr2 == nullptr)
			{
				ptr1->data.details->head = temp;
				ptr1->data.details->count++;	  //Biến đếm số lượng nến của mỗi cặp tiền tệ
				return ptr1->data.details->count; // Trả về số lượng nến sau khi thêm
			}
			//Nếu TIME của nến muốn thêm nhỏ hơn TIME của head ==> chèn nến vào head
			else if (temp->data.TIME < ptr2->data.TIME)
			{
				//Add Nến vào đầu của LinkedList
				temp->link = ptr1->data.details->head;
				ptr1->data.details->head = temp;
				ptr1->data.details->count++;	  //Biến đếm số lượng nến của mỗi cặp tiền tệ
				return ptr1->data.details->count; // Trả về số lượng nến sau khi thêm
			}

			while (ptr2 != nullptr)
			{
				// Nếu nến muốn thêm bị trùng thời gian với nến có sẵn
				if (ptr2->data.TIME == temp->data.TIME)
				{
					delete temp;					  // Free temp memory
					return ptr1->data.details->count; // Nến muốn thêm vào bị trùng TIME
				}
				// Chèn nến vào cuối LinkedList
				else if (temp->data.TIME > ptr2->data.TIME && ptr2->link == nullptr)
				{
					temp->link = ptr2->link;
					ptr2->link = temp;
					ptr1->data.details->count++;	  //Biến đếm số lượng nến của mỗi cặp tiền tệ
					return ptr1->data.details->count; // Trả về số lượng nến sau khi thêm
				}
				// Chèn nến vào giữa LinkedList
				else if (temp->data.TIME > ptr2->data.TIME && temp->data.TIME < ptr2->link->data.TIME)
				{
					temp->link = ptr2->link;
					ptr2->link = temp;
					ptr1->data.details->count++;	  //Biến đếm số lượng nến của mỗi cặp tiền tệ
					return ptr1->data.details->count; // Trả về số lượng nến sau khi thêm
				}

				ptr2 = ptr2->link;
			}
			break;
		}
		ptr1 = ptr1->link;
	}

	//Nếu chưa tồn tại cặp tiền tệ thì chèn thêm cặp tiền tệ vào đầu chuỗi
	Node<Exchange> *temp2 = new Node<Exchange>;
	temp2->data.BC = sp[1];
	temp2->data.QC = sp[2];
	temp2->data.details = new LinkedList<Candle>();
	temp2->link = data->head;
	data->head = temp2;
	data->count++; // Biến đếm số lượng cặp tiền tệ

	// Add nến vào head cặp tiền tệ vừa tạo
	temp->link = data->head->data.details->head;
	data->head->data.details->head = temp;
	data->head->data.details->count++;
	return data->head->data.details->count;
}

// Show All Candles
// void ProcessData::printAllCandle()
// {
// 	Node<Exchange> *ptr1; // ptr1 Dùng để traverse outer LinkedList
// 	ptr1 = data->head;

// 	while (ptr1 != nullptr)
// 	{
// 		Node<Candle> *ptr2; // ptr2 Dùng để traverse inner LinkedList
// 		ptr2 = ptr1->data.details->head;

// 		while (ptr2 != nullptr)
// 		{
// 			//cout << "Candle: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << endl;
// 			ptr2 = ptr2->link;
// 		}
// 		ptr1 = ptr1->link;
// 	}
// 	//cout << '\n';
// }

int ProcessData::deleteCandle(const string *sp, const int n)
{
	// Số lượng arguments nhỏ hơn 3 hoặc lớn hơn 5
	//cout << "Number of agruments: " << n << "\n\n";
	if (n < 2 || n > 4)
	{
		return -1; // Không có nến nào bị xóa
	}

	// Số nến bị xóa
	int deleted_candles = 0;

	// TIME_A <= TIME <= TIME_B
	if (n == 4)
	{
		Node<Exchange> *ptr1; // ptr1 Dùng để traverse outer LinkedList
		ptr1 = data->head;

		// Nếu TIME ko phải Integer
		if (!isInteger(sp[3]) || !isInteger(sp[4]))
		{
			return -1;
		}
		if (stol(sp[3]) > stol(sp[4]))
		{
			return -1;
		}

		while (ptr1 != nullptr)
		{

			// Tìm cặp tiền tệ cần xóa
			if (ptr1->data.BC == sp[1] && ptr1->data.QC == sp[2])
			{
				Node<Candle> *ptr2; // ptr2 Dùng để traverse inner LinkedList
				ptr2 = ptr1->data.details->head;

			repeat1:

				// Nếu cặp tiền tệ này ko còn gì để xóa
				if (ptr2 == nullptr)
				{
					return deleted_candles;
				}

				// Nếu nến cần xóa nằm ở head
				if (ptr2->data.TIME >= stol(sp[3]) && ptr2->data.TIME <= stol(sp[4]))
				{
					//cout << "Delete Candle: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << "\n\n";
					Node<Candle> *temp = ptr2;
					ptr1->data.details->head = ptr2 = ptr2->link;
					ptr1->data.details->count--; // Giảm số lượng nến xuống 1
					delete temp;

					deleted_candles += 1;
					goto repeat1;
				}

			repeat2:
				// Nếu nến cần xóa ko nằm ở head
				while (ptr2->link != nullptr)
				{
					// Tìm nến có thời gian thỏa mãn TIME_A <= TIME <= TIME_B
					if (ptr2->link->data.TIME >= stol(sp[3]) && ptr2->link->data.TIME <= stol(sp[4]))
					{
						//cout << "Delete Candle: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->link->data.TIME << " " << ptr2->data.OP << " " << ptr2->link->data.HP << " " << ptr2->link->data.LP << " " << ptr2->link->data.CP << "\n\n";
						Node<Candle> *temp = ptr2->link;
						ptr2->link = ptr2->link->link;
						ptr1->data.details->count--; // Giảm số lượng nến xuống 1
						delete temp;

						deleted_candles += 1;
						goto repeat2;
					}
					ptr2 = ptr2->link;
				}
			}
			ptr1 = ptr1->link;
		}
	}
	// TIME = TIME_A
	else if (n == 3)
	{
		Node<Exchange> *ptr1; // ptr1 Dùng để traverse outer LinkedList
		ptr1 = data->head;

		// Nếu TIME ko phải Integer
		if (!isInteger(sp[3]))
		{
			return -1;
		}

		while (ptr1 != nullptr)
		{

			// Tìm cặp tiền tệ cần xóa
			if (ptr1->data.BC == sp[1] && ptr1->data.QC == sp[2])
			{

				Node<Candle> *ptr2; // ptr2 Dùng để traverse inner LinkedList
				ptr2 = ptr1->data.details->head;

				// Nếu cặp tiền tệ này ko còn gì để xóa
				if (ptr2 == nullptr)
				{
					return 0;
				}

				// Nếu nến cần xóa nằm ở head
				if (ptr2->data.TIME == stol(sp[3]))
				{
					//cout << "Delete Candle: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << "\n\n";
					Node<Candle> *temp = ptr2;
					ptr1->data.details->head = ptr2 = ptr2->link;
					ptr1->data.details->count--; // Giảm số lượng nến xuống 1
					delete temp;

					return 1;
				}

				// Nếu nến cần xóa ko nằm ở head
				while (ptr2->link != nullptr)
				{
					if (ptr2->link->data.TIME == stol(sp[3]))
					{
						//cout << "Delete Candle: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->link->data.TIME << " " << ptr2->data.OP << " " << ptr2->link->data.HP << " " << ptr2->link->data.LP << " " << ptr2->link->data.CP << "\n\n";
						Node<Candle> *temp = ptr2->link;
						ptr2->link = ptr2->link->link;
						ptr1->data.details->count--; // Giảm số lượng nến xuống 1
						delete temp;
						return 1;
					}
					ptr2 = ptr2->link;
				}
			}
			ptr1 = ptr1->link;
		}
	}
	// Xóa hết nến của cặp tiền tệ
	else if (n == 2)
	{

		Node<Exchange> *ptr1; // ptr1 Dùng để traverse outer LinkedList
		ptr1 = data->head;

		while (ptr1 != nullptr)
		{
			if (ptr1->data.BC == sp[1] && ptr1->data.QC == sp[2])
			{

				Node<Candle> *ptr2; // ptr2 Dùng để traverse inner LinkedList
				ptr2 = ptr1->data.details->head;

			repeat3:

				// Nếu cặp tiền tệ này ko còn gì để xóa
				if (ptr2 == nullptr)
				{
					return deleted_candles;
				}

				//cout << "Delete Candle: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << "\n\n";
				Node<Candle> *temp = ptr2;
				ptr1->data.details->head = ptr2 = ptr2->link;
				ptr1->data.details->count--; // Giảm số lượng nến xuống 1
				delete temp;

				deleted_candles += 1;
				goto repeat3;
			}
			ptr1 = ptr1->link;
		}
	}

	//cout << "Deleted: " << deleted_candles << '\n';
	return deleted_candles;
}

int ProcessData::updateCandle(const string *sp, const int n)
{
	if (n != 7)
	{
		return -1; //Khong du so luong arguments = 8
	}

	// Check TIME và OP, CP, HP, LP có đúng định dạng ko
	if (!isInteger(sp[3]) || !isFloat(sp[4]) || !isFloat(sp[5]) || !isFloat(sp[6]) || !isFloat(sp[7]))
	{
		return -1;
	}

	if (stof(sp[5]) < stof(sp[4]) || stof(sp[5]) < stof(sp[7]) || stof(sp[5]) < stof(sp[6]) || stof(sp[6]) > stof(sp[4]) || stof(sp[6]) > stof(sp[7]))
	{
		return -1;
	}

	Node<Exchange> *ptr1; // ptr1 Dùng để traverse outer LinkedList
	ptr1 = data->head;

	while (ptr1 != nullptr)
	{

		if (ptr1->data.BC == sp[1] && ptr1->data.QC == sp[2])
		{

			Node<Candle> *ptr2; // ptr2 Dùng để traverse inner LinkedList
			ptr2 = ptr1->data.details->head;

			while (ptr2 != nullptr)
			{
				if (ptr2->data.TIME == stol(sp[3]))
				{
					//cout << "Update Candle: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << "\n\n";
					// delete node cần update
					const string sp2[4] = {"DEL", sp[1], sp[2], sp[3]};

					if (sp[5] < sp[4] || sp[5] < sp[7] || sp[5] < sp[6] || sp[6] > sp[4] || sp[6] > sp[7])
					{
						return -1;
					}
					else
					{
						deleteCandle(sp2, 3);
					}

					// Insert node mới có nội dung trong UPD
					const string sp3[] = {"INS", sp[1], sp[2], sp[3], sp[4], sp[5], sp[6], sp[7]};
					int res = insert(sp3, 7);

					// Return kết quả
					if (res == -1)
					{
						return -1;
					}
					else
					{
						return 1;
					}
				}
				ptr2 = ptr2->link;
			}
		}
		ptr1 = ptr1->link;
	}
	return 0;
};

int ProcessData::spinningTop(const string *sp, const int n)
{
	if (n < 2 || n > 4)
	{
		return -1;
	}

	long TIME_A;
	long TIME_B;

	if (n == 4)
	{
		if (isInteger(sp[3]) && isInteger(sp[4]))
		{
			TIME_A = stol(sp[3]);
			TIME_B = stol(sp[4]);

			if (TIME_A > TIME_B)
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}
	else if (n == 3)
	{
		if (isInteger(sp[3]))
		{
			TIME_A = TIME_B = stol(sp[3]);
		}
		else
		{
			return -1;
		}
	}
	else if (n == 2)
	{
		TIME_A = 0;
		TIME_B = 2147483647;
	}

	int count_ST = 0;	  // Biến đếm số lượng nến ST
	Node<Exchange> *ptr1; // ptr1 Dùng để traverse outer LinkedList
	ptr1 = data->head;

	while (ptr1 != nullptr)
	{

		if (ptr1->data.BC == sp[1] && ptr1->data.QC == sp[2])
		{

			Node<Candle> *ptr2; // ptr2 Dùng để traverse inner LinkedList
			ptr2 = ptr1->data.details->head;

			while (ptr2 != nullptr)
			{
				if (ptr2->data.TIME >= TIME_A && ptr2->data.TIME <= TIME_B)
				{
					// Check xem có phải nến Spinning Top lên
					// if ((ptr2->data.HP - ptr2->data.CP > 0.00005) && (ptr2->data.CP - ptr2->data.OP >= 0) && (ptr2->data.CP - ptr2->data.OP <= 0.00005) && (ptr2->data.OP - ptr2->data.LP > 0.00005))
					// OP > CP
					if (((ptr2->data.HP - ptr2->data.OP - 0.5 * PIP) > EPS && (ptr2->data.CP - ptr2->data.LP - 0.5 * PIP) > EPS && (abs(ptr2->data.OP - ptr2->data.CP) < EPS || (ptr2->data.OP - ptr2->data.CP) > EPS) && ((ptr2->data.OP - ptr2->data.CP - 0.5 * PIP) < EPS || abs(ptr2->data.OP - ptr2->data.CP - 0.5 * PIP) < EPS)) ||
						((ptr2->data.HP - ptr2->data.CP - 0.5 * PIP) > EPS && (ptr2->data.OP - ptr2->data.LP - 0.5 * PIP) > EPS && (abs(ptr2->data.CP - ptr2->data.OP) < EPS || (ptr2->data.CP - ptr2->data.OP) > EPS) && ((ptr2->data.CP - ptr2->data.OP - 0.5 * PIP) < EPS || abs(ptr2->data.CP - ptr2->data.OP - 0.5 * PIP) < EPS)))
					{
						count_ST++;
						//cout << "ST Candle: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << "\n\n";
						//cout << (abs(ptr2->data.CP - ptr2->data.OP) <= 0.5 * PIP) << "\n";
					}
					// Check xem có phải nến Spinning Top xuống
					// else if ((ptr2->data.HP - ptr2->data.OP > 0.00005) && (ptr2->data.OP - ptr2->data.CP <= 0.00005) && (ptr2->data.OP - ptr2->data.CP >= 0) && (ptr2->data.CP - ptr2->data.LP > 0.00005))
					// {
					// 	count_ST++;
					// 	cout << "ST Candle: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << "\n\n";
					// }
				}
				ptr2 = ptr2->link;
			}
			break;
		}
		ptr1 = ptr1->link;
	}
	return count_ST;
};

int ProcessData::marubozu(const string *sp, const int n)
{
	if (n < 2 || n > 4)
	{
		return -1;
	}

	long TIME_A;
	long TIME_B;

	if (n == 4)
	{
		if (isInteger(sp[3]) && isInteger(sp[4]))
		{
			TIME_A = stol(sp[3]);
			TIME_B = stol(sp[4]);
			if (TIME_A > TIME_B)
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}
	else if (n == 3)
	{
		if (isInteger(sp[3]))
		{
			TIME_A = TIME_B = stol(sp[3]);
		}
		else
		{
			return -1;
		}
	}
	else if (n == 2)
	{
		TIME_A = 0;
		TIME_B = 2147483647;
	}

	int count_MB = 0;	  // Biến đếm số lượng nến ST
	Node<Exchange> *ptr1; // ptr1 Dùng để traverse outer LinkedList
	ptr1 = data->head;

	while (ptr1 != nullptr)
	{

		if (ptr1->data.BC == sp[1] && ptr1->data.QC == sp[2])
		{

			Node<Candle> *ptr2; // ptr2 Dùng để traverse inner LinkedList
			ptr2 = ptr1->data.details->head;

			while (ptr2 != nullptr)
			{
				if (ptr2->data.TIME >= TIME_A && ptr2->data.TIME <= TIME_B)
				{
					if (((abs(ptr2->data.OP - ptr2->data.LP) < EPS) && (abs(ptr2->data.CP - ptr2->data.HP) < EPS) && ((abs(ptr2->data.OP - ptr2->data.CP) - 0.5 * PIP) > EPS)) || ((abs(ptr2->data.OP - ptr2->data.HP) < EPS) && (abs(ptr2->data.CP - ptr2->data.LP) < EPS) && ((abs(ptr2->data.OP - ptr2->data.CP) - 0.5 * PIP) > EPS)))
					{
						count_MB++;
						// cout << "MB Candle: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << "\n\n";
					}
				}
				ptr2 = ptr2->link;
			}
			break;
		}
		ptr1 = ptr1->link;
	}
	return count_MB;
};

int ProcessData::doji(const string *sp, const int n)
{
	if (n < 2 || n > 5)
	{
		return -1;
	}

	long TIME_A;
	long TIME_B;

	// Truong hop ko cung cap CODE
	if (n == 2 || !(sp[3] == "C1" || sp[3] == "C2" || sp[3] == "C3" || sp[3] == "C4"))
	{
		if (n == 2)
		{
			TIME_A = 0;
			TIME_B = 2147483647;
		}
		else if (n == 3)
		{
			if (isInteger(sp[3]))
			{
				TIME_A = TIME_B = stol(sp[3]);
			}
			else
			{
				return -1;
			}
		}
		else if (n == 4)
		{
			if (isInteger(sp[3]) && isInteger(sp[4]))
			{
				TIME_A = stol(sp[3]);
				TIME_B = stol(sp[4]);
				if (TIME_A > TIME_B)
				{
					return -1;
				}
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}

		int count_DJ = 0;	  // Biến đếm số lượng nến ST
		Node<Exchange> *ptr1; // ptr1 Dùng để traverse outer LinkedList
		ptr1 = data->head;

		while (ptr1 != nullptr)
		{

			if (ptr1->data.BC == sp[1] && ptr1->data.QC == sp[2])
			{

				Node<Candle> *ptr2; // ptr2 Dùng để traverse inner LinkedList
				ptr2 = ptr1->data.details->head;

				while (ptr2 != nullptr)
				{
					if (ptr2->data.TIME >= TIME_A && ptr2->data.TIME <= TIME_B)
					{
						// if (abs(ptr2->data.OP - ptr2->data.CP) <= 0.2 * PIP)
						if ((abs(ptr2->data.OP - ptr2->data.CP) - 0.2 * PIP) < EPS)
						{
							count_DJ++;
							//cout << "DJ Candle: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << "\n\n";
						}
					}
					ptr2 = ptr2->link;
				}
				break;
			}
			ptr1 = ptr1->link;
		}

		// Return số lượng nến DOJI đếm được
		return count_DJ;
	}

	// Truong hop co cung cap CODE
	else
	{

		if (!(sp[3] == "C1" || sp[3] == "C2" || sp[3] == "C3" || sp[3] == "C4"))
		{
			return -1; //CODE không đúng định dạng
		}

		if (n == 3)
		{
			TIME_A = 0;
			TIME_B = 2147483647;
		}
		else if (n == 4)
		{
			if (isInteger(sp[4]))
			{
				TIME_A = TIME_B = stol(sp[4]);
				if (TIME_A > TIME_B)
				{
					return -1;
				}
			}
			else
			{
				return -1;
			}
		}
		else if (n == 5)
		{
			if (isInteger(sp[4]) && isInteger(sp[5]))
			{
				TIME_A = stol(sp[4]);
				TIME_B = stol(sp[5]);
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}

		int count_DJ = 0;	  // Biến đếm số lượng nến ST
		Node<Exchange> *ptr1; // ptr1 Dùng để traverse outer LinkedList
		ptr1 = data->head;

		while (ptr1 != nullptr)
		{

			if (ptr1->data.BC == sp[1] && ptr1->data.QC == sp[2])
			{

				Node<Candle> *ptr2; // ptr2 Dùng để traverse inner LinkedList
				ptr2 = ptr1->data.details->head;

				while (ptr2 != nullptr)
				{

					//Match thoi gian TIME_A TIME_B
					if (ptr2->data.TIME >= TIME_A && ptr2->data.TIME <= TIME_B)
					{
						// Nếu là nến DOJI
						if ((abs(ptr2->data.OP - ptr2->data.CP) - 0.2 * PIP) < EPS)
						{
							switch (s_mapDoji[sp[3]])
							{
							case C1:
								if (((ptr2->data.HP - ptr2->data.OP) - 0.5 * PIP) > EPS && ((ptr2->data.HP - ptr2->data.CP) - 0.5 * PIP) > EPS &&
									((ptr2->data.OP - ptr2->data.LP) - 0.5 * PIP) > EPS && ((ptr2->data.CP - ptr2->data.LP) - 0.5 * PIP) > EPS)
								{
									count_DJ++;
									//cout << "DJ C1: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << "\n\n";
								}
								break;
							case C2:
								if ((abs(ptr2->data.HP - ptr2->data.OP) < EPS && (ptr2->data.CP - ptr2->data.LP - 0.5 * PIP) > EPS) ||
									(((ptr2->data.OP - ptr2->data.LP - 0.5 * PIP) > EPS) && (abs(ptr2->data.HP - ptr2->data.CP) < EPS)))
								{
									count_DJ++;
									//cout << "DJ Candle: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << "\n\n";
								}
								break;
							case C3:
								// if ((ptr2->data.LP - min(ptr2->data.CP, ptr2->data.OP)) < EPS && ((ptr2->data.HP - min(ptr2->data.CP, ptr2->data.OP) - 0.5 * PIP)) > EPS)
								if ((abs(ptr2->data.LP - ptr2->data.OP) < EPS && (ptr2->data.HP - ptr2->data.CP - 0.5 * PIP) > EPS) ||
									(((ptr2->data.HP - ptr2->data.OP - 0.5 * PIP) > EPS) && (abs(ptr2->data.LP - ptr2->data.CP) < EPS)))
								{
									count_DJ++;
									//cout << "DJ Candle: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << "\n\n";
								}
								break;
							case C4:
								if (abs(ptr2->data.OP - ptr2->data.CP) < EPS && abs(ptr2->data.OP - ptr2->data.HP) < EPS && abs(ptr2->data.OP - ptr2->data.LP) < EPS)
								{
									count_DJ++;
									//cout << "DJ Candle: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << "\n\n";
								}
								break;
							default:
								break;
							}
						}
					}
					ptr2 = ptr2->link;
				}
				break;
			}
			ptr1 = ptr1->link;
		}

		// Return số lượng nến DOJI đếm được
		return count_DJ;
	}
}

int ProcessData::engulfing(const string *sp, const int n)
{
	if (n < 2 || n > 5)
	{
		return -1;
	}

	long TIME_A;
	long TIME_B;

	// Truong hop ko cung cap CODE
	if (n == 2 || !(sp[3] == "C1" || sp[3] == "C2"))
	{
		if (n == 2)
		{
			TIME_A = 0;
			TIME_B = 2147483647;
		}
		else if (n == 3)
		{
			if (isInteger(sp[3]))
			{
				TIME_A = TIME_B = stol(sp[3]);
			}
			else
			{
				return -1;
			}
		}
		else if (n == 4)
		{
			if (isInteger(sp[3]) && isInteger(sp[4]))
			{
				TIME_A = stol(sp[3]);
				TIME_B = stol(sp[4]);
				if (TIME_A > TIME_B)
				{
					return -1;
				}
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}

		int count_EG = 0;	  // Biến đếm số lượng nến EG
		Node<Exchange> *ptr1; // ptr1 Dùng để traverse outer LinkedList
		ptr1 = data->head;

		while (ptr1 != nullptr)
		{

			if (ptr1->data.BC == sp[1] && ptr1->data.QC == sp[2])
			{

				Node<Candle> *ptr2; // ptr2 Dùng để traverse inner LinkedList
				ptr2 = ptr1->data.details->head;

				while (ptr2 != nullptr && ptr2->link != nullptr)
				{
					if (ptr2->data.TIME >= TIME_A && ptr2->data.TIME <= TIME_B)
					{
						// Nến nhấn chìm tăng (nến giảm sau đó là nến tăng)
						if ((((ptr2->data.OP > ptr2->data.CP) || abs(ptr2->data.OP - ptr2->data.CP) < EPS) && (ptr2->link->data.OP < ptr2->link->data.CP) && (ptr2->data.CP > ptr2->link->data.OP && ptr2->data.OP < ptr2->link->data.CP)) ||
							(((ptr2->data.OP < ptr2->data.CP) || abs(ptr2->data.OP - ptr2->data.CP) < EPS) && (ptr2->link->data.OP > ptr2->link->data.CP) && (ptr2->data.CP < ptr2->link->data.OP && ptr2->data.OP > ptr2->link->data.CP)))
						{
							count_EG++;
							//cout << "EG Candle: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << "\n\n";
						}
					}
					ptr2 = ptr2->link;
				}
				break;
			}
			ptr1 = ptr1->link;
		}

		// Return số lượng nến EG đếm được
		return count_EG;
	}

	// Truong hop co cung cap CODE
	else
	{

		if (!(sp[3] == "C1" || sp[3] == "C2"))
		{
			return -1; //CODE không đúng định dạng
		}

		if (n == 3)
		{
			TIME_A = 0;
			TIME_B = 2147483647;
		}
		else if (n == 4)
		{
			if (isInteger(sp[4]))
			{
				TIME_A = TIME_B = stol(sp[4]);
			}
			else
			{
				return -1;
			}
		}
		else if (n == 5)
		{
			if (isInteger(sp[4]) && isInteger(sp[5]))
			{
				TIME_A = stol(sp[4]);
				TIME_B = stol(sp[5]);
				if (TIME_A > TIME_B)
				{
					return -1;
				}
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}

		int count_EG = 0;	  // Biến đếm số lượng nến EG
		Node<Exchange> *ptr1; // ptr1 Dùng để traverse outer LinkedList
		ptr1 = data->head;

		while (ptr1 != nullptr)
		{

			if (ptr1->data.BC == sp[1] && ptr1->data.QC == sp[2])
			{

				Node<Candle> *ptr2; // ptr2 Dùng để traverse inner LinkedList
				ptr2 = ptr1->data.details->head;

				while (ptr2 != nullptr && ptr2->link != nullptr)
				{

					//Match thoi gian TIME_A TIME_B
					if (ptr2->data.TIME >= TIME_A && ptr2->data.TIME <= TIME_B)
					{
						// Nếu là nến DOJI

						switch (s_mapDoji[sp[3]])
						{
						case C1:
							if (((ptr2->data.OP > ptr2->data.CP) || abs(ptr2->data.OP - ptr2->data.CP) < EPS) && (ptr2->link->data.OP < ptr2->link->data.CP) && (ptr2->data.CP > ptr2->link->data.OP && ptr2->data.OP < ptr2->link->data.CP))
							{
								count_EG++;
								//cout << "EG C1: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << "\n\n";
							}
							break;
						case C2:
							if (((ptr2->data.OP < ptr2->data.CP) || abs(ptr2->data.OP - ptr2->data.CP) < EPS) && (ptr2->link->data.OP > ptr2->link->data.CP) && (ptr2->data.CP < ptr2->link->data.OP && ptr2->data.OP > ptr2->link->data.CP))
							{
								count_EG++;
								//cout << "EG C2: " << ptr1->data.BC << " " << ptr1->data.QC << " " << ptr2->data.TIME << " " << ptr2->data.OP << " " << ptr2->data.HP << " " << ptr2->data.LP << " " << ptr2->data.CP << "\n\n";
							}
							break;
						default:
							break;
						}
					}
					ptr2 = ptr2->link;
				}
				break;
			}
			ptr1 = ptr1->link;
		}

		// Return số lượng nến EG đếm được
		return count_EG;
	}
}