#pragma once

namespace MAT {
	void TThreadPool::run(std::list<std::thread*>::iterator it) {
		NodeC::pos ndp;
		while (true) {//主循环
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
					changeList.unlock();

					(nodeNow->*(nodeNow->fptr))();

					changeList.lock();
					nodeNow->running = false;//修改数值
					nodeNow->maintain(ndp.it, ndp.ptr);
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
		while (size > threads.size() and threads.size() < maxThreadsSize) {
			threads.push_back(nullptr);
			threads.back() = new std::thread(&TThreadPool::run, this, --threads.end());
		}
	}


	bool TThreadPool::tryDeleteThread(std::list<std::thread*>::iterator it) {
		if (size < threads.size()) {
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

	inline TThreadPool::TThreadPool(): maxThreadsSize(0), size(0), forDelete(nullptr) {
	}
	inline TThreadPool::~TThreadPool(){}
}