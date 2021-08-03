# `void tryCreateThread();`

|特征|内容
|---|---
|线程安全性|False
|用途|循环创建系统线程
|条件|系统线程数小于可执行节点数，且系统线程数小于最大系统线程数
|引用|创建`run`线程；访问`runableNodeSize`，访问`maxThreadsSize`，读写`threads`
|可访问性|`private`