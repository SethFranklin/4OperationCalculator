
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node
{

	size_t NumChildren;
	struct Node* Children;
	char* Operations;
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

	str[len++] = '\0';

	return realloc(str, sizeof(char) * len);

}

Node* CreateTree(Node* tree, char* start, char* end)
{

	if (tree == NULL)
	{

		tree = (Node*) malloc(sizeof(Node));
		if (tree == NULL) return tree;

	}

	tree->Operations = (char*) malloc(sizeof(Node));
	if (tree->Operations == NULL) return tree;

	int brackpos = 0; // ( = +1, ) = -1

	char* firstopen = NULL;
	char* lastclose = NULL;

	char* i = start;
	char a;

	tree->NumChildren = 0;

	// need a temp array of split points

	char** splits = (char**) malloc(sizeof(char*));

	while (i <= end)
	{

		a = *i;

		if (a == '(')
		{

			brackpos++;
			if (firstopen == NULL) firstopen = i;

		}
		else if (a == ')')
		{

			brackpos--;
			lastclose = i;

		}
		else if (brackpos == 0 && (a == '+' || a == '-' || a == '*' || a == '/'))
		{

			if (tree->NumChildren == 0) tree->NumChildren = 1;
			tree->NumChildren++;

			splits = (char**) realloc(splits, (tree->NumChildren - 1) * sizeof(char*));
			tree->Operations = (char*) realloc(tree->Operations, (tree->NumChildren - 1) * sizeof(char));

			splits[tree->NumChildren - 2] = i;
			tree->Operations[tree->NumChildren - 2] = a;

		}

		i++;

	}

	if (tree->NumChildren == 0)
	{

		free(splits);

		if (firstopen == NULL) // Get value
		{

			size_t length = 1 + end - start;
			char* evalstr = malloc((1 + length) * sizeof(char));
			evalstr[length] = '\0';
			strncpy(evalstr, start, length);
			printf("%s\n", evalstr);
			tree->Value = atof(evalstr);
			free(evalstr);

		}
		else // Paranthesis recursion
		{

			tree = CreateTree(tree, firstopen + 1, lastclose - 1);

		}

	}
	else // Split recursion
	{

		tree->Children = (Node*) malloc(tree->NumChildren * sizeof(Node));
		if (tree->Children == NULL) return tree;

		CreateTree(tree->Children, start, splits[0] - 1);

		size_t i;

		for (i = 1; i < tree->NumChildren - 1; i++)
		{

			CreateTree(tree->Children + i, splits[i - 1] + 1, splits[i] - 1);

		}

		CreateTree(tree->Children + tree->NumChildren - 1, splits[tree->NumChildren - 2] + 1, end);

		free(splits);

	}

	return tree;

}

float EvaluateTree(Node* tree) // @TODO: correct order of operations
{

	if (tree->NumChildren == 0) return tree->Value;

	float total = EvaluateTree(tree->Children);

	size_t i;

	for (i = 1; i < tree->NumChildren; i++)
	{

		switch (tree->Operations[i - 1])
		{

			case '+':
				total = total + EvaluateTree(tree->Children + i);
				break;
			case '-':
				total = total - EvaluateTree(tree->Children + i);
				break;
			case '*':
				total = total * EvaluateTree(tree->Children + i);
				break;
			case '/':
				total = total / EvaluateTree(tree->Children + i);
				break;

		}

	}

	return total;

}

void DeleteTree(Node* tree)
{

	size_t i;

	for (i = 0; i < tree->NumChildren; i++)
	{

		DeleteTree(tree->Children + i);

	}

	free(tree->Operations);
	if (tree->NumChildren != 0) free(tree->Children);

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
		free(tree);
		printf("Value: %f\n", value);

		free(OpString);

	}

	return 0;

}