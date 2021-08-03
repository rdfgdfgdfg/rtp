# `void setOver();`

|特征|内容
|---|---
|线程安全性|True
|用途|表示此节点彻底执行完毕
|条件|无
|引用|调用`lock`，调用`unlock`，调用`maintain`；读写`fptr`，访问`wrap`，修改`runableNodeSize`
|可访问性|`protected`