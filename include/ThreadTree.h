#pragma once

#include <list>
#include <thread>
#include <mutex>

#ifdef THREADTREE_DEBUG
//#include <json.hpp> 
//from https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent/tree/master/include/nlohmann/json.hpp
//project : nlohmann/json
#endif

namespace MAT {
	using size_c = size_t;

	class TThreadPool;//线程池
	class TTNode;//线程节点
	class NodeC;//线程容器



	constexpr char TTHREAD_KEEP = '\000';
	constexpr char TTHREAD_STOP = '\001';
	constexpr char TTHREAD_NEXT = '\002';


	class NodeC {
		friend class TThreadPool;
		friend class TTNode;
	public:
		struct pos;//用于存放getNodeLower返回值的表示位置的结构体
		using iterator = std::list<TTNode*>::iterator;
	private:
		/*
		* push_back可添加；getNodeLower可访问，可调用erase删除；empty next可访问
		* 存放由用户函数创建的线程节点。内部的节点指针将会被getNodeLower访问并迭代
		* 刚初始化时，链表为空。
		* 链表是否为空是getNodeLower判断是否移除（不析构）该节点指针的依据之一
		*/
		std::list<TTNode*> list;//若fptr创建了其他线程节点。它们将被加入到此迭代器中


		/*
		* 节点容器的迭代器
		* getNodeLower可访问；next erase可访问并修改
		* 存放将要被getNodeLower迭代的线程节点
		* 刚初始化时，迭代器为空
		*/
		std::list<TTNode*>::iterator activeIt;//节点容器的迭代器，指向下一个应被执行的线程节点。


		/*
		* 线程不安全
		* 执行此函数需要确保list不为空
		* 修改activeIt，读取list
		* 该函数会移动activeIt，起到遍历list的作用
		* 建议先next，再访问activeIt
		*/
		void next();//activeIt移位

		/*安全地删除list中的元素，不会使activeIt失效*/
		void erase(iterator it);//线程不安全

		void push_back(TTNode* ptr);//线程不安全

	public:


		/*
		* 线程不安全
		* 执行此函数需要确保list不为空
		* 访问activeIt，执行erase，empty
		* 该函数会依次移位并访问activeIt：访问this->activeIt，然后是(*this->activeIt)->nodeC.activeIt
		然后是(*(*this->activeIt)->nodeC.activeIt)->nodeC.activeIt以此类推（每次访问前都会next)
		* 该函数会在得到可执行且!running的线程节点时终止，否则，它会一直运行到nodeC.empty()为止
		* 它会维护所有迭代到的节点的状态
		* 反复执行该函数可以达到遍历的效果
		*/
		pos getNodeLower();//指向属于此节点容器的最近可执行节点（线程不安全）


		bool empty();//线程不安全
		iterator begin();
		iterator end();


		using iterator = std::list<TTNode*>::iterator;

		struct pos {//储存getNodeLower的返回值
			NodeC* ptr;//it是ptr的迭代器。若getNodeLower执行失败，ptr是nullptr
			NodeC::iterator it; //*it指向属于此节点容器的最近可执行节点.
		};

		NodeC() :activeIt(list.end()) {};
#ifdef THREADTREE_DEBUG
		json getJson();
#endif

	};

	class TThreadPool {
		friend class TTNode;
		friend class NodeC;
		friend class guard;
	public://test

		NodeC nodeC;
		size_c maxThreadsSize;//最大线程数量
		size_c size;//节点数量
		std::list<std::thread*> threads;
		std::thread* forDelete;

		void tryCreateThread();//尝试创建线程（线程不安全）
		bool tryDeleteThread(std::list<std::thread*>::iterator it);//尝试删除线程（线程不安全）
	protected:
		std::mutex changeList;
	public:

		TThreadPool();

		TThreadPool(const TThreadPool&) = delete;
		TThreadPool(const TThreadPool&&) = delete;

		~TThreadPool();
		void join();
		void start();//开始线程池。
		void setMaxThreadsSize();

		void run(std::list<std::thread*>::iterator it);
#ifdef THREADTREE_DEBUG
		json getJson();
#endif
	};

	class TTNode {
	public:
		using Fptr = void (TTNode::*)();//用户函数指针
		friend class NodeC;
		friend class TThreadPool;

		class Guard {
			friend class TTNode;
			std::mutex* ptr;
			inline Guard(std::mutex* ptr) : ptr(ptr) {
				ptr->lock();
			}
			inline Guard(Guard&& guard) : ptr(guard.ptr) {
				guard.ptr = nullptr;
			}
		public:
			inline ~Guard() {
				if (ptr != nullptr) {
					ptr->unlock();
				}
			}

		};
	protected:
		//初始化为nullptr，表示不要试图执行该线程节点。
		Fptr fptr;//用户函数，会被run调用
		Fptr de_fptr;//当fptr为nullptr时，且nodeC为空时，fptr会变为de_fptr

		TTNode(TThreadPool* belong);//线程不安全

		/*尽管TTNode会自动在nodeC中存储你创建的线程节点，但由于TTNode会自动移除运行完毕且nodeC为空的节点，
		你需要保存它的指针防止内存泄露 */
		TTNode(TTNode* wrap);//线程不安全

		~TTNode() {};

		void lock();
		void unlock();
		Guard getGuard();

	private:

		TThreadPool* const belong;//包含此节点的线程池
		
		//该线程节点是否正在被执行。初始化为false
		bool running;

		NodeC nodeC;


		/*
		* 线程不安全
		* 在适当的时候将 de_fptr 赋给 fptr
		确保 fptr != nullptr 或 nodeC.empty()
		在节点无效时移除自己（return true)（不会析构）
		*/
		bool maintain(NodeC::iterator it, NodeC* ptr);//维护状态

		bool fptrNULL();
	public:
		

		TTNode() = delete;
		TTNode(const TTNode&) = delete;
		TTNode(const TTNode&&) = delete;

#ifdef THREADTREE_DEBUG
		json getJson();//获取调试信息
#endif
	};


	

	

}

#include "ThreadTree/NodeC-func.h"
#include "ThreadTree/TTNode-func.h"
#include "ThreadTree/TThreadPool-func.h"
#ifdef THREADTREE_DEBUG
#include "../test/common/debug.h"
#endif