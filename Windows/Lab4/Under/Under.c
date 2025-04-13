#include <stdio.h>
#include "Windows.h"

DWORD WINAPI fib(LPVOID arg);

struct list
{
	struct list* next;
	struct list* prev;
	HANDLE threedHandle;
	DWORD threadId;
};

void insert(DWORD Id, HANDLE handle, struct list** begin)
{
	struct list* newNode = (struct list*)malloc(sizeof(struct list));
	if (newNode != NULL)
	{
		newNode->next = NULL;
		newNode->prev = NULL;
		newNode->threadId = Id;
		newNode->threedHandle = handle;
		if ((*begin) == NULL)
			(*begin) = newNode;
		else
		{
			struct list* currNode = (*begin);
			while (currNode->next != NULL)
				currNode = currNode->next;

			currNode->next = newNode;
			newNode->prev = currNode;
			return;
		}
	}
	else
		printf("Memory allocation error \n");
}

void print(struct list* begin)
{
	if (begin == NULL)
		printf("List is empty \n");
	else
	{
		int i = 0;
		struct list* currNode = begin;
		while (currNode->next != NULL)
		{
			DWORD pr = GetThreadPriority(currNode->threedHandle);
			printf("%d) %d Priority: %d\n", i, currNode->threadId, pr);
			currNode = currNode->next;
			i++;
		}
		DWORD pr = GetThreadPriority(currNode->threedHandle);
		printf("%d) %d Priority: %d\n", i, currNode->threadId, pr);
	}
}

void changePriority(int index, DWORD priorityClass, struct list* begin)
{
		struct list* currNode = begin;
		int i = 0;
		while (i != index)
		{
			if (currNode->next == NULL && i != index)
			{
				printf("Index %d is out of bound \n", index);
				return;
			}
			currNode = currNode->next;
			i++;
		}
		SetThreadPriority(currNode->threedHandle, priorityClass);
}

void delete(int index, struct list** begin)
{
	if ((*begin) == NULL)
	{
		printf("Can't delete the list is empty \n");
		return;
	}
	if (index == 0)
	{
		struct list* currNode = (*begin);
		if (currNode->next == NULL)
			(*begin) = NULL;
		else {
			(*begin) = (*begin)->next;
			(*begin)->prev = NULL;
		}
		TerminateThread(currNode->threedHandle, 0);
		CloseHandle(currNode->threedHandle);
		free(currNode);
	}
	else
	{
		struct list* currNode = (*begin);
		int i = 0;
		while (i != index)
		{
			if (currNode->next == NULL && i != index)
			{
				printf("Index %d is out of bounce \n", index);
				return;
			}
			currNode = currNode->next;
			i++;
		}
		currNode->prev->next = currNode->next;
		if (currNode->next != NULL)
			currNode->next->prev = currNode->prev;
		TerminateThread(currNode->threedHandle, 0);
		CloseHandle(currNode->threedHandle);
		free(currNode);
	}
}

void removeFinishedThreads(struct list** begin) {
	if ((*begin) == NULL)
		return;
	else
	{
		int index = 0;
		struct list* currNode = (*begin);
		DWORD exitCode;
		while (currNode->next != NULL)
		{
			GetExitCodeThread(currNode->threedHandle, &exitCode);
			currNode = currNode->next;
			if (!exitCode)
				delete(index, begin);
			index++;
		}
		GetExitCodeThread(currNode->threedHandle, &exitCode);
		if (!exitCode)
			delete(index, begin);
	}
}

void clearList(struct list* begin)
{
	if (begin == NULL)
		return;
	else
	{
		struct list* currNode = begin;
		while (currNode->next != NULL)
			currNode = currNode->next;
		while (currNode->prev != NULL)
		{
			currNode = currNode->prev;
			if (currNode->next != NULL) {
				TerminateThread(currNode->next->threedHandle, 0);
				CloseHandle(currNode->next->threedHandle);
				free(currNode->next);
			}
		}
		TerminateThread(currNode->threedHandle, 0);
		CloseHandle(currNode->threedHandle);
		free(currNode);
	}
}

void newThreadStart(DWORD priorityClass, struct list** begin)
{
	printf("Enter a positive Number\n");
	char placeHolder;
	int num = 50;
	placeHolder = scanf_s("%d", &num);
	placeHolder = getchar(); // usuñ \n
	DWORD id; // identyfikator watku
	HANDLE newThreadHandle = CreateThread(NULL, 0, fib, (LPVOID)&num, 0, &id);

	if (newThreadHandle != NULL)
	{
		insert(id, newThreadHandle, begin);
		SetThreadPriority(newThreadHandle, priorityClass);
	}
	else
		printf("Error new Thred coulnd not be create\n");
}

void createNewThread(struct list** begin) {
	system("cls");
	char option = 0;
	char placeHolder;
		printf("1) Idle \n2) Lowest \n3) Below Normal \n4) Normal \n5) Above Normal\n6) Highest\n7) Time Critical\n");
		option = getchar();
		placeHolder = getchar(); // usuñ \n
		system("cls");
		switch (option)
		{
		case '1':
			newThreadStart(THREAD_PRIORITY_IDLE,begin);
			break;
		case '2':
			newThreadStart(THREAD_PRIORITY_LOWEST, begin);
			break;
		case '3':
			newThreadStart(THREAD_PRIORITY_BELOW_NORMAL, begin);
			break;
		case '4':
			newThreadStart(THREAD_PRIORITY_NORMAL, begin);
			break;
		case '5':
			newThreadStart(THREAD_PRIORITY_ABOVE_NORMAL, begin);
			break;
		case '6':
			newThreadStart(THREAD_PRIORITY_HIGHEST, begin);
			break;
		case '7':
			newThreadStart(THREAD_PRIORITY_TIME_CRITICAL, begin);
			break;
		default:
			printf("No such option \n");
			break;	
	}
}

void priorityMenu(int index, struct list* begin){
	system("cls");
	char option = 0;
	char placeHolder;
	if (begin == NULL)
	{
		printf("The list is empty \n");
		return;
	}
		printf("1) Idle \n2) Lowest \n3) Below Normal \n4) Normal \n5) Above Normal\n6) Highest\n7) Time Critical\n");
		option = getchar();
		placeHolder = getchar(); // usuñ \n
		system("cls");
		switch (option)
		{
		case '1':
			changePriority(index,THREAD_PRIORITY_IDLE,begin);
			break;
		case '2':
			changePriority(index,THREAD_PRIORITY_LOWEST, begin);
			break;
		case '3':
			changePriority(index,THREAD_PRIORITY_BELOW_NORMAL, begin);
			break;
		case '4':
			changePriority(index,THREAD_PRIORITY_NORMAL, begin);
			break;
		case '5':
			changePriority(index,THREAD_PRIORITY_ABOVE_NORMAL, begin);
			break;
		case '6':
			changePriority(index,THREAD_PRIORITY_HIGHEST, begin);
			break;
		case '7':
			changePriority(index,THREAD_PRIORITY_TIME_CRITICAL, begin);
			break;
		default:
			printf("No such option \n");
	}
}

DWORD WINAPI fib(LPVOID arg) {

	int* numP = (int*)arg;
	int num = *numP;
	if (num < 1)
		return 1;
	else if (num == 1 || num == 2)
	{
		printf("Value = %d\n", 1);
		return 0;
	}		
	else
	{
		int prev = 1;
		int current = 1;
		int next;
		for (int i = 2; i < num; i++)
		{
			next = current + prev;
			prev = current;
			current = next;
		}
		Sleep(60000);
		printf("Value = %d\n", next);
	}
	return 0;
}

int main()
{
	DWORD id = GetCurrentProcessId();
	printf("Proces number: %d \n", id);
	struct list* idList = NULL;
	int res = 0;
	char runApp = 1;
	char option = 0;
	char placeHolder; // przechowuje \n i wartoœci zwracane przez funkcje gdy nie s¹ one potrzebne
	while (runApp) {	
		printf("c) Create new thread \nq) Quit \np) Print \ns) Stop thread \ne) Change priority \n");
		option = getchar();
		placeHolder = getchar(); // usuñ \n
		removeFinishedThreads(&idList);
		system("cls");
		switch (option)
		{
		case 'c':
			createNewThread(&idList);
			break;
		case 'q':
			runApp = 0;
			break;
		case 'p':
			print(idList);
			break;
		case 's':
			system("cls");
			printf("Enter index \n");
			placeHolder = scanf_s("%d", &res);
			placeHolder = getchar(); // usuñ \n
			delete(res, &idList);
			break;
		case 'e':
			system("cls");
			printf("Enter index \n");
			placeHolder = scanf_s("%d", &res);
			placeHolder = getchar(); // usuñ \n
			priorityMenu(res, idList);
			break;
		default:
			break;
		}	
	}
	clearList(idList);
	return 0;
}
