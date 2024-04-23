// Guard against multiple inclusions
#ifndef CRAWLER_H
#define CRAWLER_H

#include <gumbo.h>
#include <pthread.h>

typedef struct URLQueueNode {
    char *url;
    int depth;
    struct URLQueueNode *next;
} URLQueueNode;

typedef struct {
    URLQueueNode *head, *tail;
    pthread_mutex_t lock;
} URLQueue;

// Function declarations used by other files
void initQueue(URLQueue *queue);
void enqueue(URLQueue *queue, const char *url, int depth);
char *dequeue(URLQueue *queue, int *depth);
void search_for_links(GumboNode* node, URLQueue* queue, int depth);

#endif // CRAWLER_H
