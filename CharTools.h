#ifndef CHARTOOLS_CLASS
#define CHARTOOLS_CLASS

class CharToolsClass
{
  public:
	void replace(const char *source, char buffer[], const char *key, const char *value);
	void replace(const char *source, char buffer[], const char *keys[], const char *values[], int length);
};

extern CharToolsClass CharTools;

#endif