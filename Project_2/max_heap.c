#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "max_heap.h"

//Create a max_heap node and allocate the appropriate memory.
max_heap_node_ptr Create_Max_Heap_Node(float outbreaks, char* name)
{
    max_heap_node_ptr node = (max_heap_node_ptr)malloc(sizeof(struct Max_Heap_Node));
    node->data = (max_heap_data_ptr)malloc(sizeof(struct Max_Heap_Data));
    node->data->name = (char*)malloc((strlen(name)+1)*sizeof(char));
    strcpy(node->data->name,name);
    node->data->outbreaks = outbreaks;
    node->Right = NULL;
    node->Left = NULL;
    node->Parent = NULL;
    return node;
}

void Move_Up(max_heap_node_ptr new_node)
{
    int temp;
    //Until we reach the root, we swap the data of the nodes to maintain the nature of the Max_Heap, 
    //until we find a value bigger than the value of the new_node.
    while(new_node->Parent != NULL)
    {
        if(new_node->data->outbreaks > new_node->Parent->data->outbreaks)
        {
            Swap(new_node->data,new_node->Parent->data);
        }
        else 
            break;
        new_node = new_node->Parent;
    }
}

void Max_Heap_Insert(max_heap_ptr heap, float outbreaks, char* name)
{
    max_heap_node_ptr new_node = Create_Max_Heap_Node(outbreaks,name);
    
    //If heap is empty insert the root node.
    if(heap->root == NULL)
    {
        heap->root = new_node;
        heap->last = new_node;
        return;
    }
    
    // Start with the current last node and move up as long as the parent exists and the current node is its right child.
    max_heap_node_ptr current = heap->last;
    while(current->Parent != NULL)
    {
        if(current == current->Parent->Right)
            current = current->Parent;
        else 
            break;
    }

    if(current->Parent != NULL)
    {
        //Case 1: Parent has a right child, so insert the new_node to the leftmost node of the parent's right subtree.
        if(current->Parent->Right != NULL)
        {
            current = current->Parent->Right;
            while(current->Left != NULL)
                current = current->Left;
        }
        else //Case 2: The last node is a left child, so the new node must be his sibling (the right child).
            current = current->Parent;
    }
    else //Case 3: We are in the root, so the new node must be the left child of the leftmost node.
    {
        //Go down the tree to the leftmost node.
        while(current->Left != NULL)
            current = current->Left;
    }

    //Insert the new node to the node, which will be the last node.
    if(current->Left != NULL)
        current->Right = new_node;
    else 
        current->Left = new_node;
    new_node->Parent = current;
    heap->last = new_node;

    Move_Up(new_node);  //Fix the Max Heap.
}

//This function return true if we are in the level that all the children of the nodes has children.
int Check_For_Children(max_heap_node_ptr node)
{
    return ((node->Left->Left != NULL) && (node->Left->Right != NULL) && (node->Right->Left != NULL) && (node->Right->Right != NULL));
}

//This function swap the values of two nodes.
void Swap(max_heap_data_ptr node1, max_heap_data_ptr node2)
{
    max_heap_data_ptr temp = (max_heap_data_ptr)malloc(sizeof(struct Max_Heap_Data));
    temp->name = (char*)malloc((strlen(node1->name)+1)*sizeof(char));
    
    temp->outbreaks = node1->outbreaks;
    strcpy(temp->name,node1->name);
    
    node1->outbreaks = node2->outbreaks;
    free(node1->name);
    node1->name = (char*)malloc((strlen(node2->name)+1)*sizeof(char));
    strcpy(node1->name,node2->name);
    
    node2->outbreaks = temp->outbreaks;
    free(node2->name);
    node2->name = (char*)malloc((strlen(temp->name)+1)*sizeof(char));
    strcpy(node2->name,temp->name);

    free(temp->name);
    free(temp);
}

//This functions fix the heap after a pop of the root.
void Heapify(max_heap_ptr heap)
{
    max_heap_node_ptr current = heap->root;

    while((current->Left) != NULL || (current->Right) != NULL)
    {
        //If current node has both left and right child
        //Check if there are the children of its children
        //Then do the appropriate compares so fix the property of the max heap. 
        if((current->Left != NULL) && (current->Right != NULL))
        {
            if(Check_For_Children(current))
            {
                if(current->Left->data->outbreaks > current->Right->data->outbreaks)
                {
                    Swap(current->Left->data,current->data);
                    current = current->Left;
                }
                else
                {
                    Swap(current->Right->data,current->data);
                    current = current->Right;
                }            
            }
            else //THis means that we are in the penultimate level, so we have to do an additional compare with the parent.
            {
                if(current->Left->data->outbreaks > current->Right->data->outbreaks)
                {
                    if(current->Left->data->outbreaks > current->data->outbreaks)
                    {
                        Swap(current->Left->data,current->data);
                    }
                    break;
                }
                else
                {
                    if(current->Right->data->outbreaks > current->data->outbreaks)
                    {
                        Swap(current->Right->data,current->data);
                    }
                    break; 
                } 
            }
        }
        else //In tis case he have only the left child (surely)
        {
            if(current->Left->data->outbreaks > current->data->outbreaks)
            {
                Swap(current->Left->data,current->data);
            }
            break;
        }   
    }
}

//This function returns the root of the max heap and put in its position the last node.
max_heap_data_ptr Max_Heap_Delete(max_heap_ptr heap)
{
    max_heap_data_ptr temp = (max_heap_data_ptr)malloc(sizeof(struct Max_Heap_Data));

    //Case 1: Max_Heap is empty.
    if(heap->root == NULL)  
        return NULL;

    //Case 2: There is only the root in the Max Heap.
    if(heap->root == heap->last)
    {  
        temp->name = (char*)malloc((strlen(heap->root->data->name)+1)*sizeof(char));
        temp->outbreaks = heap->root->data->outbreaks;
        strcpy(temp->name,heap->root->data->name);
        free(heap->root->data->name);
        free(heap->root->data);
        free(heap->root);
        heap->root = NULL;
        return temp;
    }
    
    //Case 3: There is at least two nodes in the Max Heap.
    //Begin with the last node.
    max_heap_node_ptr current = heap->last;
    
    //Update the value of the root.
    temp->name = (char*)malloc((strlen(heap->root->data->name)+1)*sizeof(char));
    temp->outbreaks = heap->root->data->outbreaks;
    strcpy(temp->name,heap->root->data->name);
    heap->root->data->outbreaks = heap->last->data->outbreaks;
    free(heap->root->data->name);
    heap->root->data->name = (char*)malloc((strlen(heap->last->data->name)+1)*sizeof(char));
    strcpy(heap->root->data->name,heap->last->data->name);
    
    //Move up while the current node is a left child.
    while(current->Parent != NULL)
    {
        if(current == current->Parent->Left)
            current = current->Parent;
        else 
            break;
    }

    //Case 1: We aren't reach the root.
    if(current->Parent != NULL)
    {
        //Move to the sibling left node and move donw to the rightmost child.
        current = current->Parent->Left;
        
        while(current->Right != NULL)
            current = current->Right;
    }
    else if(current->Right != NULL)//Case 2: We reach the root.
        current = current->Right;
    
    //We reach to the second last node.
    //Free the previous last node and change the new last node.
    if(heap->last == heap->last->Parent->Right)
        heap->last->Parent->Right = NULL;
    else
        heap->last->Parent->Left = NULL;
    
    free(heap->last->data->name);
    free(heap->last->data);
    free(heap->last);
    heap->last = current;

    Heapify(heap);  //Fix the Max Heap.

    return temp;
}

void Print_Max_Heap(max_heap_node_ptr root)
{
    if(root != NULL) 
    { 
        Print_Max_Heap(root->Left); 
        printf("%f\n", root->data->outbreaks);
        Print_Max_Heap(root->Right);         
    } 
}

void printGivenLevel(max_heap_node_ptr root, int level) 
{ 
    if (root == NULL) 
        return; 
    if (level == 1) 
        printf("%f %s ",root->data->outbreaks ,root->data->name); 
    else if (level > 1) 
    { 
        printGivenLevel(root->Left, level-1); 
        printGivenLevel(root->Right, level-1); 
    } 
} 

void printLevelOrder(max_heap_node_ptr root, int height) 
{  
    int i; 
    for (i=1; i<=height; i++) 
    { 
        printGivenLevel(root, i); 
        printf("\n"); 
    } 
} 

void Deallocate_Max_Heap(max_heap_node_ptr node)
{
    if (node == NULL)
        return;	
	Deallocate_Max_Heap(node->Left);
	Deallocate_Max_Heap(node->Right);
    free(node->data->name);
    free(node->data);
    free(node);
}

//Returns the top k nodes of the max heap.
void Top_k(max_heap_ptr heap, int k, char** result)
{
    int i;
    max_heap_data_ptr data;

    for(i=0;i<k;i++)
    {
        if(heap->root != NULL)
        {
            data = Max_Heap_Delete(heap);
            if(*result==NULL)
            {
                //Save the date
                (*result) = (char*)malloc((strlen(data->name)+1)*sizeof(char));
                strcpy((*result),data->name);

                char res[10];
                sprintf(res,"%d",(int)(data->outbreaks*100));

                (*result) = (char*)realloc((*result), (strlen((*result))+1 ) + (strlen(res)+1));
                strcat((*result),res); 

                (*result) = (char*)realloc((*result), (strlen((*result))+1)+2);
                strcat((*result),"%");

                (*result) = (char*)realloc((*result), (strlen((*result))+1)+3);
                strcat((*result),"\n");
            }
            else
            {
                (*result) = (char*)realloc((*result), (strlen((*result))+1 ) + (strlen(data->name)+1));
                strcat((*result),data->name); 

                char res[10];
                sprintf(res,"%d",(int)(data->outbreaks*100));

                (*result) = (char*)realloc((*result), (strlen((*result))+1 ) + (strlen(res)+1));
                strcat((*result),res); 

                (*result) = (char*)realloc((*result), (strlen((*result))+1)+2);
                strcat((*result),"%");

                (*result) = (char*)realloc((*result), (strlen((*result))+1)+3);
                strcat((*result),"\n");
            }
            // printf("%s %d\n",data->name,data->outbreaks);
            free(data->name);
            free(data);
        }
    }
    if(k==0)
    {
        (*result) = (char*)malloc((strlen("empty")+1)*sizeof(char));
        strcpy((*result),"empty");
    }
}