There are so many ways to implement different things.

### Query
Decision?
- SQCM for things like sparql necessary.
- SQCM for things like geometries profitable (--> index)
- CQSM for object-queries for flexibility
	
#### Simple Query, Complex Module
- Query contains only query data, no logic (like "SELECT * WHERE { ... }")
- Module implements all the logic to answer the query

Pro:
- Module might implement special methods, indexing, ....

Con:
- restricted to queries specificated by the Module


#### Complex Query, Simple Module
- Query contains all the logic to select an object, implements a check:
	- void Query::consider(Object*)
	- e.g. geometric tests (compute area, pass if greater than a threshold)
- Module just needs to apply the query to every object

Pro:
- can implement custom query-algorithm without the need of a new module

Con:
- process object by object, no clever indexing etc.