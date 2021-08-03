# `class TTNode`

线程节点  
内部定义了`Fptr`用于存放用户函数  
内部定义了`Guard`锁用于保证线程安全，与`std::lock_guard`类似
