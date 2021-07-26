#pragma once

namespace MAT {

	TTNode::TTNode(TThread* belong) :belong(belong), fptr(nullptr), de_fptr(nullptr), running(false) {
		belong->changeList.lock();
		belong->nodeC.push_back(this);
		belong->size++;
		belong->tryCreateThread();
		belong->changeList.unlock();
	};

	TTNode::TTNode(TTNode* wrap) :belong(wrap->belong), fptr(nullptr), de_fptr(nullptr), running(false) {
		belong->changeList.lock();
		wrap->nodeC.push_back(this);
		belong->size++;
		belong->tryCreateThread();
		belong->changeList.unlock();
	};

	inline TTNode::~TTNode() {}
}