## Reference

Computation of the eccentricity of all nodes of a connected undirected unweighted graph as described in
Takes, F.W.; Kosters, W.A. Computing the Eccentricity Distribution of Large Graphs. Algorithms 2013, 6, 100-118.

## To compile

gcc eccentricity.c -o eccentricity -O9

## To execute

./eccentricity edgelist > result
- "edgelist" should contain an undirected unweighted connected graph: one edge on each line (two unsigned 32 bits int (ID of the 2 nodes) separated by a space).
- "result" will contain two unsigned int that correspond to the ID of a node and its eccentricity

# Example

Execute using example.graph which is the graph from the reference cited above (A=0, B=1, L=11):

```
$ ./eccentricity example.graph
12 nodes 13 edges
0 5
1 3
5 4
2 4
3 6
4 6
7 5
8 5
6 3
9 4
10 4
11 6
12 nodes 6 steps
```
