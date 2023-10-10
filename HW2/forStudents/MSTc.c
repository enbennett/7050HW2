#include <stdio.h>
#include <stdlib.h>
#include "cs4050.h"
#include "MST.h"

struct node
{
    Edge edge;
    struct node * next;
};

struct queue
{
    // initalizing front and rear of queue to null
    struct node * front;
    struct node * rear;

};

int compare(const void * a, const void *b)
{
    Edge * edgea = (Edge *)a;
    Edge * edgeb = (Edge *)b;

    return(edgea->weight - edgeb->weight);
}

int * parents = NULL;

void makeset(int countVertices)
{
    for(int i=0; i<countVertices; i++)
    {
        parents[i] = i+1;
    }
}

int find(int u)
{
    return parents[u];
}

void unionset(int u, int v)
{
    for (int i=0; i<sizeof(parents); i++)
    {
        if(parents[i] == u)
        {
            parents[i] = v;
        }
    }
}

// adding nodes to queue
void enqueue(struct queue * queue, Edge edge)
{
    // create a new node containing the value we are enqueueing 
    struct node * n = malloc(sizeof(struct node));
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

void printq(struct queue * list) //print queue function, used mostly during debugging
{
    // Print numbers
    struct node * ptr = list->front;
    while (ptr != NULL)
    {
        struct node * next = ptr->next;
        printf("%i -> %i (%3.f)\n", ptr->edge.from, ptr->edge.to, ptr->edge.weight);
        free(ptr);
        ptr = next;
    }
}

void MST_Kruskal(Vertex vertices[], int countVertices, Edge edges[], int countEdges)
{
    printf("\n MST Kruskal \n");

    qsort(edges, countEdges, sizeof(Edge), compare);

    parents = (int *)malloc(countVertices * sizeof(int));
    if(parents == NULL)
    {
        printf("Memory not allocated");
        return;
    }

    makeset(countVertices);

    struct queue * mst = malloc(sizeof(struct queue));
    mst->front = NULL;
    mst->rear = NULL;

    for(int i=0; i<countEdges; i++)
    {
        if (find(edges[i].from-1) != find(edges[i].to-1))
        {
            enqueue(mst, edges[i]);
            unionset((edges[i].from-1), (edges[i].to-1));
        }

    }

    printq(mst);

    free(mst);

    free(parents);

}


