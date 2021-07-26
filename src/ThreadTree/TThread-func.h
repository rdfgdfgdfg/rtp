#pragma once

namespace MAT {
	void TThread::run(std::list<std::thread*>::iterator it) {
		NodeC::pos ndp;
		char signal;
		while (true) {//��ѭ��
			NodeC* nodeCNow = &nodeC;//��Ҫִ�еĽڵ�Ľڵ�������ָ��
			TTNode* nodeNow = nullptr;//��Ҫִ�еĽڵ��ָ��
			while (true) {//����ѭ��
				changeList.lock();
				ndp = nodeCNow->getNodeLower();//��ȡ��һ���ڵ��λ��
				if (ndp.ptr != nullptr) {//����Ϸ���
					nodeNow = *ndp.it;//��ֵ
					nodeCNow = &nodeNow->nodeC;
					nodeNow->running = true;
					changeList.unlock();
					do {//ִ���û�����
						signal = (nodeNow->*(nodeNow->fptr))();
					} while (signal == TTHREAD_KEEP);
					changeList.lock();
					nodeNow->running = false;//�޸���ֵ
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