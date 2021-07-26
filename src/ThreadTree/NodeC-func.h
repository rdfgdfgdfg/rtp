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
		bool fptrNull;
		bool ptrRun;
		while (true) {//循环
			fptrNull = ptr->fptr == nullptr;
			ptrRun = !ptr->running;
			if (!fptrNull and ptrRun) {//若ptr满足条件，就返回
				return rt;
			}
			else {//若ptr不满足条件
				if (ptr->nodeC.empty()) {//并且ptr下没有其他节点（如图）
		/*
|
_____________(rt.ptr指向容器,it是容器的迭代器)
o ^ o ^ o ^ o(ptr是此层，指向节点的指针）
		*/
					if (fptrNull) {//若该ptr不可执行
						if (ptr->de_fptr == nullptr) {//并且该ptr不存在回调函数
							rt.ptr->erase(rt.it);//将该节点移除
							delete ptr;//并删除
						}
						else {//但是该ptr存在回调函数
							ptr->fptr = ptr->de_fptr;//将回调函数作为用户函数
							ptr->de_fptr = nullptr;
							return rt;//正常返回
						}
					}
					rt.ptr = nullptr;//报错
					return rt;
					//(为什么不在rt.ptr内继续查找？
					//——不必要。已经next过了，run主循环迭代后，如果重新到这里，实际上已经起到了继续查找的目的
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
