#include <iostream>
#include <future>
#include <numeric>
#include <functional>
#include <chrono>
#include <cmath>
#include <exception>
#include <stdexcept>
#include <mutex>
#include <queue>
#include <memory>
#include <vector>
#include <condition_variable>
#include <string>

using namespace std;

template<typename T>
class sequential_queue {
		
	struct node {
		T data;
		unique_ptr<node> next;

		node(T data) : data(move(data)) {}
	};

	condition_variable cv;

	//initializing the values of the node
	unique_ptr<node> head;
	node* tail;

	//declaring mutex locks each for head and tail
	mutex head_mutex;
	mutex tail_mutex;

	node* get_tail() {
		lock_guard<mutex> lg(tail_mutex);
		return tail;
	}

public:
	sequential_queue(): head(new node), tail(head.get()) {}

	void push(T value) {
		
		shared_ptr<T> new_data(make_shared<T>(move(value)));
		unique_ptr<node> p(new node);
		node* const new_tail = p.get();

		{
			lock_guard<mutex> tail_lock(tail_mutex);
			tail->data = new_data;
			tail->next = move(p);
			tail = new_tail;

		}
		cv.notify_one();
		
	}

	shared_ptr<T> pop() {

		lock_guard<mutex> head_guard(head_mutex);
		if (head.get() == get_tail()) {
			return shared_ptr<T>();
		}

		shared_ptr<T> const res(make_shared<T>(move(head->data)));
		unique_ptr<node> const old_head = move(head);
		head = move(old_head->next);
		return res;

	}
};

int main() {


}
