# `MAT::nodeC`简介  
nodeC是储存线程节点的类，与[TThreadPool](technicalDetails/TThreadPool/README.md)和[TTNode](technicalDetails/TTNode/README.md)是友元  
内部定义了`struct pos`和`iterator`类   

线程不安全  

|内部类型|意义
|---|---
|`iterator`|表示`list`的迭代器