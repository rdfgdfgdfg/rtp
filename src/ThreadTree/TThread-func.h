#pragma once

namespace MAT {
	void TThread::run(std::list<std::thread*>::iterator it) {
		NodeC::pos ndp;
		char signal;
		while (true) {//主循环
			NodeC* nodeCNow = &nodeC;//将要执行的节点的节点容器的指针
			TTNode* nodeNow = nullptr;//将要执行的节点的指针
			while (true) {//单链循环
				changeList.lock();
				ndp = nodeCNow->getNodeLower();//获取下一个节点的位置
				if (ndp.ptr != nullptr) {//检验合法性
					nodeNow = *ndp.it;//赋值
					nodeCNow = &nodeNow->nodeC;
					nodeNow->running = true;
					changeList.unlock();
					do {//执行用户函数
						signal = (nodeNow->*(nodeNow->fptr))();
					} while (signal == TTHREAD_KEEP);
					changeList.lock();
					nodeNow->running = false;//修改数值
					if (signal == TTHREAD_STOP) {
						if (nodeCNow->empty()) {
							ndp.ptr->erase(ndp.it);
							delete nodeNow;
						}
						else {
							nodeNow->fptr = nullptr;
						}
						if (tryDeleteThread(it)) {
							changeList.unlock();
							return;
						}
					}
					changeList.unlock();

				}
				else {
					changeList.unlock();
					break;
				}
			}
		}
	}


	inline void TThread::tryCreateThread() {
		while (size > threads.size() and threads.size() < maxThreadsSize) {
			threads.push_back(nullptr);
			threads.back() = new std::thread(&TThread::run, this, --threads.end());
		}
	}


	bool TThread::tryDeleteThread(std::list<std::thread*>::iterator it) {
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

	inline TThread::TThread(): maxThreadsSize(0), size(0) {}
	inline TThread::~TThread(){}
}