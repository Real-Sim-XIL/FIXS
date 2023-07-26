
namespace RealSimMath{
	
template<typename T> int isnan(T x)
{
    volatile T tmp = x; /* Prevent optimization by compiler. */
    return tmp != x;
}



}