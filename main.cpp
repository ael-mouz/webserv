#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
// #include <sys/stat.h>

class obj
{
protected:
	int i;
	void ff();
public:

};
class chiled : public obj{
	private:
	void ft();
};
void obj::ff()
{
	this->i;
}
void chiled::ft()
{
	this->i;
}
int main()
{

}
