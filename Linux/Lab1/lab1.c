#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct list
{
	struct list *next;
	struct list *prev;
	char* text;
};

void insert(char* text, struct list *begin)
{
	struct list *newNode = (struct list *)malloc(sizeof(struct list));
	newNode->next = NULL;
	newNode->prev = NULL;
	struct list *currNode = begin;
	if(currNode->next == NULL) // empty list insert after begin
	{
		currNode->next = newNode;
		newNode->prev = currNode;
		newNode->text = text;
		return;
	}
	else
	{
		currNode = currNode->next;
		while(currNode->next != NULL)
		{
			if(strcmp(text, currNode->text) >= 0) // insert before currNode
			{
				newNode->next = currNode;
				newNode->prev = currNode->prev;
				currNode->prev->next = newNode;
				currNode->prev = newNode;
				newNode->text = text;
				return;
			}
			currNode = currNode->next;
		}
		if(strcmp(text, currNode->text) < 0) // inserst after end
		{
			currNode->next = newNode;
			newNode->prev = currNode;
			newNode->text = text;
			return;
		}
		else if (strcmp(text, currNode->text) >= 0) // insert before end
		{
			newNode->next = currNode;
			newNode->prev = currNode->prev;
			currNode->prev->next = newNode;
			currNode->prev = newNode;
			newNode->text = text;
			return;
		} 
	}
}

void print(struct list *begin)
{
	if (begin->next == NULL)
		printf("List is empty \n");
	else
	{
		struct list *currNode = begin->next;
		while (currNode->next != NULL)
		{
			printf("%s", currNode->text);
			currNode = currNode->next;
		}
		printf("%s \n", currNode->text);
	}
}

void delete(int index, struct list *begin)
{
	if (begin->next == NULL)
		printf("Can't delete the list is empty \n");
	else
	{
		struct list *currNode = begin->next;
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
		free(currNode->text);
		free(currNode);
	}
}

void clearList(struct list *begin)
{
	if (begin->next == NULL)
		return;
	else
	{
		struct list *currNode = begin->next;
		while (currNode->next != NULL)
		{
			currNode = currNode->next;
		}
		while (currNode->prev != NULL)
		{
			currNode = currNode->prev;
			free(currNode->next->text);
			free(currNode->next);
		}
	}
}

int main()
{
	struct list *begin = (struct list *)malloc(sizeof(struct list));
	begin->next = NULL;
	begin->prev = NULL;
	char zn = 0;
	char* text = NULL;
	size_t textLength = 0;
	int index = 0;
	int loop = 1;
	while (loop == 1)
	{
		zn = 0;
		printf("Insert command \n d) delete \n i) insert \n p) print \n q) quit \n");
		zn = getchar();
		getchar(); // usuwanie \n
		if (zn == 'i')
		{
			system("clear");
			printf("Insert text \n");
			text = NULL;
			getline(&text, &textLength, stdin);
			insert(text, begin);
		}
		if (zn == 'p')
		{
			system("clear");
			print(begin);
		}
		if (zn == 'q')
			loop = 0;
		if (zn == 'd')
		{
			system("clear");
			printf("Insert index \n");
			scanf("%d", &index);
			getchar();
			delete (index, begin);
			
		}
	}
	clearList(begin);
	free(begin);
	return 0;
}
