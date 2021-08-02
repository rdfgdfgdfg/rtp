# `std::list<`[TTNode](technicalDetails/TTNode/README)`*>::iterator activeIt`

|特征|内容
|---|---
|用途|存放下一个被访问的节点
|性质|节点容器的迭代器
|初始化|刚初始化时，为非法迭代器
|可访问性|`private`
-----

原理
该函数会依次移位并访问activeIt：访问this->activeIt，然后是(\*this->activeIt)->nodeC.activeIt
然后是(\*(\*this->activeIt)->nodeC.activeIt)->nodeC.activeIt以此类推（每次访问前都会next)
