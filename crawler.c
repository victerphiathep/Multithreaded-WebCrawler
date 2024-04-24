// Included libraries with starter code
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

// Non starter code libraries
#include <curl/curl.h>
#include <stdbool.h>
#include <gumbo.h>

//#define MAX_DEPTH 3
#define USER_AGENT "Mozilla/5.0 (compatible; MiniCrawler/1.0; +http://example.com/bot)"

// Define a structure for queue elements.
typedef struct URLQueueNode {
    char *url;
    int depth;
    struct URLQueueNode *next;
} URLQueueNode;

// Define a structure for a thread-safe queue.
typedef struct {
    URLQueueNode *head, *tail;
    pthread_mutex_t lock;
    pthread_cond_t cond;  // Condition variable for signaling
} URLQueue;

// Structure to hold the response data
typedef struct {
    char *data;     // Response buffer
    size_t size;    // Size of response
} CurlResponse;

typedef struct {
    URLQueue *queue;
    int max_depth;
    int thread_num;
} ThreadArgs;


// Initialize a URL queue.
void initQueue(URLQueue *queue) {
    queue->head = queue->tail = NULL;
    pthread_mutex_init(&queue->lock, NULL);
}

// Add a URL to the queue.
void enqueue(URLQueue *queue, const char *url, int depth, int max_depth) {
    if (depth > max_depth) {
        return;
    }

    URLQueueNode *newNode = malloc(sizeof(URLQueueNode));
    if (!newNode) {
        fprintf(stderr, "Failed to allocate memory for new queue node.\n");
        return;
    }
    newNode->url = strdup(url);
    newNode->depth = depth;
    newNode->next = NULL;

    pthread_mutex_lock(&queue->lock);
    if (queue->tail) {
        queue->tail->next = newNode;
    } else {
        queue->head = newNode;
    }
    queue->tail = newNode;
    pthread_mutex_unlock(&queue->lock);
}

// Remove a URL from the queue.
char *dequeue(URLQueue *queue, int *depth) {
    pthread_mutex_lock(&queue->lock);
    if (!queue->head) {
        pthread_mutex_unlock(&queue->lock);
        return NULL;
    }
    URLQueueNode *temp = queue->head;
    char *url = temp->url;
    *depth = temp->depth;
    queue->head = queue->head->next;
    if (!queue->head) {
        queue->tail = NULL;
    }
    pthread_mutex_unlock(&queue->lock);
    free(temp);
    return url;
}

// Placeholder for the function to fetch and process a URL.
// Helper function to collect and concatenate text from HTML elements
static void search_for_links(GumboNode* node, URLQueue* queue, int depth, int max_depth, int thread_num) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }
    GumboAttribute* href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
        printf("Thread %d: Found link at depth %d: %s\n", thread_num, depth, href->value);
        enqueue(queue, href->value, depth + 1, max_depth);
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        search_for_links(children->data[i], queue, depth + 1, max_depth, thread_num);
    }
}

// Callback Function

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    CurlResponse *mem = (CurlResponse *)userp;
    char *ptr = realloc(mem->data, mem->size + real_size + 1);
    if (!ptr) {
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, real_size);
    mem->size += real_size;
    mem->data[mem->size] = '\0';
    return real_size;
}

// Function to fetch and process a URL
void *fetch_url(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    int thread_num = args->thread_num;
    URLQueue *queue = args->queue;
    int max_depth = args->max_depth;

    printf("Thread %d starting\n", thread_num);

    CURL *curl = curl_easy_init();
    if (curl) {
        CurlResponse response = {0};
        curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);

        int depth;
        char *url;
        while ((url = dequeue(queue, &depth)) && depth <= max_depth) {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                fprintf(stderr, "Thread %d: curl_easy_perform() failed: %s\n", thread_num, curl_easy_strerror(res));
            } else {
                GumboOutput* output = gumbo_parse(response.data);
                search_for_links(output->root, queue, depth, max_depth, thread_num);
                gumbo_destroy_output(&kGumboDefaultOptions, output);
            }

            free(response.data);
            response.data = NULL;
            response.size = 0;
            free(url);
        }
        curl_easy_cleanup(curl);
    }

    printf("Thread %d finishing\n", thread_num);
    return NULL;
}

// Function to free the URL queue
void freeQueue(URLQueue *queue) {
    URLQueueNode *current = queue->head;
    while (current != NULL) {
        URLQueueNode *temp = current;
        current = current->next;
        free(temp->url);  // Free the string allocated with strdup
        free(temp);       // Free the node
    }
}

// Main function to drive the web crawler.
int main(int argc, char *argv[]) {

    // Getting user input

    printf("Enter the starting URL (example format: https://example.com): ");
    char starting_url[256];
    if (scanf("%255s", starting_url) != 1) {
        fprintf(stderr, "Error reading URL.\n");
        return 1;
    }

    printf("Enter the maximum crawl depth: ");
    int max_depth;
    if (scanf("%d", &max_depth) != 1) {
        fprintf(stderr, "Error reading maximum depth.\n");
        return 1;
    }

    int NUM_THREADS;
    printf("Enter the number of threads to use: ");
    if (scanf("%d", &NUM_THREADS) != 1) {
        fprintf(stderr, "Error reading number of threads.\n");
        return 1;
    }

    // Placeholder for creating and joining threads.
    // You will need to create multiple threads and distribute the work of URL fetching among them.
    // Example thread count, adjust as needed.
    URLQueue queue;
    initQueue(&queue);
    enqueue(&queue, starting_url, 0, max_depth);

    pthread_t *threads = malloc(NUM_THREADS * sizeof(pthread_t));
    ThreadArgs *args = malloc(NUM_THREADS * sizeof(ThreadArgs));

    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].queue = &queue;
        args[i].max_depth = max_depth;
        args[i].thread_num = i + 1;
        pthread_create(&threads[i], NULL, fetch_url, &args[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup and program termination.
    // You may need to add additional cleanup logic here.
    freeQueue(&queue);  // Free all nodes in the queue
    pthread_mutex_destroy(&queue.lock);  // Destroy the mutex

    printf("All threads completed. Queue freed and mutex destroyed.\n");
    
    return 0;
}