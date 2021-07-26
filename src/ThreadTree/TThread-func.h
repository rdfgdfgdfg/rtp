#pragma once

namespace MAT {
	void TThread::run(std::thread* ptr) {
		NodeC::pos ndp;
		char signal;
		while (true) {//主循环
			NodeC* nodeCNow = &nodeC;//将要执行的节点的节点容器的指针
			TTNode* nodeNow = nullptr;//将要执行的节点的指针
			while (true) {//单链循环
				changeList.lock();
				ndp = nodeCNow->getNodeLower();//获取下一个节点的位置
				if (ndp.ptr != nullptr) {//检验合法性
					nodeNow = *ndp.it;
					nodeCNow = &nodeNow->nodeC;
					nodeNow->running = true;
					changeList.unlock();
					do {
						signal = (nodeNow->*(nodeNow->fptr))();
					} while (signal == TTHREAD_KEEP);
					changeList.lock();
					nodeNow->running = false;
					if (signal == TTHREAD_STOP) {
						if (nodeCNow->empty()) {
							ndp.ptr->erase(ndp.it);
						}
						else {
							nodeNow->fptr = nullptr;
						}
						if (tryDeleteThread(ptr)) {
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
}