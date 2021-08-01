#pragma once

#include <list>
#include <thread>
#include <mutex>
#include <chrono>

#ifdef THREADTREE_DEBUG
#include <json.hpp> 
//from https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent/tree/master/include/nlohmann/json.hpp
//project : nlohmann/json
#endif

namespace MAT {
#ifdef THREADTREE_DEBUG
	using json = nlohmann::json;
#endif

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
#ifdef THREADTREE_DEBUG
	public:
#endif
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
#ifdef THREADTREE_DEBUG
	public:
#endif

		NodeC nodeC;
		size_c maxThreadsSize;//最大线程数量
		size_c runableNodeSize;//节点数量
		std::list<std::thread*> threads;
		std::thread* forDelete;

		void tryCreateThread();//尝试创建线程（线程不安全）
		bool tryDeleteThread(std::list<std::thread*>::iterator it);//尝试删除线程（线程不安全）

		void run(std::list<std::thread*>::iterator it);
	protected:
		std::mutex changeList;
	public:

		TThreadPool();

		TThreadPool(const TThreadPool&) = delete;
		TThreadPool(const TThreadPool&&) = delete;

		~TThreadPool();
		void join();
		void setMaxThreadsSize(size_c size);
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
#ifdef THREADTREE_DEBUG
	public:
#endif
		template<class T>
		TTNode(TThreadPool* wrap, T fptr);//线程不安全

		/*尽管TTNode会自动在nodeC中存储你创建的线程节点，但由于TTNode会自动移除运行完毕且nodeC为空的节点，
		你需要保存它的指针防止内存泄露 */
		template<class T>
		TTNode(TTNode* belong, T fptr);//线程不安全

		~TTNode() {};

		void lock();
		void unlock();

		template <class T> void setFptr(T ptr);//设置下一个要运行的函数（必须可执行）
		template <class T> void setDFptr(T ptr);//运行结束，等待子线程节点运行，设置子线程节点运行结束后运行的函数（必须可执行）
		void setOver();//彻底结束，此节点无效化
		Guard getGuard();

		TThreadPool* const wrap;//包含此节点的线程池
		TTNode* const belong;//包含此节点的线程节点
	private:
#ifdef THREADTREE_DEBUG
	public:
#endif
		//初始化为nullptr，表示不要试图执行该线程节点。
		Fptr fptr;//用户函数，会被run调用
		Fptr de_fptr;//当fptr为nullptr时，且nodeC为空时，fptr会变为de_fptr

		//该线程节点是否正在被执行。初始化为false
		bool running;

		NodeC nodeC;
		NodeC::iterator it;

		/*
		* 线程不安全
		* 在树结构被改变时运行 如erase setOver setDFptr 
		* 任务：
		* 1.若nodeC为空，将本节点的de_fptr赋值到fptr
		* 2.若nodeC为空且本节点的de_fptr为nullptr，移除本节点
		*/
		void maintain();//维护状态
		bool runable();

		bool fptrNULL();
	public:
		

		TTNode() = delete;
		TTNode(const TTNode&) = delete;
		TTNode(const TTNode&&) = delete;

#ifdef THREADTREE_DEBUG
		json getJson();//获取调试信息
#endif
	};

//-----------------------------------------------------------------
//NodeC-func

	inline void NodeC::next() {
		if (activeIt == --list.end()) {
			activeIt = list.begin();
		}
		else {
			activeIt++;
		}
	}

	inline bool NodeC::empty() {
		return list.empty();
	}

	inline void NodeC::push_back(TTNode* ptr) {
		if (list.empty()) {
			list.push_back(ptr);
			activeIt = list.begin();
		}
		else {
			list.push_back(ptr);
		}
	}

	void NodeC::erase(iterator it) {
		TTNode* ptr = *it;
		TThreadPool* wrap = ptr->wrap;
		TTNode* belong = ptr->belong;

		if (!ptr->fptrNULL()) {
			wrap->runableNodeSize--;
		}
		
		if (activeIt == it) {
			if (activeIt == --list.end()) {//删除的线程在线程链末尾
				if (activeIt == list.begin()) {//线程池线程中只有待删除线程
					list.erase(it);
					activeIt._Ptr = nullptr;
				}
				else {//线程池当前可访问的线程中还有其他线程
					list.erase(it);
					activeIt = list.begin();
				}
			}
			else {
				activeIt++;
				list.erase(it);
			}
		}
		else {
			list.erase(it);
		}
		if (belong != nullptr) {
			belong->maintain();
		}
	}



	NodeC::pos NodeC::getNodeLower() {
		/*
		|
		_____________(rt.ptr指向容器,it是容器的迭代器)
		o ^ o ^ o ^ o(ptr是此层，指向节点的指针）
		|       |
		_____
		o ^ o
		
		*/
		pos rt;//返回值
		if (empty()) {
			rt.ptr = nullptr;
			return rt;
		}
		rt.ptr = this;
		next();
		rt.it = activeIt;

		TTNode* ptr = *rt.it;//要执行的节点的指针
		while (true) {//循环

			if (ptr->running | ptr->fptrNULL()) {
				if (ptr->nodeC.empty()) {
					rt.ptr = nullptr;//若节点正在被执行
					return rt;//报错
				}
				else {
					/*
|
_____________(rt.ptr指向容器,it是容器的迭代器)
o ^ o ^ o ^ o(ptr是此层，指向节点的指针）
|       |
_____(新的rt.ptr指向ptr的容器,it是新容器的迭代器)
o ^ o(新ptr是此层，指向节点的指针）

*/
					rt.ptr = &ptr->nodeC;
					rt.ptr->next();
					rt.it = rt.ptr->activeIt;
					ptr = *rt.it;
				}
			}
			else {
				return rt;
			}
			
		}
	}


//---------------------------------------------------------------------
//TThreadPool-func
	void TThreadPool::run(std::list<std::thread*>::iterator it) {
		NodeC::pos ndp;
		while (true) {//主循环
			this;
			NodeC* nodeCNow = &nodeC;//将要执行的节点的节点容器的指针
			TTNode* nodeNow = nullptr;//将要执行的节点的指针
			while (true) {//单链循环
				changeList.lock();
				if (tryDeleteThread(it)) {
					changeList.unlock();
					return;
				}
				ndp = nodeCNow->getNodeLower();//获取下一个节点的位置
				if (ndp.ptr != nullptr) {//检验合法性
					nodeNow = *ndp.it;//赋值
					nodeCNow = &nodeNow->nodeC;
					nodeNow->running = true;
					//std::cout << std::endl << getJson() << std::endl;
					changeList.unlock();

					(nodeNow->*(nodeNow->fptr))();

					changeList.lock();
					nodeNow->running = false;//修改数值
					//std::cout << std::endl << getJson() << std::endl;
					//nodeNow->maintain(ndp.it, ndp.ptr);
					changeList.unlock();
				}
				else {
					changeList.unlock();
					break;
				}
			}
		}
	}

	inline void TThreadPool::tryCreateThread() {
		while ((runableNodeSize > threads.size()) & (threads.size() < maxThreadsSize)) {
			threads.push_back(nullptr);
			threads.back() = new std::thread(&TThreadPool::run, this, --threads.end());
		}
	}

	void TThreadPool::setMaxThreadsSize(size_c size) {
		changeList.lock();
		maxThreadsSize = size;
		tryCreateThread();
		changeList.unlock();
	}

	bool TThreadPool::tryDeleteThread(std::list<std::thread*>::iterator it) {
		if (runableNodeSize < threads.size()) {
			if (forDelete != nullptr) {
				while (forDelete->joinable());
			}
			delete forDelete;
			forDelete = *it;
			threads.erase(it);
			return true;
		}
		return false;
	}

	void TThreadPool::join() {
		while (true) {
			changeList.lock();
			if (threads.empty()) {
				changeList.unlock();
				return;
			}
			changeList.unlock();
			try {
				threads.front()->join();
			}
			catch (...) {}
		}
	}

	inline TThreadPool::TThreadPool(): maxThreadsSize(0), runableNodeSize(0), forDelete(nullptr) {}

	inline TThreadPool::~TThreadPool(){}

//---------------------------------------------------------------------
//TTNode-func
	template<class T>
	TTNode::TTNode(TThreadPool* wrap, T fptr) :
		wrap(wrap), belong(nullptr), fptr(static_cast<Fptr>(fptr)), de_fptr(nullptr), running(false) {
		wrap->nodeC.push_back(this);
		it = --wrap->nodeC.list.end();
		wrap->runableNodeSize++;
		wrap->tryCreateThread();
	};

	template<class T>
	TTNode::TTNode(TTNode* belong, T fptr) :
		wrap(belong->wrap), belong(belong), fptr(static_cast<Fptr>(fptr)), de_fptr(nullptr), running(false) {
		belong->nodeC.push_back(this);
		it = --belong->nodeC.list.end();
		wrap->runableNodeSize++;
		wrap->tryCreateThread();
	};

	inline bool TTNode::fptrNULL() {
		return fptr == nullptr;
	}

	void TTNode::maintain() {
		if (nodeC.empty()) {
			if (de_fptr == nullptr) {
				if (belong == nullptr) {
					wrap->nodeC.erase(it);
				}
				else {
					belong->nodeC.erase(it);
				}
			}
			else {
				fptr = de_fptr;
				de_fptr = nullptr;
				wrap->runableNodeSize++;
			}
		}
	}

	template <class T> inline void TTNode::setFptr(T ptr) {
#ifdef THREADTREE_DEBUG
		if (ptr == nullptr) {
			throw ptr;
		}
#endif
		lock();
		if (fptr == nullptr) {
			wrap->runableNodeSize++;
		}
		fptr = static_cast<Fptr>(ptr);
		unlock();
	}

	template <class T> inline void TTNode::setDFptr(T ptr) {
#ifdef THREADTREE_DEBUG
		if (ptr == nullptr) {
			throw ptr;
		}
#endif
		lock();
		if (fptr != nullptr) {
			wrap->runableNodeSize--;
		}
		de_fptr = static_cast<Fptr>(ptr);
		fptr = nullptr;
		maintain();
		unlock();
	}

	inline void TTNode::setOver() {
		lock();
		if (fptr != nullptr) {
			wrap->runableNodeSize--;
		}
		fptr = nullptr;
		maintain();
		unlock();
	}

	inline void TTNode::lock() {
		wrap->changeList.lock();
	}

	inline void TTNode::unlock() {
		wrap->changeList.unlock();
	}

	inline TTNode::Guard TTNode::getGuard() {
		return Guard(&wrap->changeList);
	}

//---------------------------------------------------------------------
//debug code
#ifdef THREADTREE_DEBUG
	std::string decPtr(void* ptr) {
		std::stringstream ss;
		ss.unsetf(std::ios_base::dec);
		ss << ptr;
		return ss.str();
	}


	json TThreadPool::getJson() {//输出调试信息
		json jrt;
		jrt["type"] = "TThreadPool";
		jrt["runableNodeSize"] = this->runableNodeSize;
		jrt["threads"] = json::array();
		for (auto i : threads) {
			jrt["threads"].push_back(json(decPtr(i)));
		}
		jrt["nodeC"] = nodeC.getJson();
		return jrt;
	}

	json NodeC::getJson() {
		json jrt;
		jrt["this"] = decPtr(this);
		if (empty()) {
			jrt["it"] = nullptr;
		}
		else {
			jrt["it"] = decPtr(*activeIt);
		}
		
		jrt["list"] = json::array();
		
		for (auto i : list) {
			jrt["list"].push_back(i->getJson());
		}
		return jrt;
	}

	json TTNode::getJson() {
		json jrt;
		jrt["this"] = decPtr(this);
		jrt["fptr"] = decPtr(*reinterpret_cast<void**>(&fptr));
		jrt["de_fptr"] = decPtr(*reinterpret_cast<void**>(&de_fptr));
		jrt["running"] = running;
		jrt["nodeC"] = nodeC.getJson();
		return jrt;
	}
#endif
}