#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "crawler.h"  // Include your crawler definitions
#include <stdlib.h>

// Declaration of functions from crawler.c that we need to test
extern void initQueue(URLQueue *queue);
extern void enqueue(URLQueue *queue, const char *url, int depth);
extern char *dequeue(URLQueue *queue, int *depth);
extern void search_for_links(GumboNode* node, URLQueue* queue, int depth);

// Test functions
void test_queue_operations() {
    URLQueue queue;
    initQueue(&queue);
    enqueue(&queue, "http://example.com", 0);
    int depth;
    char* url = dequeue(&queue, &depth);
    assert(strcmp(url, "http://example.com") == 0 && depth == 0);
    free(url);
    printf("Queue operations test passed.\n");
}

void test_html_parsing() {
    const char* test_html = "<html><body><a href='http://test.com'>Test</a></body></html>";
    GumboOutput* output = gumbo_parse(test_html);
    URLQueue queue;
    initQueue(&queue);
    search_for_links(output->root, &queue, 0);
    int depth;
    char* url = dequeue(&queue, &depth);
    assert(strcmp(url, "http://test.com") == 0 && depth == 1);
    free(url);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    printf("HTML parsing test passed.\n");
}

// Test runner
void run_tests() {
    test_queue_operations();
    test_html_parsing();
}

int main() {
    printf("Running tests...\n");
    run_tests();
    printf("All tests passed.\n");
    return 0;
}