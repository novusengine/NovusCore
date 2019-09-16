#include "InputQueue.h"

moodycamel::ConcurrentQueue<Message>* InputQueue::_inputQueue = nullptr;