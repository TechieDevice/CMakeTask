#include "CppClient.cpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <signal.h>

static volatile int keepRunning = 1;

void getDataString(char** p_data_string)
{
	*p_data_string = new char[MAX_LENGTH + 1];

	printf("getting data\n");

	char* data = *p_data_string;
	int len = 0;
	while (true)
	{
		*data = getchar();
		if (*data == '\n')
		{
			*data = '\0';
			return;
		}
		if (!isdigit(*data))
		{
			printf("error, only numbers\n");
			*p_data_string = (char*)("err");
			return;
		}
		data++;
		len++;
		if (len == MAX_LENGTH)
		{
			*data = '\0';

			return;
		}
	}
}

void quicksort(char* &p_data_string, int first, int last)
{
	int mid, count;
	int f = first, l = last;
	mid = (int)p_data_string[(f + l) / 2];
	do
	{
		while ((int)p_data_string[f] > mid) f++;
		while ((int)p_data_string[l] < mid) l--;
		if (f <= l)
		{
			count = (int)p_data_string[f];
			p_data_string[f] = p_data_string[l];
			p_data_string[l] = (char)count;
			f++;
			l--;
		}
	} while (f < l);
	if (first < l) quicksort(p_data_string, first, l);
	if (f < last) quicksort(p_data_string, f, last);
}

void KBswitch(char* &data_string)
{
	int size = strlen(data_string);
	char* tmp = new char[(int)(size * 1.5 + 1)];
	tmp[(int)(size * 1.5)] = 0;
	
	for (int i = 0, j = 0; j < (size * 1.5); i = i + 2, j = j + 3)
	{
		tmp[j] = data_string[i];
		if ((j + 2) <= (size * 1.5))
		{
			tmp[j + 1] = 'K';
			tmp[j + 2] = 'B';
		}
	}
	data_string = tmp;
}
#ifdef WIN32
void dataReading(char* &buf, condition_variable &cv, mutex &ready, bool &write_ready, bool &read_ready)
{
	char* data_string;
	printf("reader start\n");

	while (true)
	{
		getDataString(&data_string);
		if (strcmp(data_string, "err") == 0) continue;

		int first = 0; 
		int last = strlen(data_string) - 1;
		quicksort(data_string, first, last);

		KBswitch(data_string);

		{
			unique_lock<mutex> locker(ready);
			printf("reader lock\n");
			if (!write_ready) cv.wait(locker, [&] {return write_ready; });
			printf("reader unlock\n");
		}
		
		buf = data_string;
		printf("data -> buf\n");

		write_ready = false;
		read_ready = true;
		printf("write false, read true\n");

		cv.notify_one();
	}
}
#elif __linux__
void dataReading(int &shmid, condition_variable &cv, mutex &ready, bool &write_ready, bool &read_ready)
{
	char* data_string;
	shared shm;
	char* buf;
	buf = (char*)shm.getShmptr(shmid);
	printf("reader start\n");

	while (true)
	{
		getDataString(&data_string);
		if (strcmp(data_string, "err") == 0) continue;

		int first = 0; 
		int last = strlen(data_string) - 1;
		quicksort(data_string, first, last);

		KBswitch(data_string);

		{
			unique_lock<mutex> locker(ready);
			printf("reader lock\n");
			if (!write_ready) cv.wait(locker, [&] {return write_ready; });
			printf("reader unlock\n");
		}
		sprintf(buf, data_string);
		printf("data -> buf\n");

		write_ready = false;
		read_ready = true;
		printf("write false, read true\n");

		cv.notify_one();
	}
}
#endif

int getSum(char* data_string)
{
	int str_len = strlen(data_string);
	int sum = 0;

	for (int i = 0; i < str_len; i++)
	{
		if (isdigit(data_string[i]))
		{
			char _int[1] = { data_string[i] };
			sum = sum + atoi(_int);
		}
	}
	return sum;
}

void signal_callback_handler(int signum) {
	printf("Caught signal %d\n", signum);
	keepRunning = 0;
	exit(signum);
}

int main()
{
	TcpClient client;
	condition_variable cv;
	mutex ready;
	bool write_ready = true;
	bool read_ready = false;
	char* buf;
	char* data_string;
	signal(SIGINT, signal_callback_handler);
	printf("settings done\n");

	printf("ented host ip and port\n");
	char hostIP[15];
	unsigned short hostPort = 0;
	scanf("%s", hostIP);
	scanf("%hd", &hostPort);
	getchar();
	
	if (client.ClientSet(&hostIP[0], hostPort) == TcpClient::connection_status::conn) {
		printf("Connected to server\n");
	}
	else {
		printf("Connection error!\n");
		return -1;
	}
	
#ifdef __linux__
	shared shm;
	if(!shm.createShm(96))
                printf("shared memory error\n");
            else
                printf("shared memory created\n");
	int shmid = shm.getMemId();
	buf = (char*)shm.getShmptr(shmid);
#endif

	thread reader(dataReading, ref(BUF), ref(cv), ref(ready), ref(write_ready), ref(read_ready));
	reader.detach();
	printf("reader detach\n");
	
	while (keepRunning == 1)
	{
		{
			unique_lock<mutex> locker(ready);
			printf("sender lock\n");
			cv.wait(locker, [&] {return read_ready; });
			printf("sender unlock\n");
		}

#ifdef WIN32	
		data_string = buf;
#elif __linux__
		sprintf(data_string, buf);
		memset(buf, '\0', shm.getSize());
#endif
		printf("buf -> data\n");
		read_ready = false;
		write_ready = true;
		printf("read false, write true\n");

		cv.notify_one();
		
		int sum = getSum(data_string);

		printf("%s %d\n", data_string, sum);
		
		int size = strlen(&to_string(sum)[0]);
		if (!(client.sendData(&to_string(sum)[0], size))) printf("send error");
	}
	reader.~thread();
#ifdef __linux__
	shmdt(buf);
	shm.freeShm();
#endif
	client.~TcpClient();

    return 0;   
}