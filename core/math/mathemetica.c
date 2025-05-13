#include "mathematica.h"
#include <stdlib.h>

// Helper function to create a new node
static Vector* create_node(Generic* data) {
    Vector* node = (Vector*)malloc(sizeof(Vector));
    if (!node) return NULL;
    
    node->data = data;
    node->next = NULL;
    node->push_back = push;
    return node;
}

// Push implementation
void push(struct Vector* vec, Generic* data) {
    if (!vec) return;
    
    // Find the last node
    Vector* current = vec;
    while (current->next) {
        current = current->next;
    }
    
    // Create and append new node
    Vector* new_node = create_node(data);
    if (!new_node) return;
    
    current->next = new_node;
}

// Delete a node with specific data
void delete(struct Vector* vec) {
    if (!vec) return;
    
    Vector* current = vec;
    Vector* prev = NULL;
    
    while (current) {
        Vector* next = current->next;
        free(current);
        current = next;
    }
}

// Free the entire vector
void free_vector(struct Vector* vec) {
    delete(vec); // Just alias to delete in this implementation
}

// Allocate memory for a new vector
Vector* allocate_memory_for_vector() {
    Vector* vec = (Vector*)malloc(sizeof(Vector));
    if (!vec) return NULL;
    
    vec->data = NULL;
    vec->next = NULL;
    vec->push_back = push;
    return vec;
}

// Initialize a new vector
Vector* init_vector() {
    Vector* vec = allocate_memory_for_vector();
    return vec;
}