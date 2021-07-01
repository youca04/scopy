#ifndef GRPORT_H
#define GRPORT_H

#include <grsource.h>

class GRPort
{
	typedef enum {SHORT,FLOAT,COMPLEX} dataType;

	GRSource::sptr src;
	uint srcPort;
	dataType type;
	std::string name;
	std::string shortName;

	bool isSink;


public:
	GRPort();
	GRPort(GRSource::sptr src, uint srcPort, dataType type, std::string name, std::string shortName, bool isSink = false);
	virtual ~GRPort();

};

#endif // GRPORT_H
