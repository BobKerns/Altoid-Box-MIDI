#include "Callback.h"

int Callback::next_idx = 0;
std::function<void()> Callback::dispatch[Callback::number_of_callbacks];
