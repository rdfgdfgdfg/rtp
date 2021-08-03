# `void setMaxThreadsSize(size_c size);`

|特征|内容
|---|---
|线程安全性|True
|用途|设定maxThreadsSize并使其立即生效
|条件|无
|引用|调用`tryCreateThread`；修改`maxThreadsSize`，操作`changeList`
|可访问性|`public`