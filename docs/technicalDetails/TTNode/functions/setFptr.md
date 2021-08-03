# `template <class T> void setFptr(T ptr);`

|特征|内容
|---|---
|线程安全性|True
|用途|设定待执行函数
|条件|定的待执行函数的类型必须可以转换为`Fptr`，即满足**用户函数**的条件
|引用|调用`lock`，调用`unlock`；读写`fptr`，访问`wrap`，修改`runableNodeSize`
|可访问性|`protected`