#include "stdafx.h"
#include <iostream>
#include <unordered_map>
#include "thread"
#include "mutex"
#include <chrono>
#include <ctime>
#include <vector>
typedef std::chrono::high_resolution_clock Clock;
std::mutex mtx;
using namespace std;

class LRUCache
{
	std::mutex mu;

private:
	int cacheSize;

	std::unordered_map<int, std::pair<int, std::list<int>::iterator>> cache;

	std::list<int> lru;
	void update(std::unordered_map<int, std::pair<int, std::list<int>::iterator>>::iterator &it);

	void deleteM(std::unordered_map<int, std::pair<int, std::list<int>::iterator>>::iterator &it);
public:
	template <typename T>
	void setSize(T size);
	template <typename T>
	int get(T key);
	template <typename T>
	void set(T key, T value);

};
template <typename T>
void LRUCache::setSize(T size)
{
	std::lock_guard <std::mutex> locker(mu);
	cacheSize = size;
}
template <typename T>
int LRUCache::get(T key)
{
	std::lock_guard <std::mutex> locker(mu);
	auto item = cache.find(key);

	if (item == cache.end())
	{
		return -1;
	}

	update(item);

	return item->second.first;
}
template <typename T>
void LRUCache::set(T key, T value)
{
	std::lock_guard <std::mutex> locker(mu);

	auto item = cache.find(key);

	if (item != cache.end())
	{
		update(item);
		cache[key] = { value, lru.begin() };
		return;
	}

	if (cache.size() == cacheSize)
	{
		cache.erase(lru.back());
		lru.pop_back();
	}

	lru.push_front(key);
	cache.insert({ key,{ value, lru.begin() } });
}

void LRUCache::update(std::unordered_map<int, std::pair<int, std::list<int>::iterator>>::iterator &it)
{
	lru.erase(it->second.second);
	lru.push_front(it->first);
	it->second.second = lru.begin();
}

void LRUCache::deleteM(std::unordered_map<int, std::pair<int, std::list<int>::iterator>>::iterator &it)
{
	lru.erase(it->second.second);
	it->second.second = lru.begin();
}
void test1(LRUCache *lruCache, int k, int n)
{
	int i;
	int size = 100000;
	int start, end;
	if (n == 1)
	{
		start = 0;
		end = size;
	}
	else
	{
		start = (size / n)* (k - 1);
		end = (size / n)* k - 1;
	}

	for (i = (0 + start); i < (end - 1); i++)
	{
		lruCache->set(i, i);
		//std::cout << "Thread " << k << "  = " << lruCache->get(i) << endl;
	}
}
void thread_proc(int tnum) {
	LRUCache *lruCache = new LRUCache();
	lruCache->setSize(10);
	int i;
	for (i = 1; i < 10000; i++) {
		{
			std::lock_guard<std::mutex> lock(mtx);
			lruCache->set(i, i);
			//std::cout << "Thread " << tnum << "  = " <<lruCache->get(i) << endl;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
void FIter(int n) {
	LRUCache *lruCache = new LRUCache();
	lruCache->setSize(n);
	int i;
	std::thread t2;
	for (i = 1; i <= n; i++) {
		std::thread t2(test1, lruCache, i, n);
		t2.join();
	}

}
int main() {
	// test for the creation (set) 
	std::vector<std::thread> threads;
	auto t1 = Clock::now();
	auto t2 = Clock::now();
	/*// it work , but so slow (
	t1 = Clock::now();
	for (int i = 0; i < 20; i++) {
		std::thread thr(thread_proc, i);
		threads.emplace_back(std::move(thr));
	}
	for (auto& thr : threads) {
		thr.join();
	}
	t2 = Clock::now();
	auto test01 = t2 - t1;
	cout << "time20: "
		<< std::chrono::duration_cast<std::chrono::nanoseconds>(test01).count()
		<< " nanoseconds" << endl;

	t1 = Clock::now();
	for (int i = 0; i < 1; i++) {
	std::thread thr(thread_proc, i);
	threads.emplace_back(std::move(thr));
	}

	// can't use const auto& here since .join() is not marked const
	for (auto& thr : threads) {
	thr.join();
	}


	t2 = Clock::now();
	auto test02 = t2 - t1;
	cout << "time1 : "
	<< std::chrono::duration_cast<std::chrono::nanoseconds>(test02).count()
	<< " nanoseconds" << endl;
	cout << (double)(test01).count() / ((double)(test02).count() ) << endl;
	*/
	int n = 20;//number of threads
			   //for n threads
	t1 = Clock::now();
	FIter(n);
	t2 = Clock::now();
	auto test0 = t2 - t1;
	cout << "time20: "
		<< std::chrono::duration_cast<std::chrono::nanoseconds>(test0).count()
		<< " nanoseconds" << endl;
	//for 1 threads
	t1 = Clock::now();
	FIter(1);
	t2 = Clock::now();
	auto test1 = t2 - t1;
	cout << "time1 : "
		<< std::chrono::duration_cast<std::chrono::nanoseconds>(test1).count()
		<< " nanoseconds" << endl;

	cout << (double)(test0).count() / ((double)(test1).count() / n) << endl;

	system("pause");
	return 0;
}