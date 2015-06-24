#ifndef TTF_LIBRARY_H_
#define TTF_LIBRARY_H_
namespace ttf
{
class Library
{
public:
  Library();
  Library(Library const& other);
  Library(Library&& other);
  Library& operator=(Library other);
  ~Library();
};
}
#endif