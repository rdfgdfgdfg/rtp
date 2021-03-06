#pragma once

#include <list>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <condition_variable>

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
		using iterator = std::list<TTNode*>::iterator;
	private:
#ifdef THREADTREE_DEBUG
	public:
#endif
		std::list<TTNode*> list;

		std::list<TTNode*>::iterator activeIt;

		void next();//activeIt移位

		void erase(iterator it);

		void push_back(TTNode* ptr);//线程不安全

		TTNode* getNodeLower();//指向属于此节点容器的最近可执行节点（线程不安全）

		bool empty();//线程不安全
	public:

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
		size_c runableNodeSize;//节点数量
		std::vector<std::thread*> threads;
		size_c runningThread;

		void tryCreateThread();//尝试创建线程（线程不安全）
		bool tryDeleteThread(std::list<std::thread*>::iterator it);//尝试删除线程（线程不安全）

		void run();
		bool checkRunable();
	protected:
		std::mutex changeList;
		std::mutex noticeLock;
		std::condition_variable cv;
	public:

		TThreadPool():runningThread(0), runableNodeSize(0){};

		TThreadPool(const TThreadPool&) = delete;
		TThreadPool(const TThreadPool&&) = delete;

		~TThreadPool();
		void join();
		void start(size_c size);
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
		Fptr de_fptr;

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



	TTNode* NodeC::getNodeLower() {
		/*
		|
		_____________(ptrC指向容器,activeIt是容器的迭代器)
		o ^ o ^ o ^ o(ptr是此层，指向节点的指针）
		|       |
		_____
		o ^ o
		
		*/
		if (empty()) {
			return nullptr;
		}
		NodeC* ptrC = this;
		next();
		NodeC::iterator itC = activeIt;

		TTNode* ptr = *itC;//要执行的节点的指针
		while (true) {//循环

			if (ptr->running | ptr->fptrNULL()) {
				if (ptr->nodeC.empty()) {
					return nullptr;
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
					ptrC = &ptr->nodeC;
					ptrC->next();
					itC = ptrC->activeIt;
					ptr = *itC;
				}
			}
			else {
				return ptr;
			}
			
		}
	}


//---------------------------------------------------------------------
//TThreadPool-func
	void TThreadPool::run() {
		while (true) {//主循环
			this;
			NodeC* nodeCNow = &nodeC;//将要执行的节点的节点容器的指针
			TTNode* nodeNow = nullptr;//将要执行的节点的指针
			while (true) {//单链循环
				changeList.lock();
				if (checkRunable()) { changeList.unlock(); return; }
				//auto start = std::chrono::high_resolution_clock::now();
				nodeNow = nodeCNow->getNodeLower();//获取下一个节点的位置
				//auto end = std::chrono::high_resolution_clock::now();
				//std::cout << "cost" << (end - start).count() << std::endl;
				if (nodeNow != nullptr) {//检验合法性
					nodeCNow = &nodeNow->nodeC;//赋值
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


	void TThreadPool::join() {
		while (true) {
			changeList.lock();
			if (runningThread == 0) {
				changeList.unlock();
				return;
			}
			changeList.unlock();
			for (auto i : threads) {
				if (i->joinable()) {
					i->join();
				}
			}
		}
	}

	void TThreadPool::start(size_c size) {
		threads.reserve(size);
		changeList.lock();
		for (size_c i = 0; i < size; i++) {
			threads.push_back(new std::thread(&TThreadPool::run, this));
		}
		runningThread = size;
		changeList.unlock();
	}

	bool TThreadPool::checkRunable() {
		while (true) {
			if (runableNodeSize == 0) {
				runningThread = 0;
				cv.notify_one();
				return true;
			}
			if (runableNodeSize < runningThread) {
				runningThread--;
				std::unique_lock<std::mutex> lk(noticeLock);
				changeList.unlock();
				cv.wait(lk);
				changeList.lock();
				runningThread++;
			}
			else {
				if ((runableNodeSize > runningThread) & (runningThread < threads.size())) {
					cv.notify_one();
				}
				return false;
			}
		}

	}

	TThreadPool::~TThreadPool() {
		join();
		for (auto i : threads) {
			delete i;
		}
	}

//---------------------------------------------------------------------
//TTNode-func
	template<class T>
	TTNode::TTNode(TThreadPool* wrap, T fptr) :
		wrap(wrap), belong(nullptr), fptr(static_cast<Fptr>(fptr)), de_fptr(nullptr), running(false) {
		wrap->nodeC.push_back(this);
		it = --wrap->nodeC.list.end();
		wrap->runableNodeSize++;
	};

	template<class T>
	TTNode::TTNode(TTNode* belong, T fptr) :
		wrap(belong->wrap), belong(belong), fptr(static_cast<Fptr>(fptr)), de_fptr(nullptr), running(false) {
		belong->nodeC.push_back(this);
		it = --belong->nodeC.list.end();
		wrap->runableNodeSize++;
	};

	inline bool TTNode::fptrNULL() {
		return fptr == nullptr;
	}

	void TTNode::maintain() {
		if (nodeC.empty()) {
			if (de_fptr == nullptr) {
				if (fptr == nullptr) {
					if (belong == nullptr) {
						wrap->nodeC.erase(it);
					}
					else {
						belong->nodeC.erase(it);
					}
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