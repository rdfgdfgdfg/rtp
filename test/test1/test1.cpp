﻿// test1.cpp: 定义应用程序的入口点。
//

#include "test1.h"

using namespace std;
nlohmann::json jlog = nlohmann::json::array();
class A :public MAT::TTNode {
public:
	int a;
	A** ptr;
	void init() {
		if (a > 0) {
			ptr = new A * [a];
		}
		else {
			ptr = nullptr;
		}
		for (int i = 0; i < a; i++) {
			ptr[i] = new A(this, a - 1);
		}
	}
	A(MAT::TThreadPool* belong, int a) : MAT::TTNode(belong, &A::foo), a(a){
		init();
	}
	A(MAT::TTNode* wrap, int a) : MAT::TTNode(wrap, &A::foo), a(a) {
		init();
	}
	void foo() {
		cout << "foo" << endl;
		setDFptr(&A::de_foo);
	}
	//->foo
	void de_foo() {

		for (int i = 0; i < a; i++) {
			delete ptr[i];
		}
		delete[] ptr;
		cout << "delete foo" << endl;
		setOver();
	}
};

void test0() {
	MAT::TThreadPool ttp;
	A a0(&ttp, 0);
	ttp.setMaxThreadsSize(1);
	ttp.join();
}

void test1() {
	MAT::TThreadPool ttp;
	A a0(&ttp, 2);
	ttp.setMaxThreadsSize(1);
	ttp.join();
}

void test2() {//
	MAT::TThreadPool ttp;
	A a0(&ttp, 1);
	ttp.setMaxThreadsSize(1);
	ttp.join();
}

void test3() {
	MAT::TThreadPool ttp;
	A a0(&ttp, 0);
	A a1(&ttp, 0);
	ttp.setMaxThreadsSize(1);
	ttp.join();
}

void test4() {//
	MAT::TThreadPool ttp;
	A a0(&ttp, 0);
	A a1(&ttp, 1);
	ttp.setMaxThreadsSize(1);
	ttp.join();
}

//1'000'000'000ns = 1s , 1'000'000ns = 1ms
void test5() {//getNodeLower平均978ns，最长112'900ns
	MAT::TThreadPool ttp;
	A a0(&ttp, 5);
	A a1(&ttp, 5);
	ttp.setMaxThreadsSize(2);
	ttp.join();
}

void test6() {//
	MAT::TThreadPool ttp;
	A a0(&ttp, 1);
	A a1(&ttp, 1);
	ttp.setMaxThreadsSize(6);
	ttp.join();
}

void test7() {//getNodeLower平均
	MAT::TThreadPool ttp;
	A a0(&ttp, 256);
	A a1(&ttp, 256);
	ttp.setMaxThreadsSize(-1);
	ttp.join();
}

int main()
{
	cout << "烧机即将开始" << endl;
	system("pause");
	test7();
	return 0;
}
