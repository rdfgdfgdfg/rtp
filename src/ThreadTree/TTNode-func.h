#pragma once

namespace MAT {

	TTNode::TTNode(TThreadPool* belong) :belong(belong), fptr(nullptr), de_fptr(nullptr), running(false) {
		belong->changeList.lock();
		belong->nodeC.push_back(this);
		belong->size++;
		belong->tryCreateThread();
	};

	TTNode::TTNode(TTNode* wrap) :belong(wrap->belong), fptr(nullptr), de_fptr(nullptr), running(false) {
		belong->changeList.lock();
		wrap->nodeC.push_back(this);
		belong->size++;
		belong->tryCreateThread();
	};

	inline bool TTNode::fptrNULL() {
		return fptr == nullptr;
	}

	inline void TTNode::unlock() {
		belong->changeList.unlock();
	}

	inline bool TTNode::maintain(NodeC::iterator it, NodeC* ptr) {
		if (nodeC.empty()) {
			if (de_fptr != nullptr) {
				fptr = de_fptr;
			}
			if (fptr == nullptr) {
				ptr->erase(it);
				return true;
			}
		}
		return false;
	}
}