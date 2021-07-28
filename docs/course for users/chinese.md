# 用户教程
## 配置
要想使用rtp库，你需要确保你的编译器找得到rtp/src/ThreadTree.h并include  
最简单的方法是include<绝对路径>  
编译时，由于系统调度问题，解决方案配置必须是release版本，否则可能会引发错误  
如果你的解决方案版本必须是debug，你可以将库以release版本编译成lib再调用  
  
## 理论  
递归线程池管理线程，并且可以通过递归结构合理分配资源  
rtp库将被线程池管理的线程称为线程节点  
线程节点不会被系统直接执行，递归线程池会决定该执行哪一个线程节点  
  
### 递归线程池
递归线程池有两种状态：启动，未启动
未启动状态下所有属于该池的线程节点都不会自动启动，一般在该状态下由外部向池添加初始线程节点  
  
启动状态下所有属于该池的线程节点都会合理地自动启动，这时也可以向池添加线程节点。
但是一般是由线程节点内部添加（线程节点内部操作时，记得加锁）。
因为从外部添加时，池还在运行。可能还没有添加完，池就运行完了  
池在运行完后，状态为未启动  
  
### 线程节点
线程节点是对线程的抽象，逻辑上可以当作线程使用  
线程节点有2个函数指针，分别为fptr和de_fptr，类型为Fptr,即void (TTNode::\*)() 即TTNode中声明的void \*();  
还有一个存放线程节点的容器nodeC，执行该线程节点时创建的线程会被放入nodeC中  
其中fptr会被执行，de_fptr会在线程节点的nodeC为空时，覆盖fptr并执行(前提是de_fptr不是nullptr)  
若fptr为nullptr，此线程节点不会被执行  
  
由于fptr和de_fptr的类型为void (TTNode::\*)()，所以用户函数要在TTNode中声明————这显然是不可能的  
我建议你派生TTNode类，在派生类中写代码，然后将函数指针赋给fptr  
**如果线程节点的fptr和de_fptr都为nullptr，且nodeC为空，那么它会被移除（在nodeC中的线程节点被erase）（不析构）。**
  
## 代码   
```c++
Class UserNode : public MAT::TTNode{//派生TTNode类
	void foo(){......}//用户函数
	UserNode(MAT::TThread* belong, ......user def) : MAT::TTNode(belong) {
		fptr = static_cast<Fptr>(&UserNode::foo);//fptr是必须赋值的，否则该线程节点没什么意义
		......//de_fptr是否赋值要根据实际情况
	}
	UserNode(MAT::TTNode* wrap, ......user def) : MAT::TTNode(wrap) {......}
}

int main(){
	MAT::TThread tth;//创建递归线程池
	UserNode a(&tth);//创建线程节点
	UserNode b(&a);//&b在a的nodeC内
	tth.start();
	tth.join();
}
```


