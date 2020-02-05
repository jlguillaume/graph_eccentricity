#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>


/*
 * Code adapted from https://github.com/maxdan94/LoadGraph
 * No Licence
 * Adaptations: use of int32_t and 64 instead of ul and ull
 */

#define NLINKS 100000000 //maximum number of edges for memory allocation, will increase if needed

typedef struct {
  int32_t s;
  int32_t t;
} edge;

//edge list structure:
typedef struct {
  int32_t n; //number of nodes
  int64_t e; //number of edges
  edge     *edges; //list of edges
  int64_t *cd; //cumulative degree cd[0]=0 length=n+1
  int32_t *adj; //concatenated lists of neighbors of all nodes
} adjlist;

//compute the maximum of three int32_t
static inline int32_t max3(int32_t a, int32_t b, int32_t c){
  a = (a > b) ? a : b;
  return (a > c) ? a : c;
}

//compute the maximum of two int32_t
static inline int32_t max(int32_t a, int32_t b) {
  return (a > b) ? a : b;
}

//compute the minimum of two int32_t
static inline int32_t min(int32_t a, int32_t b) {
  return (a > b) ? b : a;
}

//reading the edgelist from file
adjlist* readedgelist(char* input){
  int64_t e1 = NLINKS;
  FILE *file = fopen(input, "r");

  adjlist *g = malloc(sizeof(adjlist));
  g->n = 0;
  g->e = 0;
  g->edges = malloc(e1 * sizeof(edge));//allocate some RAM to store edges

  while (fscanf(file, "%" SCNu32 "%" SCNu32, &(g->edges[g->e].s), &(g->edges[g->e].t)) == 2) {
    g->n = max3(g->n, g->edges[g->e]. s,g->edges[g->e].t);
    if (++(g->e) == e1) {//increase allocated RAM if needed
      e1 += NLINKS;
      g->edges = realloc(g->edges, e1 * sizeof(edge));
    }
  }
  fclose(file);

  g->n++;
  g->edges = realloc(g->edges, g->e * sizeof(edge));

  return g;
}

//building the adjacency matrix
void mkadjlist(adjlist* g){
  int32_t i, u, v;
  int32_t *d = calloc(g->n, sizeof(int32_t));

  for (i = 0; i < g->e; i++) {
    d[g->edges[i].s]++;
    d[g->edges[i].t]++;
  }

  g->cd = malloc((g->n + 1) * sizeof(int64_t));
  g->cd[0] = 0;
  for (i = 1; i < g->n + 1; i++) {
    g->cd[i] = g->cd[i - 1] + d[i - 1];
    d[i - 1] = 0;
  }

  g->adj = malloc(2 * g->e * sizeof(int32_t));

  for (i = 0; i < g->e; i++) {
    u = g->edges[i].s;
    v = g->edges[i].t;
    g->adj[g->cd[u] + d[u]++] = v;
    g->adj[g->cd[v] + d[v]++] = u;
  }

  free(d);
}


//freeing memory
void free_adjlist(adjlist *g){
  free(g->edges);
  free(g->cd);
  free(g->adj);
  free(g);
}

// Computes eccentricity of node in g. All distances are stored in d and eccentricity is returner
int32_t eccentricity(adjlist *g, int32_t node, int32_t *d) {
  static int32_t *queue = NULL;
  if (queue == NULL)
    queue = malloc(g->n * sizeof(int32_t));
  queue[0] = node;
  


  int32_t b = 0;
  int32_t t = 1;
  
  memset(d, 0, g->n * sizeof(int32_t));
  d[node] = 0;

  int32_t maxDistance = 0;
  while (b < t) {
    int32_t c = queue[b++]; 
    for (int64_t i = g->cd[c]; i < g->cd[c + 1]; i += 1) {
      int32_t neigh = g->adj[i];
      if (d[neigh] == 0) {
	d[neigh] = d[c] + 1;
	maxDistance = max(maxDistance, d[neigh]);
	queue[t++] = neigh;
      }
    }
  }

  return maxDistance;
}


int main(int argc,char** argv){
  adjlist* g;

  // Read graph
  g = readedgelist(argv[1]);
  mkadjlist(g);

  fprintf(stderr,"%" PRIu32 " nodes %" PRIu64 " edges\n", g->n, g->e);
  
  // Allocate initial values, upper and lower bounds
  int32_t *e = calloc(g->n, sizeof(int32_t));
  int32_t *el = calloc(g->n, sizeof(int32_t));
  int32_t *eu = calloc(g->n, sizeof(int32_t));
  int32_t *d = calloc(g->n, sizeof(int32_t));
  for (int32_t i = 0; i < g->n; i += 1) {
    eu[i] = g->n;
  }

  // Main loop
  uint32_t nb = 0;
  for (int32_t i = 0; i < g->n; i += 1) {
    if (e[i] == 0) {
      e[i] = eccentricity(g, i, d);
      printf("%" PRIu32 " %" PRIu32 "\n", i, e[i]); 
      nb ++;

      for (int32_t j = 0; j < g->n; j += 1) {
	if (e[j] == 0) {
	  el[j] = max3(el[j], e[i] - d[j], d[j]);
	  eu[j] = min(eu[j], e[i] + d[j]);
	  if (el[j] == eu[j]) {
	    e[j] = el[j];
	    printf("%" PRIu32 " %" PRIu32 "\n", j, e[j]); 
	  }
	}
      }
    }
  }
  
  fprintf(stderr,"%" PRIu32 " nodes %" PRIu32 " steps\n", g->n, nb);

  free_adjlist(g);
}

