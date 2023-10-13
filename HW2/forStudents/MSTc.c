#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cs4050.h"
#include "MST.h"

//used to keep edges for adding to MST
struct node
{
    Edge edge;
    struct node * next;
};

//used to hold MST
struct queue
{
    struct node * front;
    struct node * rear;

};

//used to hold a list of vertices within a subset for our union find
struct list
{
    int vertex;
    struct list * next;
};

//used to make our list of subsets
struct subset
{
    int name;
    struct list * vertices;
    struct subset * next;
};

//initialize our global linklist parent, which will contain each subset
struct subset * parents = NULL;

//comparator function for quicksort
int compare(const void * a, const void * b)
{
    Edge * edgea = (Edge *)a;
    Edge * edgeb = (Edge *)b;

    return(edgea->weight - edgeb->weight); //will sort smallest to largest
}

//used to add the starting vertex to our list within each subset
void addlist(struct subset * sub, int vert)
{
    //allocate a new vertex to go in the list
    struct list * nvertex = malloc(sizeof(struct list));
    if(nvertex == NULL)
    {
        printf("Memory not allocated"); //return if malloc fails
        return;
    }

    nvertex->vertex = vert; //set vertex value = to value we passed in
    nvertex->next = NULL; //next is going to be NULL

    sub->vertices = nvertex; //add to list front and return

}

void makeset(int vertex)
{
    //allocate a new subset
    struct subset * n = malloc(sizeof(struct subset));
    if(n == NULL)
    {
        printf("Memory not allocated"); //if malloc fails return
        return;
    }
    //set subset name to the first vertex, each vertex will begin with it's own subset
    n->name = vertex;
    n->next = NULL;
    addlist(n, vertex); //add the vertex to it's own list of vertices as well

    if(parents == NULL) // if list is empty
    {
        parents=n;
    }

    else
    {
        // Iterate over nodes in list
        struct subset * ptr = parents;
        while (ptr != NULL)
        {
            // If at end of list
            if (ptr->next == NULL)
            {
                // Append node
                ptr->next = n;
                break;
            }
            ptr = ptr->next;
        }
    }
    
}

struct subset * find(int u)
{
    //start at the front of our subsets list
    struct subset * currSubset = parents;

    //iterate through subset list
    while(currSubset != NULL)
    {
        if (currSubset->name == u) //if the subset name is the node we are searching for stop, it is it's own parent
        {
            return currSubset;
        }
        //iterate through each subsets list of vertices
        struct list * currVertex = currSubset->vertices;
        while(currVertex != NULL)
        {
            if(currVertex->vertex == u)
            {
                return currSubset; //if we find the name in the list stop and return parent it's in
            }
            currVertex = currVertex->next;
        }
        currSubset = currSubset->next;
    }
    
    return currSubset; //this should not happen, indicates error, every node should have a parent
}

void deletesubset(struct subset * v)
{
    if(v->name == parents->name) //if it's at the top of the parents 
    {
        struct subset * top = parents;
        struct subset * second = top->next;
        free(top); // delete the subset
        parents=second; // connect the two subsets on either side of the one we freed
    }
    else // if it's in the middle of parents
    {
        struct subset * temp = parents; // this will be the one in front of the one we are deleting
        struct subset * forward = temp->next; 
        while(forward->name != v->name) // find the one we are going to delete
        {
            temp = temp->next;
            forward = forward->next;
        }
        //remove combined subset the results
        struct subset * next = v->next;
        free(v); // delete the subset
        temp->next=next; // connect the two subsets on either side of the one we freed
    }
}

void unionset(struct subset * u, struct subset * v) // v will always append to u
{
    // Iterate over nodes in list
    struct list * ptr = u->vertices;

    while (ptr != NULL)
    {
        // If at end of list
        if (ptr->next == NULL)
        {
            // Append node
            ptr->next = v->vertices; // add v to the end of u's list
            break;
        }
        ptr = ptr->next;
        
    }

    deletesubset(v); // delete v from subsets (it's vertices list will only exist now in u)

}

// adding nodes to our mst queue
void enqueue(struct queue * queue, Edge edge)
{
    // create a new node containing the value we are enqueueing 
    struct node * n = malloc(sizeof(struct node));
    if(n == NULL)
    {
        printf("Memory not allocated");
        return;
    }
    n->edge = edge;
    n->next = NULL;

    if(queue->front == NULL) // if list is empty
    {
        // new node becomes front and rear
        queue->front = n;
        queue->rear = n;
        return;
    }

    // if list is not empty
    queue->rear->next = n; // move rear to new node
    queue->rear = n; // rear is at new node
}

void printq(struct queue * list) //print and free queue function, used to print MST at end of alg
{
    // make pointer to scroll through queue, starting at front
    struct node * ptr = list->front;
    while (ptr != NULL)
    {
        struct node * next = ptr->next; // set a temporary next
        printf("%i -> %i (%3.f)\n", ptr->edge.from, ptr->edge.to, ptr->edge.weight); // print where we are
        free(ptr); //free where we were
        ptr = next; //go to next
    }
}

void printAdjacency(struct list * list) //print adjacency function, used within print parents during debugging
{
    for(struct list * ptr = list; ptr != NULL; ptr = ptr->next)
    {
        printf("vertex in subset %i \n", ptr->vertex);
    }    

}

void printParent(struct subset * list) //print parents function, used mostly during debugging
{
    for (struct subset * ptr = list; ptr != NULL; ptr = ptr->next)
    {
        printf("subset named %i \n", ptr->name);
        printAdjacency(ptr->vertices);
    }
}

void MST_Kruskal(Vertex vertices[], int countVertices, Edge edges[], int countEdges)
{
    printf("\n MST Kruskal \n");

    qsort(edges, countEdges, sizeof(Edge), compare); //sort all edges by weight smallest to largest

    for(int i=0; i<countVertices; i++) //to start off make each vertex it's own subset
    {
        makeset(vertices[i].number);
    }

    struct queue * mst = malloc(sizeof(struct queue)); //create queue to store MST
    if(mst == NULL)
    {
        printf("Memory not allocated"); //if malloc fails return
        return;
    }

    //initialize queue to null
    mst->front = NULL;
    mst->rear = NULL;

    for(int i=0; i<countEdges; i++) //iterate through each edge by weight from smallest to largest
    {
        struct subset * parent1 = find(edges[i].from);
        struct subset * parent2 = find(edges[i].to);
        if (parent1->name != parent2->name) //check to see if edges are already in same set (will prevent cycle)
        {
            enqueue(mst, edges[i]); // if not cycle add edge to our mst
            unionset(parent1, parent2); // union edge to prevent future cycle
            //printParent(parents);
        }
    }

    //prints and frees our mst queue
    printq(mst);

    //free mst alloc
    free(mst);

    //frees last remaining parent and it's vertices, others freed during union
    struct list * temp = parents->vertices;
    while(temp != NULL)
    {
        struct list * next = temp->next;
        free(temp);
        temp = next;
    }
    free(parents);

}


