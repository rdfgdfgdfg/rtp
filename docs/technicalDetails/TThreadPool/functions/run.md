# `void run(std::list<std::thread*>::iterator it);`

|特征|内容
|---|---
|线程安全性|True
|用途|执行线程节点
|条件|无
|引用|调用`tryDeleteThread`，调用`getNodeLower`；取址`nodeC`，修改`running`，操作`changeList`，执行`fptr`
|可访问性|`private`
-----

这是核心代码，原理见[算法](algorithm)
