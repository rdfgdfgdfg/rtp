# `bool tryDeleteThread(`
# `std::list<std::thread*>::iterator it`
# `);`

|特征|内容
|---|---
|线程安全性|False
|用途|循环移除并析构上一个被移除的系统线程
|条件|只能移除`list`中的线程，建议不要在除run之外的地方调用
|引用|访问`runableNodeSize`，读写`threads`，读写`forDelete`
|可访问性|`private`
-----

### Q:为什么要将被移除的线程指针写入`forDelete`中，在下一次调用时析构？
A:线程不能析构自己，为了避免内存泄露，必须让别的线程释放线程指针，这就是forDelete的用途。它可以将析构的操作放入下一个线程中。
（`~TThreadPool`也会析构`forDelete`）