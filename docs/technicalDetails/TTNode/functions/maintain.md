# `void maintain();`

|特征|内容
|---|---
|线程安全性|False
|用途|维护状态
|条件|无
|引用|调用`erase`；读写`nodeC`，读写`de_fptr`，读写`fptr`，访问`belong`，访问`wrap`，修改`runableNodeSize`
|可访问性|`private`
-----

### 维护以下状态
* **不**存在`fptr`为`nullptr`的叶子节点
* **只**存在`de_fptr`为`nullptr`的叶子节点

### 为了维护以上状态，maintain会采取以下操作
* 将`de_fptr`赋给`fptr`，并将`de_fptr`设为`nullptr`
* 将节点移除