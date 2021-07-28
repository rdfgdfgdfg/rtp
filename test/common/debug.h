#pragma once

#include <sstream>
namespace MAT {
	std::string decPtr(void* ptr) {
		std::stringstream ss;
		ss.unsetf(std::ios_base::dec);
		ss << ptr;
		return ss.str();
	}


	json TThreadPool::getJson() {
		json jrt;
		jrt["type"] = "TThreadPool";
		jrt["size"] = this->size;
		jrt["threads"] = json::array();
		for (auto i : threads) {
			jrt["threads"].push_back(json(decPtr(i)));
		}
		jrt["nodeC"] = nodeC.getJson();
		return jrt;
	}

	json NodeC::getJson() {
		json jrt;
		jrt["this"] = decPtr(this);
		if (empty()) {
			jrt["it"] = nullptr;
		}
		else {
			jrt["it"] = decPtr(*activeIt);
		}
		
		jrt["list"] = json::array();
		
		for (auto i : list) {
			jrt["list"].push_back(i->getJson());
		}
		return jrt;
	}

	json TTNode::getJson() {
		json jrt;
		jrt["this"] = decPtr(this);
		jrt["fptr"] = decPtr(*reinterpret_cast<void**>(&fptr));
		jrt["de_fptr"] = decPtr(*reinterpret_cast<void**>(&de_fptr));
		jrt["running"] = running;
		jrt["nodeC"] = nodeC.getJson();
		return jrt;
	}
}