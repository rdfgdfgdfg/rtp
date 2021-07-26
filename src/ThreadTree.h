#pragma once

#include <list>
#include <thread>
#include <mutex>

namespace MAT {
	using size_c = size_t;

	class TThread;//线程池
	class TTNode;//线程节点
	class NodeC;//线程容器



	constexpr char TTHREAD_KEEP = '\000';
	constexpr char TTHREAD_STOP = '\001';
	constexpr char TTHREAD_NEXT = '\002';


	class NodeC {
		friend class TThread;
	public:
		struct pos;//用于存放getNodeLower返回值的表示位置的结构体
		using iterator = std::list<TTNode*>::iterator;
	private:
		/*
		push_back可添加；getNodeLower可访问，可调用erase删除；empty next可访问
		存放由用户函数创建的线程节点。内部的节点指针将会被getNodeLower访问并迭代
		刚初始化时，链表为空。
		链表是否为空是getNodeLower判断是否移除（不析构）该节点指针的依据之一
		*/
		std::list<TTNode*> list;//若fptr创建了其他线程节点。它们将被加入到此迭代器中


		/*
		节点容器的迭代器
		getNodeLower可访问；next erase可访问并修改
		存放将要被getNodeLower迭代的线程节点
		刚初始化时，迭代器为空
		*/
		std::list<TTNode*>::iterator activeIt;//节点容器的迭代器，指向下一个应被执行的线程节点。


		/*
		线程不安全
		执行此函数需要确保list不为空
		修改activeIt，读取list
		该函数会移动activeIt，起到遍历list的作用
		建议先next，再访问activeIt
		*/
		void next();//activeIt移位

		/*安全地删除list中的元素，不会使activeIt失效*/
		void erase(iterator it);//线程不安全
	public:

		void push_back(TTNode* ptr);//线程不安全

		/*
		线程不安全
		执行此函数需要确保list不为空
		访问activeIt，执行erase，empty
		该函数会依次移位并访问activeIt：访问this->activeIt，然后是(*this->activeIt)->nodeC.activeIt
		然后是(*(*this->activeIt)->nodeC.activeIt)->nodeC.activeIt以此类推（每次访问前都会next)
		该函数会在得到可执行且!running的线程节点时终止，否则，它会一直运行到nodeC.empty()为止
		如果发现不可执行且nodeC为空的节点，它会将它的fptr更改为de_fptr，若de_fptr为nullptr它会将其删除（析构）
		反复执行该函数可以达到遍历的效果
		*/
		pos getNodeLower();//指向属于此节点容器的最近可执行节点（线程不安全）

		bool empty();//线程不安全

		using iterator = std::list<TTNode*>::iterator;

		struct pos {//储存getNodeLower的返回值
			NodeC* ptr;//it是ptr的迭代器。若getNodeLower执行失败，ptr是nullptr
			NodeC::iterator it; //*it指向属于此节点容器的最近可执行节点.
		};

		NodeC() :activeIt(list.end()) {};
	};

	/*
	delete情况：
	fptr返回TTHREAD_STOP，且nodeC.empty()时
	getNodeLower发现 fptr==nullptr且de_fptr==nullptr且nodeC.empty 时
	*/
	class TTNode {//此类线程安全
	public:
		using Fptr = void (TTNode::*)();//用户函数指针
		friend class NodeC;
	protected:
		//初始化为nullptr，表示不要试图执行该线程节点。
		Fptr fptr;//用户函数，会被run调用
		Fptr de_fptr;//当fptr为nullptr时，且nodeC为空时，fptr会变为de_fptr
	private:
		TThread* const belong;//包含此节点的线程池
		
		//该线程节点是否正在被执行。初始化为false
		bool running;

		NodeC nodeC;

		friend class TThread;


	public:
		TTNode(TThread* belong);
		TTNode(TTNode* wrap);
		virtual ~TTNode() {};


		TTNode() = delete;
		TTNode(const TTNode&) = delete;
		TTNode(const TTNode&&) = delete;

	};


	class TThread {
	public://test
		NodeC nodeC;
		size_c maxThreadsSize;//最大线程数量
		size_c size;//节点数量
		std::list<std::thread*> threads;
		std::thread* forDelete;
		std::mutex changeList;

		void tryCreateThread();//尝试创建线程（线程不安全）
		bool tryDeleteThread(std::list<std::thread*>::iterator it);//尝试删除线程（线程不安全）
	public:
		friend class TTNode;

		TThread();
		~TThread();
		void join();
		void start();//开始线程池。
		void stop();//
		void setMaxThreadsSize();

		void run(std::list<std::thread*>::iterator it);

	};

	

	

}

#include "ThreadTree/NodeC-func.h"
#include "ThreadTree/TTNode-func.h"
#include "ThreadTree/TThread-func.h"
