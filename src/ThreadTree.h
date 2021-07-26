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
		std::list<TTNode*> list;//若fptr创建了其他线程节点。它们将被加入到此迭代器中
		std::list<TTNode*>::iterator activeIt;//节点容器的迭代器，指向下一个应被执行的线程节点。

		void next();//activeIt移位
	public:
		struct pos;
		using iterator = std::list<TTNode*>::iterator;

		inline void push_back(TTNode* ptr) { list.push_back(ptr); }//线程不安全

		//获得TTNode节点的指针
		pos getNodeLower();//指向属于此节点容器的最近可执行节点（线程不安全）

		bool empty();//线程不安全

		void erase(iterator it);//线程不安全


		using iterator = std::list<TTNode*>::iterator;

		struct pos {//储存getNodeLower的返回值
			NodeC* ptr;//it是ptr的迭代器。若getNodeLower执行失败，ptr是nullptr
			NodeC::iterator it; //*it指向属于此节点容器的最近可执行节点.
		};
	};

	class TTNode {//此类线程安全
	public:
		using Fptr = char (TTNode::*)();//用户函数指针
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
		~TTNode();


		TTNode() = delete;
		TTNode(const TTNode&) = delete;
		TTNode(const TTNode&&) = delete;

	};


	class TThread {

		NodeC nodeC;
		size_c size;//节点数量
		std::list<std::thread*> threads;
		std::thread* forDelete;
		std::mutex changeList;

		void tryCreateThread();//尝试创建线程（线程不安全）
		bool tryDeleteThread(std::thread* ptr);//尝试删除线程（线程不安全）
	public:
		friend class TTNode;

		TThread();
		~TThread();
		void join();
		void start();//开始线程池。
		void stop();//

		void run(std::thread* ptr);

	};

	

	

}

#include "ThreadTree/NodeC-func.h"
#include "ThreadTree/TTNode-func.h"
#include "ThreadTree/TThread-func.h"
