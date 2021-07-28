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
		TThreadPool* belong = (*it)->belong;
		belong->size--;
		
		if (activeIt == it) {
			if (activeIt == list.end()) {//删除的线程在线程链末尾
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
		rt.ptr = this;
		next();
		rt.it = activeIt;
		TTNode* ptr = *rt.it;//要执行的节点的指针
		while (true) {//循环
			if (ptr->maintain(rt.it, rt.ptr)) {//维护
				rt.ptr = nullptr;//若节点被移除
				return rt;//报错
			}
			else {//若节点存在
				if (ptr->running) {
					rt.ptr = nullptr;//若节点正在被执行
					return rt;//报错
				}
				else {
					if (!ptr->fptrNULL()) {//满足条件
						return rt;
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
						rt.ptr->next();
						rt.ptr = &ptr->nodeC;
						rt.it = rt.ptr->activeIt;
						ptr = *rt.it;
					}
				}
			}
		}
	}


}
