#include <stdio.h>
#include "Windows.h"

struct list
{
	struct list* next;
	struct list* prev;
	PROCESS_INFORMATION proc;
};

void insert(PROCESS_INFORMATION proc, struct list** begin)
{
	struct list* newNode = (struct list*)malloc(sizeof(struct list));
	if (newNode != NULL)
	{
		newNode->next = NULL;
		newNode->prev = NULL;
		newNode->proc = proc;
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
			DWORD pr = GetPriorityClass(currNode->proc.hProcess);
			printf("%d) %d Priority: %d\n", i,currNode->proc.dwProcessId, pr);
			currNode = currNode->next;
			i++;
		}
		DWORD pr = GetPriorityClass(currNode->proc.hProcess);
		printf("%d) %d Priority: %d\n", i, currNode->proc.dwProcessId, pr);
	}
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
		UINT code = 0;
		TerminateProcess(currNode->proc.hProcess, code);
		CloseHandle(currNode->proc.hProcess);
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
				printf("Index %d is out of bound \n", index);
				return;
			}
			currNode = currNode->next;
			i++;
		}
		currNode->prev->next = currNode->next;
		if (currNode->next != NULL)
			currNode->next->prev = currNode->prev;
		UINT code = 0;
		TerminateProcess(currNode->proc.hProcess, code);
		CloseHandle(currNode->proc.hProcess);
		free(currNode);
	}
}

void removeClosedProcesses(struct list** begin) {
	if ((*begin) == NULL)
		return;
	else
	{
		int index = 0;
		struct list* currNode = (*begin);
		DWORD exitCode;
		while (currNode->next != NULL)
		{
			GetExitCodeProcess(currNode->proc.hProcess, &exitCode);
			currNode = currNode->next;
			if (!exitCode)
				delete(index, begin);			
			index++;
		}
		GetExitCodeProcess(currNode->proc.hProcess, &exitCode);
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
			UINT code = 0;
			if (currNode->next != NULL) {
				TerminateProcess(currNode->next->proc.hProcess, code);
				CloseHandle(currNode->next->proc.hProcess);
				free(currNode->next);
			}			
		}
		UINT code = 0;
		TerminateProcess(currNode->proc.hProcess, code);
		CloseHandle(currNode->proc.hProcess);
		free(currNode);
	}
}

PROCESS_INFORMATION new_process_start(DWORD priorityClass)
{
	char exe_path[] = "ProgramPodrzedny.exe";        /* wiersz poleceń (command line) Windows jako proces podrzędny */
	STARTUPINFO structStartupInfo;      /* struktura startowa */
	PROCESS_INFORMATION structProcInfo; /* struktura z danymi procesu podrzędnego */

	// wypełniamy strukturę startową danymi bieżącego procesu  
	GetStartupInfo(&structStartupInfo);
	// tworzymy nowy proces podrzędny
	if (!CreateProcess // twierdzi że przecieka, ale dane o procesie nie giną
	(NULL,              /* lpApplicationName - nazwa modułu, zostawiamy puste */
		exe_path,           /* lpCommandLine - lista parametrów, jeśli lpApplicationName == NULL to pierwszy parametr tu jest ścieżką do pliku EXE który będzie procesem podrzędnym */
		NULL,               /* lpProcessAttributes - wskaźnik do struktury SECURITY_ATTRIBUTES, która określa, czy zwrócone dojście do nowego obiektu procesu może być dziedziczone przez procesy potomne */
		NULL,               /* lpThreadAttributes */
		FALSE,              /* bInheritHandles */
		CREATE_NEW_CONSOLE, /* dwCreationFlags - ustawiając flagę CREATE_NEW_CONSOLE umożliwiamy otwarcie nowego okna konsoli dla procesu podrzędnego */
		0,                  /* lpEnvironment */
		0,                  /* lpCurrentDirectory */
		&structStartupInfo, /* lpStartupInfo - wskaźnik na strukturę startową */
		&structProcInfo     /* lpProcessInformation - wskaźnik na strukturę z danymi procesu podrzędnego */
	))
		printf("CreateProcess failed (%d).\n", GetLastError());
	SetPriorityClass( structProcInfo.hProcess, priorityClass);
	CloseHandle(structProcInfo.hThread);
	return structProcInfo;
}

PROCESS_INFORMATION createNewProcess() {
	system("cls");
	char option = 0;
	char placeHolder;
	PROCESS_INFORMATION a;
	while (1) {	
		printf("1) Idle \n2) Normal \n3) High \n4) Realtime \n");
		option = getchar();
		placeHolder = getchar(); // usuń \n
		system("cls");
		switch (option)
		{
		case '1':
			return new_process_start(IDLE_PRIORITY_CLASS);
		case '2':
			return new_process_start(NORMAL_PRIORITY_CLASS);
		case '3':
			return new_process_start(HIGH_PRIORITY_CLASS);
		case '4':
			return new_process_start(REALTIME_PRIORITY_CLASS);
		default:
			printf("No such option \n");
		}
	}
	return a;
}

int main()
{
	struct list* idList = NULL;
	int res = 0;
	PROCESS_INFORMATION procInfo;
	char runApp = 1;
	char option = 0;
	char placeHolder; // przechowuje \n i wartości zwracane przez funkcje gdy nie są one potrzebne
	while (runApp) {	
		printf("c) Create new process \nq) Quit \np) Print \ns) Stop process \n");
		option = getchar();
		placeHolder = getchar(); // usuń \n
		removeClosedProcesses(&idList);
		system("cls");
		switch (option)
		{
		case 'c':
			procInfo = createNewProcess();
			insert(procInfo, &idList);
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
			placeHolder = getchar(); // usuń \n
			delete(res, &idList);
			break;
		default:
			break;
		}
		
	}
	clearList(idList);
	return 0;
}
