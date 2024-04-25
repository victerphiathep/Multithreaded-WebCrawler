# Multithreaded Web Crawler in C

## Project Overview
This project involves developing a multithreaded web crawler in C that efficiently fetches, parses, and processes web pages from the internet. Utilizing multiple threads, the web crawler aims to improve performance and efficiency, reducing the time required to crawl a set of URLs.

## Problem Statement
The vast and ever-growing internet presents challenges for traditional single-threaded web crawlers, which are often insufficient to handle the volume of information available. This project addresses the need for a web crawler that can efficiently navigate the web by fetching multiple pages in parallel.

## Features

### Thread Management
- Manages worker threads that fetch and process web pages concurrently.
- Implements mechanisms for gracefully terminating threads once their tasks are complete.

### URL Queue
- Implements a thread-safe queue to manage URLs pending fetch operations.
- Ensures that multiple threads can add to and fetch from the queue without data corruption.

### HTML Parsing
- Parses HTML content to extract links using the Gumbo parsing library.
- Identifies and follows links within HTML documents.

### Depth Control
- Allows controlling the depth of the crawl to prevent infinite recursion.
- Users can specify the maximum depth for the crawl.

### Synchronization
- Utilizes mutexes to ensure that shared resources like the URL queue are accessed safely.

### Error Handling
- Handles network failures, invalid URLs, and other exceptions.
- Ensures the crawler can recover from errors and continue operating.

### Logging
- Logs the progress of the web crawler, noting visited URLs and any errors encountered.
- Supports varying verbosity levels for the logging system.

## Developers
- Victer Phiathep
- Kenbrian Tuppalk

## Compilation and Usage

### Requirements
Ensure your system has `gcc` installed and can compile C code using the C11 standard. Additionally, you must have the `curl` and `gumbo` libraries installed.

### Compilation
Enter this command to compile:
gcc -std=c11 -pedantic -pthread crawler.c -o crawler

Developers:
Victer Phiathep
Kenbrian Tuppal
