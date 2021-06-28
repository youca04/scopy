#include "iiosource.h"
#include <iio/device_source.h>
#include <gnuradio/blocks/copy.h>
#include <gnuradio/blocks/null_sink.h>
#include <QDebug>
#include <iio.h>

IioSource::IioSource(struct iio_context *ctx, std::string _dev, QObject *parent) :
	gr::hier_block2("iiosource",
			gr::io_signature::make(0, 0, sizeof(short)) ,

			/*terminate called after throwing an instance of 'std::runtime_error'
			  what():  Hierarchical blocks do not yet support arbitrary or variable numbers of inputs or outputs (iiosource)*/
			gr::io_signature::make(2, 2, sizeof(short))),
	GRSource(ctx, parent)
{
	struct iio_device *dev = iio_context_find_device(ctx, _dev.c_str());
	nb_channels = iio_device_get_channels_count(dev);
	iio_blk = gr::iio::device_source::make_from(ctx, _dev.c_str(),{"voltage0","voltage1"},_dev.c_str(),{""});

	/* Avoid unconnected channel errors by connecting a dummy sink */
	auto dummy_copy = gr::blocks::copy::make(sizeof(short));
	auto dummy = gr::blocks::null_sink::make(sizeof(short));

	for (unsigned i = 0; i < nb_channels; i++) {
		hier_block2::connect(iio_blk,i, dummy_copy, i);
		hier_block2::connect(dummy_copy, i, dummy, i);
		hier_block2::connect(iio_blk,i,self(),i);
	}

	dummy_copy->set_enabled(true);

	timeout_b = gnuradio::get_initial_sptr(new adiscope::timeout_block("msg"));
	hier_block2::msg_connect(iio_blk, "msg", timeout_b, "msg");

	QObject::connect(&*timeout_b, SIGNAL(timeout()), this,
			 SLOT(got_timeout()));
	qDebug()<<"iio source created";
}

IioSource::~IioSource()
{
	qDebug()<<"iiosource destroyed";
}

IioSource::sptr IioSource::make(struct iio_context *ctx, std::string _dev)
{
	return gnuradio::get_initial_sptr(new IioSource(ctx, _dev));
}


void IioSource::set_buffer_size(unsigned long size)
{
	//std::unique_lock<std::mutex> lock(copy_mutex);
	buffer_size = size;
	update_buffer_size_unlocked();
}
void IioSource::update_buffer_size_unlocked()
{
	iio_blk->set_buffer_size(buffer_size);
}

void IioSource::set_device_timeout(unsigned int mseconds)
{
	iio_blk->set_timeout_ms(mseconds);
}
