# `void erase(iterator it)`

|特征|内容
|---|---
|线程安全性|False
|用途|安全地删除list中的元素，不会使activeIt失效
|条件|it合法
|引用|调用`fptrNULL`；修改`activeIt`，访问`list`
|可访问性|`private`