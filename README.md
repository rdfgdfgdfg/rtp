# rtp
Recursive thread pool. Use tree-structure to assign resource.

## where to use
1.Very large and complex problems that need recursive algorithm.  
2.Have to use multithreading.  
3.In a thread, each child-thread should be assigned equal sources.  
4.Traversing one by one will waste a lot of time.  
