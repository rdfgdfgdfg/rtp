#pragma once

namespace MAT {

	TTNode::TTNode(TThreadPool* belong) :
		belong(belong), fptr(nullptr), de_fptr(nullptr), running(false) {
		belong->nodeC.push_back(this);
		belong->size++;
	};

	TTNode::TTNode(TTNode* wrap) :
		belong(wrap->belong), fptr(nullptr), de_fptr(nullptr), running(false) {
		wrap->nodeC.push_back(this);
		belong->size++;
	};

	inline bool TTNode::fptrNULL() {
		return fptr == nullptr;
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

	inline void TTNode::lock() {
		belong->changeList.lock();
	}

	inline void TTNode::unlock() {
		belong->changeList.unlock();
	}

	inline TTNode::Guard TTNode::getGuard() {
		return Guard(&belong->changeList);
	}
	
}