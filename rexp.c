#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "dictionary.h"
#include "rexp.h"



int string_pos =  0;

struct enode * string_to_enode(const char * s) {
  struct enode * node = malloc(sizeof(struct enode));
  while(1) {
    if(s[string_pos] == '+') {
      node->type = '+';
      break;
    }
    if(s[string_pos] == '-' && 
       !(s[string_pos + 1] >= '0'  && s[string_pos + 1] <= '9' )) { 
      node->type = '-';
      break;
    }
    if(s[string_pos] == '*') {
      node->type = '*';
      break;
    }
    if(s[string_pos] == '/') {
      node->type = '/';
      break;
    }
    if((s[string_pos] >= '0' && s[string_pos] <= '9') 
       || s[string_pos] == '-') {
      node->type = 'n';
      break;
    }
    if(s[string_pos] == '\0') {
      printf("Should never reach here\n");
      return NULL;
    }
    if((s[string_pos] >= 'a' && s[string_pos] <= 'z') || 
       (s[string_pos] >= 'A' && s[string_pos] <= 'Z')) {
      node->type = 'v';
      break;
    }
    else {
      string_pos++;
    }
  }
  if(node->type == 'n') {
    int number;
    char * num_str = malloc(33 * sizeof(char));
    sscanf(s + string_pos, "%s", num_str);
    sscanf(s + string_pos, "%d", &number);
    string_pos += strlen(num_str);
    free(num_str);
    node->number = number;
    node->left = NULL;
    node->right = NULL;
    return node;
  } 
  else if (node->type == 'v') {     
    char * id_extract = malloc(21 * sizeof(char)); 
    int counter = 0;
    while(s[string_pos] != '\0' && s[string_pos] != ' ' 
          && s[string_pos] != ')') {
      id_extract[counter] = s[string_pos];
      string_pos++;
      counter++;
    }
    id_extract[counter] = '\0';
    strcpy(node->id, id_extract);
    node->right = NULL;
    node->left = NULL;
    free(id_extract);
    return node;
  }
  else {
    string_pos++;
    node->left = string_to_enode(s); 
    node->right = string_to_enode(s);
    return node;
  }
}

struct rexp *string_to_rexp(const char *s) {
  struct rexp * expression = malloc(sizeof(struct rexp));
  expression->root = string_to_enode(s);
  return expression;
}

int eval_nodes(struct enode * node, const struct dictionary *constants) {
  if(node->type == 'n') {
    return node->number;
  }
  else if(node->type == 'v') {
    int ret_val = dict_lookup(node->id, constants);
    return ret_val;
  }
  else if(node->type == '+') {
    return eval_nodes(node->left, constants) + 
      eval_nodes(node->right, constants);
  }
  else if(node->type == '-') {
    return eval_nodes(node->left, constants) - 
      eval_nodes(node->right, constants);
  }
  else if(node->type == '*') {
    return eval_nodes(node->left, constants) * 
      eval_nodes(node->right, constants);
  }
  else if(node->type == '/') {
    return eval_nodes(node->left, constants) 
      / eval_nodes(node->right, constants);
  }
  return 0;
}

int rexp_eval(const struct rexp *r, const struct dictionary *constants) {
  string_pos = 0;
  return eval_nodes(r->root, constants);
}

void enode_destroy(struct enode * node) {
  if(node == NULL) {
    return;
  }
  enode_destroy(node->left);
  enode_destroy(node->right);
  free(node);
}

void rexp_destroy(struct rexp *r) {
  enode_destroy(r->root);
  free(r);
}


void add_constant(const char *s, struct dictionary *constants) {
  int start_pos = 8; 
  char * key = malloc(21 * sizeof(char));
  sscanf(s + start_pos, "%s", key);
  string_pos = start_pos + strlen(key);
  struct rexp * const_exp = string_to_rexp(s);
  int value = rexp_eval(const_exp, constants);
  dict_add(key, value, constants);
  string_pos = 0;
  free(key);
  rexp_destroy(const_exp);
}



void print_enode(const struct enode *e) {
  assert(e);
  if (e->type == 'n') {
    printf("%d", e->number);
  } else if (e->type == 'v') {
    printf("%s", e->id);
  } else {
    printf("(%c ", e->type);
    print_enode(e->left);
    printf(" ");
    print_enode(e->right);
    printf(")");
  }
}

void print_rexp(const struct rexp *r) {
  assert(r);
  print_enode(r->root);
  printf("\n");
}
