
#include "grport.h"
#include <grsource.h>

GRPort::GRPort()
{

}

GRPort::GRPort(GRSource::sptr src, uint srcPort, dataType type, std::string name, std::string shortName, bool isSink) :
	src(src), srcPort(srcPort),type(type),name(name),shortName(shortName),isSink(isSink)
{

}


GRPort::~GRPort() {

}
