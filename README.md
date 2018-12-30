# mysql-graphdb
Small set of C++ classes to store graphs in MySQL. Mapper between RDBMS and graph representation.  
Provides functions to persist graphs (CRUD) and very simple graph traversing functionalities.

Note: To use the library, it is necessary to have a working MySQL version installed and to have the connection data entered correctly in ```DBUtil.cpp```.

## Creating, clearing and deleting a database
mysql-graphdb creates a new mysql database for you. To do that, just call the ```GraphDB``` constructor and pass in a database name.
```cpp
GraphDB gdb("social_network"); // create DB if not exists and connect to it
```
You can also call the constructor again to reuse the database if it already exists.
To clear the database, i.e. drop the whole graph, you can call ```gdb.clearDB();```.

```cpp
gdb.clearDB(); // clear all graph data if exists
```
To delete the entire database (i.e., DROP DATABASE), it is possible to call ```gdb.deleteDB()```.

## Create nodes and edges
To create a node, just create a new ```Node```. The parameters required are the GraphDB instance and a (node) type as a string.
A node can also have any number of properties (key-value pairs), which are also defined as a string.

```cpp
// create node with type 'human' and properties 'name' and 'age'
Node peter(gdb, "human");
peter.setProperty("name", "Peter");
peter.setProperty("age", "29");
```

Similarly, connections between nodes can be made. Just create an edge object.

```cpp
Edge peter_knows_frieda(gdb, peter, frieda, "knows"); // create edge (peter)-[:knows]->(frieda)
peter_knows_frieda.setProperty("since", "yesterday");
```

## Traverser
The ```Traverser``` adds some very primitive graph operations. It is possible to **trace the graph using depth-first search**. Furthermore, **paths between two nodes** can be determined. The traverser must always be initialized with a start node.

```cpp
Traverser t(peter); // create traverser object with peter as a start node
```

### Depth-first search (DFS)
The result of the depth-first search is an object containing all the nodes and edges traversed (in the order in which they were reached).

```cpp
Result r = t.depthFirst();
vector<Node> nodes = r.getNodes(); // get all traversed nodes
vector<Edge> edges = r.getEdges(); // get all traversed edges
```

### Paths between two nodes
The result of the paths function is a ```vector<Track>```:
```cpp
vector<Track> vt = t.paths(frieda, heiko); // get path(s) from Frieda to Heiko
```

While a track is a sequence of ```GraphElement``` objects, i.e. nodes and edges:
```cpp
Track firstTrack = vt.front(); // get first track found
vector<GraphElement*> path = firstTrack.getPath(); // sequence of nodes and edges
```

### Traverser options
The traverser is capable to track e.g. only ```OUTGOING``` or ```INCOMING``` edges of a node. By default, the edge direction does not matter (```ALL```).
```cpp
t.setDirection(OUTGOING); // tell the traverser to track only outgoing edges
```

Another feature is to constrain the traverser to specific node or edge properties so that only nodes or edges that have the specified properties are traversed. The same applies to the node or edge type.

```cpp
t.addNodeType("human"); // only nodes of type 'human'
t.addEdgeType("knows"); // only edges of type 'knows'

t.addNodeProperty("name"); // only nodes with property 'name'
t.addEdgeProperty("since"); // only edges with property 'since'

t.addNodeProperty("age", "29"); // only nodes where property 'age' equals '29'
t.addEdgeProperty("since", "yesterday"); // only edges where property 'since' equals 'yesterday'
```

## Other useful stuff
Some capabilities that might - sometimes - be useful.

### GraphDB
The GraphDB class contains some 'convenience' functions.
#### Get all elements of a graph
For example there are functions to get all elements of a graph.
```cpp
gdb.getAllNodes(); // returns all nodes of the graph (could be very much)
gdb.getAllEdges(); // returns all edges of the graph ;-)
```

#### Get a node or an edge by its internal ID
Internally, every node and every edge has a unique ID. You can use this ID (int) to return the corresponding element.
```cpp
Node peter = gdb.getNodeById(1);
gdb.getEdgeById(...);
```

#### Get nodes or  edges based on their type or property
Really useful may be the functionality to have certain nodes or edges returned based on their type or property:
```cpp
vector<Node> onlyHumans = gdb.getTypedNodes("human"); // all nodes with type 'human'
vector<Edge> knowsEdges = gdb.getTypedEdges("knows");

gdb.getPropertyNodes("age"); // all nodes that own the property 'age'
gdb.getPropertyEdges("...");

gdb.getPropertyNodes("name", "Peter"); // all nodes where 'name' is 'Peter'
gdb.getPropertyEdges("...", "...");
```

### Node and Edge
Both the ```Node``` and the ```Edge``` class extend the ```GraphElement``` class that declares the following (pure) virtual functions:

#### Get property value by key
To get the value for a property key, just use the ```getProperty``` function:
```cpp
ele.getProperty("name");
```
#### Get all properties
To get all properties of a node/edge, call the ```getProperties``` function - it returns a map<string, string>.
#### Delete a property
To delete a property by its key, call the ```deleteProperty``` function:
```cpp
ele.deleteProperty("name");
```
#### Delete all properties
There is also the ```clearProperties``` function to delete all properties of a node/edge.

#### Set a property
To set a property:
```cpp
ele.setProperty("name", "Otto");
```
#### Delete a node or an edge
The ```remove``` function deletes a node or an edge. When deleting a node, the corresponding edges are also deleted.
```cpp
ele.remove();
```

#### Get edges of a node
The Node class has the ```getEdges``` function that returns ```ALL``` (default), only ```INCOMING``` or only ```OUTGOING``` edges of a certain node:
```cpp
node.getEdges(OUTGOING); // get all outgoing edges for that node
```

#### Get source and target node of an edge
The Edge class has functions that can return the adjacent nodes (source and target node) or their IDs:
```cpp
edge.getSourceId(); // return ID of the source (incoming) node
edge.getTargetId(); // return ID of the target (outgoing) node

edge.getSource(); // return Node object of the source node
edge.getTarget(); // return Node object of the target node
```
There are also setter functions for the mentioned fields. While the ```setSourceId```/```setTargetId``` function just changes the corresponding field (no persistent change in DB), the ```setSource```/```setTarget``` function actually saves (persists) the change of the node.
```cpp
edge.setSourceId(42); // temporarily sets the source ID to 42
edge.setSource(peter); // sets the source node of this edge (this change is saved to DB) where peter is a Node object
```

### Draw result graph
There is a functionality to draw the graph based on the ```Result``` object as PNG using DOT (GraphViz). It was implemented by a former fellow student of mine. There is just one line of code that does not work, unless you adjust the command and/or path in ```Result :: print```.
