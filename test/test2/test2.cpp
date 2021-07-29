// test1.cpp: 定义应用程序的入口点。
//

#include "test2.h"

using namespace std;
nlohmann::json jlog = nlohmann::json::array();
class A :public MAT::TTNode {
public:
	int a;
	A** ptr;
	void init() {
		fptr = static_cast<Fptr>(&A::foo);
		if (a > 0) {
			ptr = new A * [a];
		}
		else {
			ptr = nullptr;
		}
		for (int i = 0; i < a; i++) {
			new A(this, a - 1);
		}
	}
	A(MAT::TThreadPool* belong, int a) : MAT::TTNode(belong), a(a){
		init();
	}
	A(MAT::TTNode* wrap, int a) : MAT::TTNode(wrap), a(a) {
		init();
	}
	void foo() {
		cout << "foo" << endl;
		fptr = nullptr;
	}
	//->foo
	void de_foo() {
		for (int i = 0; i < a; i++) {
			delete ptr[i];
		}
		delete ptr;
	}
};

void test0() {
	MAT::TThreadPool ttp;
	A a0(&ttp, 0);
	ttp.setMaxThreadsSize(1);
	ttp.start();
	ttp.join();
}

void test1() {
	MAT::TThreadPool ttp;
	A a0(&ttp, 2);
	ttp.setMaxThreadsSize(1);
	ttp.start();
	ttp.join();
}

void test2() {//
	MAT::TThreadPool ttp;
	A a0(&ttp, 3);
	ttp.setMaxThreadsSize(1);
	ttp.start();
	ttp.join();
}

void test3() {
	MAT::TThreadPool ttp;
	A a0(&ttp, 0);
	A a1(&ttp, 0);
	ttp.setMaxThreadsSize(1);
	ttp.start();
	ttp.join();
}

void test4() {
	MAT::TThreadPool ttp;
	A a0(&ttp, 0);
	A a1(&ttp, 1);
	ttp.setMaxThreadsSize(1);
	ttp.start();
	ttp.join();
}

int main()
{
	test3();
	return 0;
}
