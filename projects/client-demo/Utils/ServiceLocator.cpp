#include "ServiceLocator.h"

moodycamel::ConcurrentQueue<Message>* ServiceLocator::_mainInputQueue = nullptr;