#include <queue>
#include <utility>
#include <stdint.h>
#include <cassert>



using Entry = std::pair<uint32_t, int>;



struct _Comparator
{
	bool operator()(const Entry& lhs, const Entry& rhs){return lhs.first > rhs.first;};
};



int main()
{

	std::priority_queue<Entry, std::vector<Entry>, _Comparator> q;

	q.emplace(15000, 1);
	assert(q.top().second == 1);	
	q.emplace(5000, 0);
	assert(q.top().second == 0);	
	q.emplace(30000, 2);
	assert(q.top().second == 0);	
}