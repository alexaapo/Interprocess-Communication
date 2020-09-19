struct Max_Heap_Data
{
    float outbreaks;
    char* name;
};

typedef struct Max_Heap_Data *max_heap_data_ptr;

struct Max_Heap_Node
{
    max_heap_data_ptr data;
	struct Max_Heap_Node* Parent;
	struct Max_Heap_Node* Right;
	struct Max_Heap_Node* Left;    
};

typedef struct Max_Heap_Node *max_heap_node_ptr;

struct Max_Heap
{
    max_heap_node_ptr root;
    max_heap_node_ptr last;
};

typedef struct Max_Heap *max_heap_ptr;

void Max_Heap_Insert(max_heap_ptr, float, char*);
void Move_Up(max_heap_node_ptr);
max_heap_node_ptr Create_Max_Heap_Node(float, char*);
void Print_Max_Heap(max_heap_node_ptr);
max_heap_data_ptr Max_Heap_Delete(max_heap_ptr);
void Heapify(max_heap_ptr);
int Check_For_Children(max_heap_node_ptr);
void printLevelOrder(max_heap_node_ptr, int);
void printGivenLevel(max_heap_node_ptr, int);
void Deallocate_Max_Heap(max_heap_node_ptr);
void Swap(max_heap_data_ptr, max_heap_data_ptr);
void Top_k(max_heap_ptr, int,char**);

