#pragma once

namespace MAT {
	void TThread::run(std::thread* ptr) {
		NodeC::pos ndp;
		char signal;
		while (true) {//��ѭ��
			NodeC* nodeCNow = &nodeC;//��Ҫִ�еĽڵ�Ľڵ�������ָ��
			TTNode* nodeNow = nullptr;//��Ҫִ�еĽڵ��ָ��
			while (true) {//����ѭ��
				changeList.lock();
				ndp = nodeCNow->getNodeLower();//��ȡ��һ���ڵ��λ��
				if (ndp.ptr != nullptr) {//����Ϸ���
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