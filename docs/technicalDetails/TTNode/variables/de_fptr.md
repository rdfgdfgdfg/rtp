# `Fptr de_fptr;`

|特征|内容
|---|---
|用途|存放回调函数指针
|性质|当`de_fptr`不为`nullptr`时，且`nodeC`为空时，`fptr`会变为`de_fptr`，同时`de_fptr`变为`nullptr`
|初始化|刚初始化时，为`nullptr`
|可访问性|`private`