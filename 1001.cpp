#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>

// #include <unistd.h>
// #include <dirent.h>
// #include <sys/user.h>
// #include <sys/time.h>
// #include <sys/resource.h>
// #include <sys/wait.h>
// #include <sys/ptrace.h>
// #include <sys/syscall.h>
using namespace std;

int fun[100000];
void f(int b)
{
	//int test[100];
	if(b == 0)
		return ;
	f(b--);
}
int main(void)
{
	int num;
	while(cin >> num)
	{
		cout << num << endl;
	}
	for(int i = 99999; i < 101010; ++i)
	{
		fun[i] = 100;
	}
	//f(10000000);
	// int pid = fork();
	// if(pid == 0)
	// {
	// 	exit(0);
	// }
	// else {
	// 	waitpid(pid, NULL, NULL);
	// 	exit(0);
	// }
	return 0;
}
