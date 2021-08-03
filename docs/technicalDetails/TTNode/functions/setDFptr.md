# `template <class T> void setDFptr(T ptr);`

|特征|内容
|---|---
|线程安全性|True
|用途|表示此节点执行完毕，设定回调函数，等待子节点执行完毕，执行完毕后执行回调函数
|条件|设定的回调函数的类型必须可以转换为`Fptr`，即满足**用户函数**的条件
|引用|调用`lock`，调用`unlock`，调用`maintain`；读写`fptr`，修改`de_fptr`，访问`wrap`，修改`runableNodeSize`
|可访问性|`protected`