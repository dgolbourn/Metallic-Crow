#ifndef FOR_EACH_H_
#define FOR_EACH_H_
namespace event
{
template<typename Container, typename... Args> void for_each(Container& container, Args... args)
{
  for(auto iter = container.begin(); iter != container.end();)
  {
    if((*iter)(args...))
    {
      ++iter;
    }
    else
    {
      iter = container.erase(iter);
    }
  }
}
}
#endif