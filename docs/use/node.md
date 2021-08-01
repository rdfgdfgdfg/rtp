# 线程节点 `MAT::TTNode`  
**彻底运行完毕后会被移除（不析构）**  
要想使线程节点有实际意义（让线程节点运行用户代码），需要`public`继承`MAT::TTNode`类  
继承时，有以下函数需要实现  

| 函数 | 特征 | 行为  
|---|---|---
| 构造函数`UserThread(TThreadPool* wrap,......)` | 必须在初始化列表中包含`MAT::TTNode(wrap, 用户函数)`，且函数体中不允许出现任何加锁行为或调用线程安全的函数 | 初始化用户线程类，并将本对象直接加入线程池下
| 构造函数`UserThread(TThreadPool* belong,......)` | 必须在初始化列表中包含`MAT::TTNode(belong, 用户函数)`，且函数体中不允许出现任何加锁行为 | 初始化用户线程类，并将本对象加入belong下
| 用户函数`void userfunc()` | 需要被多线程执行，如果你要在用户函数创建更多线程节点，你需要加锁。你还需要在return前调用setFptr或setDFptr来指定下一次被执行的函数，若不指定，下次执行时还会执行同一个函数 | 被系统函数执行 
----- 
**在用户函数中创建线程节点前，必须lock或getGuard**  
  
  
-----
下面将介绍`MAT::TTNode`类中以下函数的用途  
  
|函数 | 用途 | 备注  
|---|---|---
|`template <class T> void TTNode::setFptr(T ptr)`| 设定新的待执行的用户函数 | 只能在用户函数中执行，线程安全，必须确保传入的线程指针可执行且能转换为Fptr。逻辑上，此函数后应当紧跟return
|`template <class T> void TTNode::setDFptr(T ptr)`|表示此函数节点执行完毕，设定在子线程节点全部执行完毕时运行的用户函数 | 子线程节点全部执行完毕前，系统线程会直接跳过本线程节点。只能在用户函数中执行，线程安全，必须确保传入的线程指针可执行且能转换为Fptr。逻辑上，此函数后应当紧跟return
|`void setOver()` | 表示此函数节点彻底执行完毕，需要被尽快移除 | 只能在用户函数中执行，线程安全。逻辑上，此函数后应当紧跟return
|`void lock()` | 锁定线程池 | 具体用法类似`std::mutex::lock`，在unlock前不可以执行setFptr，setDFptr，setOver。
|`void unlock()`| 解锁线程池 | 具体用法类似`std::mutex::unlock`，必须在lock后，返回前执行
|`Guard getGuard()`|获得锁 | 具体用法类似`std::lock_guard`
-----
下面将介绍`MAT::TTNode`类中可访问成员变量（常量）  

|变量 | 含义
|---|---
|`TThreadPool* const wrap`|拥有此线程节点的线程池
|`TTNode* const belong`|包含此线程节点的线程节点，可能为`nullptr`