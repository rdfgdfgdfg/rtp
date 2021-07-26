#pragma once

namespace MAT {
	inline void NodeC::next() {
		if (activeIt == list.end()) {
			activeIt = list.begin();
		}
		else {
			activeIt++;
		}
	}

	inline bool NodeC::empty() {
		return list.empty();
	}

	void NodeC::erase(iterator it) {
		if (activeIt == it) {
			if (activeIt == list.end()) {//ɾ�����߳����߳���ĩβ
				if (activeIt == list.begin()) {//�̳߳��߳���ֻ�д�ɾ���߳�
					list.erase(it);
					activeIt._Ptr = nullptr;
				}
				else {//�̳߳ص�ǰ�ɷ��ʵ��߳��л��������߳�
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
	}

	NodeC::pos NodeC::getNodeLower() {
		/*
		|
		_____________(rt.ptrָ������,it�������ĵ�����)
		o ^ o ^ o ^ o(ptr�Ǵ˲㣬ָ��ڵ��ָ�룩
		|       |
		_____
		o ^ o
		
		*/
		pos rt;
		rt.ptr = this;
		rt.it = activeIt;
		TTNode* ptr = *rt.it;//Ҫִ�еĽڵ��ָ��
		bool fptrNull;
		bool ptrRun;
		next();
		while (true) {//ѭ��
			fptrNull = ptr->fptr != nullptr;
			ptrRun = !ptr->running;
			if (fptrNull and ptrRun) {//��ptr�����������ͷ���
				return rt;
			}
			else {//��ptr����������
				if (ptr->nodeC.empty()) {//����ptr��û�������ڵ�
		/*
|
_____________(rt.ptrָ������,it�������ĵ�����)
o ^ o ^ o ^ o(ptr�Ǵ˲㣬ָ��ڵ��ָ�룩
		*/
					if (!fptrNull) {//����ptr����ִ��
						if (ptr->de_fptr == nullptr) {
							rt.ptr->erase(rt.it);
							delete ptr;
						}
						else {
							ptr->fptr = ptr->de_fptr;
						}
					}
					rt.ptr = nullptr;//����
					return rt;
					//(Ϊʲô����rt.ptr�ڼ������ң�
					//��������Ҫ���Ѿ�next���ˣ�run��ѭ��������������µ����ʵ�����Ѿ����˼������ҵ�Ŀ��
				}
				else {
					/*
|
_____________(rt.ptrָ������,it�������ĵ�����)
o ^ o ^ o ^ o(ptr�Ǵ˲㣬ָ��ڵ��ָ�룩
|       |
_____(�µ�rt.ptrָ��ptr������,it���������ĵ�����)
o ^ o(��ptr�Ǵ˲㣬ָ��ڵ��ָ�룩

*/
					rt.ptr = &ptr->nodeC;
					rt.it = rt.ptr->activeIt;
					ptr = *rt.it;
					rt.ptr->next();
				}
			}
		}
	}


}