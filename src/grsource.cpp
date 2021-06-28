#include "grsource.h"

#include <QDebug>



GRSource::GRSource(struct iio_context *ctx, QObject *parent) {
	qDebug()<<"GRSource created;";
}
GRSource::~GRSource()  {

	qDebug()<<"GRSource destroyed;";
}
/*static GRSource::sptr GRSource::make(struct iio_context *ctx) {
	return gnuradio::get_initial_sptr(new GRSource(ctx));
}*/


void GRSource::set_buffer_size(unsigned long size) {
	buffer_size = size;
	update_buffer_size_unlocked();
}


void GRSource::set_device_timeout(unsigned int mseconds) {

}

void GRSource::got_timeout() {
	Q_EMIT timeout();
}
