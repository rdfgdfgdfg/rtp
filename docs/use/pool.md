# 线程池  `MAT::TThreadPool`  
|函数 | 用途 | 备注  
|---|---|---
|构造函数 : 默认 |
|`void setMaxThreadsSize(UINT)` | 设置线程池中能同时运行的系统线程数（最大值） | 立即生效，可以用来开始和停止线程池  
|`nlohmann::json getJson()` | 输出调试信息 | 定义`THREADTREE_DEBUG`宏后才可以使用  
-----
若想向线程池中增加线程，见[线程节点](/use/node)
