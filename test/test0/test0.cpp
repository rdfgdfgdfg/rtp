// test1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// test NodeC::getNodeLower()

#include "test0.h"


using namespace std;
class A :public MAT::TTNode {
public:



	A(MAT::TThreadPool* belong, int a) : MAT::TTNode(belong, &A::foo) {
		for (int i = 0; i < a; i++) {
			new A(this, a - 1);
		}
	}
	A(MAT::TTNode* wrap, int a) : MAT::TTNode(wrap, &A::foo) {
		for (int i = 0; i < a; i++) {
			new A(this, a - 1);
		}
	}
	void foo() {
		cout << "hello" << endl;
		setOver();
	}
};

int main()
{

	MAT::TThreadPool ttp;
	A a1(&ttp, 3);
	A a2(&ttp, 3);
	A a3(&ttp, 3);
	ttp.start(1);
	ttp.join();

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
