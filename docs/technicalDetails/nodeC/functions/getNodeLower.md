# `TTNode* getNodeLower();`

|特征|内容
|---|---
|线程安全性|False
|用途|反复执行该函数可以达到遍历的效果
|返回值|若返回值不为`nullptr`，表示`run`下一个要执行的线程节点
|条件|list不为空
|引用|调用`empty`，调用`next`，调用`fptrNULL`；访问`activeIt`
|可访问性|`private`
-----

原理
该函数会依次移位并访问`activeIt`：访问`this->activeIt`，然后是`(\*this->activeIt)->nodeC.activeIt`
然后是`(\*(\*this->activeIt)->nodeC.activeIt)->nodeC.activeIt`以此类推（每次访问前都会next)

