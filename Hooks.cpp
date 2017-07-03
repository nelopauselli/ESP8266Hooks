#ifndef Hooks_h
#define Hooks_h

class Hooks
{
	public:
		Hooks(const char* mac){
			_mac = mac;
		}

		const char* get_mac(){
			return _mac;
		}
	private:
		const char* _mac;
};

#endif