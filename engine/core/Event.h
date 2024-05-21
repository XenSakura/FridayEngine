#include <queue>

template <typename... PayloadType>
class Publisher
{
	Publisher()
	{

	}
	
	std::queue<PayloadType> payload;
};

class EventHandler
{

};

class Subscriber
{

};
