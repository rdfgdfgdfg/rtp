# `std::list<`[TTNode](technicalDetails/TTNode/README)`*> list`

|特征|内容
|---|---
|用途|储存[TTNode](technicalDetails/TTNode/README)的指针的链表，内部的节点指针将会被getNodeLower访问并迭代。链表是否为空是maintain判断是否移除（不析构）该节点指针的依据之一
|性质|若用户函数创建了其他线程节点。它们将被加入到此链表中
|初始化|刚初始化时，链表为空。
|可访问性|`private`