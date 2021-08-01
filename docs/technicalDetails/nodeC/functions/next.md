# `void next();`

|特征|内容
|---|---
|线程安全性|False
|用途|该函数会移动[activeIt](technicalDetails/nodeC/variables/activeIt)，起到遍历list的作用
|条件|执行此函数需要确保[list](technicalDetails/nodeC/variables/list)不为空
|使用建议|在访问[activeIt](technicalDetails/nodeC/variables/activeIt)前调用
|引用|修改[activeIt](technicalDetails/nodeC/variables/activeIt)，访问[list](technicalDetails/nodeC/variables/list)
|可访问性|`private`