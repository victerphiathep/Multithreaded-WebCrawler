// Included libraries with starter code
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

// Non starter code libraries
#include <curl/curl.h>

// Define a structure for queue elements.
typedef struct URLQueueNode {
    char *url;
    struct URLQueueNode *next;
} URLQueueNode;

// Define a structure for a thread-safe queue.
typedef struct {
    URLQueueNode *head, *tail;
    pthread_mutex_t lock;
} URLQueue;

// Initialize a URL queue.
void initQueue(URLQueue *queue) {
    queue->head = queue->tail = NULL;
    pthread_mutex_init(&queue->lock, NULL);
}

// Add a URL to the queue.
void enqueue(URLQueue *queue, const char *url) {
    URLQueueNode *newNode = malloc(sizeof(URLQueueNode));
    newNode->url = strdup(url);
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
char *dequeue(URLQueue *queue) {
    pthread_mutex_lock(&queue->lock);
    if (queue->head == NULL) {
        pthread_mutex_unlock(&queue->lock);
        return NULL;
    }

    URLQueueNode *temp = queue->head;
    char *url = temp->url;
    queue->head = queue->head->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    free(temp);
    pthread_mutex_unlock(&queue->lock);
    return url;
}

// Placeholder for the function to fetch and process a URL.

// Function to handle the data received from HTTP requests
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    // Append the data to a string or buffer (you need to define this part)
    return real_size;
}

void *fetch_url(void *arg) {
    // Cast arg to your queue or custom data structure.
    URLQueue *queue = (URLQueue *)arg;
    
    // TODO: Implement fetching and processing logic here.
    // This could involve sending HTTP requests, parsing HTML content to find links,
    // and adding new URLs to the queue.
    CURL *curl;
    CURLcode res;

    // Initialize CURL
    curl = curl_easy_init();
    if(curl) {
        while (true) {
            pthread_mutex_lock(&queue->lock);
            if (queue->head == NULL) {
                pthread_mutex_unlock(&queue->lock);
                break;  // Exit if queue is empty
            }
            char *url = dequeue(queue);
            pthread_mutex_unlock(&queue->lock);

            if (url == NULL) {
                continue;  // No URL fetched, skip this iteration
            }

            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            // You need to define a buffer to store the fetched content
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
            
            // Perform the request, res will get the return code
            res = curl_easy_perform(curl);
            if(res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            } else {
                // Parse buffer here to find URLs and enqueue them
            }

            free(url);  // URL dequeued and processed, so free it
        }

        // Cleanup
        curl_easy_cleanup(curl);
    }

    return NULL;
}

// Main function to drive the web crawler.
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <starting-url>\n", argv[0]);
        return 1;
    }

    URLQueue queue;
    initQueue(&queue);
    enqueue(&queue, argv[1]);

    // Placeholder for creating and joining threads.
    // You will need to create multiple threads and distribute the work of URL fetching among them.
    const int NUM_THREADS = 4; // Example thread count, adjust as needed.
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, fetch_url, (void *)&queue);
    }

    // Join threads after completion.
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup and program termination.
    // You may need to add additional cleanup logic here.
    
    return 0;
}