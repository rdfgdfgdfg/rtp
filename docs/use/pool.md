# 线程池  `MAT::TThreadPool`  
|函数 | 用途 | 备注  
|---|---|---
|构造函数 : 默认 |
|`void start(UINT)` | 开始线程池并设定线程最大数
|`void join()` | 阻塞
|`nlohmann::json getJson()` | 输出调试信息 | 定义`THREADTREE_DEBUG`宏后才可以使用  
-----
若想向线程池中增加线程，见[线程节点](/use/node)
