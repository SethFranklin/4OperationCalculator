
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum NodeType
{

	NUM,
	ADD,
	SUB,
	MUL,
	DIV

};

struct Node
{

	struct Node* L;
	struct Node* R;
	unsigned char Type;
	float Value;
	
};

typedef struct Node Node;

char* InputString(FILE* fp, size_t size)
{

	char* str;
	char chr;
	size_t len = 0;

	str = malloc(sizeof(char) * size);
	if (!str) return str;

	while ((chr = fgetc(fp)) != EOF && chr != '\n')
	{

		str[len++] = chr;

		if (len == size)
		{

			str = realloc(str, sizeof(char) * (size += 16));
			if (!str) return str;

		}

	}

	return realloc(str, sizeof(char) * len);

}

Node* CreateTree(Node* tree, char* start, char* end)
{

	if (tree == NULL)
	{

		tree = (Node*) malloc(sizeof(Node));
		if (tree == NULL) return tree;

	}

	// find operation, find split point

	int brackpos = 0; // ( = +1, ) = -1

	char* firstopen;
	char* lastclose;

	char* i = start;

	char* split;

	tree->L = NULL;

	while (i < end && tree->Type == NUM)
	{

		switch (*i)
		{

			case '(':
				brackpos++;
				if (firstopen == NULL) firstopen = i;
				break;
			case ')':
				brackpos--;
				lastclose = i;
				break;
			case '+':
				if (brackpos == 0)
				{
					tree->Type = ADD;
					split = i;
				}
				break;
			case '-':
				if (brackpos == 0)
				{
					tree->Type = SUB;
					split = i;
				}
				break;
			case '*':
				if (brackpos == 0)
				{
					tree->Type = MUL;
					split = i;
				}
				break;
			case '/':
				if (brackpos == 0)
				{
					tree->Type = DIV;
					split = i;
				}
				break;

		}

		i++;

	}

	if (tree->Type == NUM)
	{

		if (firstopen == NULL) // number
		{

			end[1] = '\0';
			tree->Value = atof(start);

		}
		else // paranthesis
		{

			tree = CreateTree(tree, firstopen + 1, lastclose - 1);

		}

	}
	else
	{

		tree->L = CreateTree(tree->L, start, split - 1);
		tree->R = CreateTree(tree->R, split + 1, end);

	}

	return tree;

}

float EvaluateTree(Node* tree)
{

	switch(tree->Type)
	{

		case NUM:
			return tree->Value;
			break;
		case ADD:
			return EvaluateTree(tree->L) + EvaluateTree(tree->R);
			break;
		case SUB:
			return EvaluateTree(tree->L) - EvaluateTree(tree->R);
			break;
		case MUL:
			return EvaluateTree(tree->L) * EvaluateTree(tree->R);
			break;
		case DIV:
			return EvaluateTree(tree->L) / EvaluateTree(tree->R);
			break;

	}

}

void DeleteTree(Node* tree)
{

	if (tree->Type != NUM)
	{

		DeleteTree(tree->L);
		DeleteTree(tree->R);

	}

	free(tree);

}

int main(int argc, char** argv)
{

	char* OpString;
	size_t i;
	size_t len;
	float value;

	while (1)
	{

		printf("Operations: ");
		
		OpString = InputString(stdin, 10);
		len = strlen(OpString);

		Node* tree = NULL;
		tree = CreateTree(tree, OpString, OpString + len - 1);
		value = EvaluateTree(tree);
		DeleteTree(tree);

		printf("Value: %f\n", value);

		free(OpString);

	}

	return 0;

}