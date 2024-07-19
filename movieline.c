//Programming Assignment 4
//Walker Farnsworth
//wa934080

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAXCUSTOMERS 500000
#define MAXLINES 12
#define MAXLEN 50
#define MAXTIME 1000000000

typedef struct{
	
	char * name;
	int nt;
	int lineno;
	int t;
	
}customer_t;

typedef struct node_s{
	
	customer_t * custptr;
	struct node_s * next;
	
}node_t;

typedef struct{
	
	node_t * front;
	node_t * back;
	int size;
	
}queue_t;

queue_t * make_empty_queue();
void enqueue(queue_t * line, customer_t * new_customer);
customer_t * dequeue(queue_t * line);
customer_t * peek(queue_t * line);
int is_empty(queue_t * line);
void queue_size(queue_t * line);
node_t * generate_node(customer_t * new_customer);
customer_t * enter_customer();
void free_customer(customer_t * happy_customer);
void clearBuffer();

int main()
{
	// Generate queues
	queue_t ** lines = calloc(MAXLINES, sizeof(queue_t*));
	
	if(lines == NULL)
	{
		printf("calloc error in main...program exiting\n");
		exit(1);
	}
	
	for(int i = 0; i < MAXLINES; ++i)
		lines[i] = make_empty_queue();
	
	int booths[MAXLINES][MAXLINES] = {};
	int boothCount[MAXLINES] = {};
	int queueCount[MAXLINES] = {};
	int n, b;
	int k = 0;
	int kdivb;
	int kmodb;
	int nbFlag = 1;
	
	do{
		scanf("%d %d", &n, &b);
		clearBuffer();
		
		if(n <= 0 || n > MAXCUSTOMERS || b <= 0 || b > 12)
			printf("Invalid input.\n");
		else
			nbFlag = 0;
	}while(nbFlag);
	
	// Generate purchase entries for every n
	for(int i = 0; i < n; ++i)
	{
		// Enter person
		customer_t * new_customer = enter_customer();
		
		// Check if lineno is 0, if so, change that
		if(new_customer->lineno == 0)
		{
			if(i != 0)
			{
				// Find all nonempty queues and assign to first queue with the lowest count
				int low = n;
				int lowIndex = 0;
				
				for(int j = 0; j < MAXLINES; ++j)
				{
					if(is_empty(lines[j]) == 0)
					{
						if(low > lines[j]->size)
						{
							low = lines[j]->size;
							lowIndex = j;
						}
					}
				}
				
				new_customer->lineno = lowIndex + 1;
			}
			else
			{
				new_customer->lineno = 1;
			}
		}
		
		// Enqueue them
		enqueue(lines[new_customer->lineno - 1], new_customer);
	}
	
	// Find nonempty queues
	for(int i = 1; i <= MAXLINES; ++i)
	{
		if(is_empty(lines[i - 1]) == 0)
		{
			queueCount[i - 1] = i;
			++k;
		}
	}
	
	// Shorten array of nonempty queues to be k number of elements
	int size = MAXLINES;
	for(int i = 0; i < size; ++i)
	{
		if(queueCount[i] == 0)
		{
			for(int j = i; j < size; ++j)
			{
				queueCount[j] = queueCount[j + 1];
			}
			
			--size;
			--i;
		}
	}
	
	// Convert to 0's index
	for(int i = 0; i < k; ++i)
	{
		queueCount[i] -= 1;
	}
	
	kdivb = k / b;
	kmodb = k % b;
	
	// Divying up queues to queueCount
	int boothTraversal = 0;
	for(int i = 0; i < b; ++i)
	{
		for(int j = 0; j < kdivb; ++j)
		{
			booths[i][j] = queueCount[boothTraversal++];
			boothCount[i]++;
		}
		
		if(i < kmodb)
		{
			booths[i][kdivb] = queueCount[boothTraversal++];
			boothCount[i]++;
		}
	}
	
	int time;
	int combined;
	
	// Magic time
	for(int i = 0; i < b; ++i)
	{
		time = 0;
		combined = 0;
		printf("Booth %d\n", i + 1);
		
		// Use queue sizes to count how many total customers per booth
		for(int j = 0; j < boothCount[i]; ++j)
			combined += lines[booths[i][j]]->size;
		
		// Per booth
		for(int j = 0; j < combined; ++j)
		{
			// Find lowest time of arrival
			customer_t * hold = NULL;
			int checkPoint = 0;
			for(int k = 0; k < boothCount[i]; ++k)
			{
				if(k == 0 || hold == NULL)
				{
					if(is_empty(lines[booths[i][k]]) == 0)
					{
						hold = peek(lines[booths[i][k]]);
						checkPoint = k;
					}
				}
				else
				{
					customer_t * hold2 = NULL;
					
					if(is_empty(lines[booths[i][k]]) == 0)
					{
						hold2 = peek(lines[booths[i][k]]);
					}
					
					if(hold2 != NULL)
					{
						if(hold->t > hold2->t)
						{
							hold = hold2;
							checkPoint = k;
						}
					}
				}
			}
			
			hold = dequeue(lines[booths[i][checkPoint]]);
			
			if(hold->t > time)
				time = hold->t;
			
			int ticketsProcess = hold->nt * 5;
			int addConstant = ticketsProcess + 30;
			time += addConstant;
			
			printf("%s from line %d checks out at time %d.\n", hold->name, hold->lineno, time);
			
			free_customer(hold);
		}
		
		printf("\n");
	}
	
	// Free queues after all other nodes and people are freed
	for(int i = 0; i < MAXLINES; ++i)
		free(lines[i]);
	free(lines);
	
	lines = NULL;
	
	return 0;
}

queue_t * make_empty_queue()
{
	queue_t * temp = malloc(sizeof(queue_t));
	
	if(temp == NULL)
	{
		printf("malloc error in make_empty_queue...program exiting\n");
		exit(1);
	}
	
	temp->front = NULL;
	temp->back = NULL;
	temp->size = 0;
	
	return temp;
}

void enqueue(queue_t * line, customer_t * new_customer)
{
	node_t * assigned_node = generate_node(new_customer);
	
	if(is_empty(line) == 1)
		line->front = assigned_node;
	else
		line->back->next = assigned_node;
	
	line->back = assigned_node;
	
	queue_size(line);
}

customer_t * dequeue(queue_t * line)
{
	customer_t * temp = peek(line);
	node_t * target = line->front;
	
	line->front = line->front->next;
	free(target);
	
	return temp;
}

customer_t * peek(queue_t * line)
{
	return line->front->custptr;
}

int is_empty(queue_t * line)
{
	if(line->front == NULL)
		return 1;
	else
		return 0;
}

void queue_size(queue_t * line)
{
	// My epic oh-so good method for ensuring O(1) time
	
	line->size += 1;
}

node_t * generate_node(customer_t * new_customer)
{
	node_t * temp = malloc(sizeof(node_t));
	
	if(temp == NULL)
	{
		printf("malloc error in generate_node...program exiting\n");
		exit(1);
	}
	
	temp->custptr = new_customer;
	temp->next = NULL;
	
	return temp;
}

customer_t * enter_customer()
{
	customer_t * new_customer = malloc(sizeof(customer_t));
	
	if(new_customer == NULL)
	{
		printf("malloc error in enter_customer...program exiting\n");
		exit(1);
	}
	
	new_customer->name = malloc(sizeof(char) * MAXLEN);
	
	if(new_customer->name == NULL)
	{
		printf("malloc error in enter_customer...program exiting\n");
		exit(1);
	}
	
	int inputFlag = 1;
	
	do{
		scanf("%s %d %d", new_customer->name, &new_customer->nt, &new_customer->t);
		clearBuffer();
		
		if(new_customer->nt <= 0 || new_customer->nt > 500 || new_customer->t < 0 || new_customer->t > MAXTIME)
			printf("Invalid input.\n");
		else
			inputFlag = 0;
	}while(inputFlag);
	
	int p = (int)new_customer->name[0] - 65;
	int q = p % 13;
	new_customer->lineno = q;
	
	return new_customer;
}

void free_customer(customer_t * happy_customer)
{
	free(happy_customer->name);
	free(happy_customer);
}

void clearBuffer()
{
	while(getchar() != '\n');
}
