#include "wavsrc_experimental.h"
#include <iio/device_source.h>
#include <gnuradio/blocks/copy.h>
#include <gnuradio/blocks/null_sink.h>

#include <gnuradio/blocks/wavfile_source.h>
#include <gnuradio/blocks/float_to_short.h>
#include <QDebug>
#include <iio.h>

WavSrc_Experimental::WavSrc_Experimental(struct iio_context *ctx, std::string _dev, QObject *parent) :
	gr::hier_block2("wavsource",
			gr::io_signature::make(0, 0, sizeof(short)) ,

			/*terminate called after throwing an instance of 'std::runtime_error'
			  what():  Hierarchical blocks do not yet support arbitrary or variable numbers of inputs or outputs (iiosource)*/
			gr::io_signature::make(4, 4, sizeof(short))),
	GRSource(ctx, parent)
{

	struct iio_device *dev = iio_context_find_device(ctx, _dev.c_str());
	nb_channels = 1;//iio_device_get_channels_count(dev);
	//iio_blk = gr::iio::device_source::make_from(ctx, _dev.c_str(),{"voltage0","voltage1"},_dev.c_str(),{""});
	iio_blk = gr::blocks::wavfile_source::make("/home/adi/build-alsa-test-Desktop-Debug/happy.wav",true);


	auto f2s = gr::blocks::float_to_short::make(1,1024);


	/* Avoid unconnected channel errors by connecting a dummy sink */
	auto dummy_copy = gr::blocks::copy::make(sizeof(short));
	auto dummy = gr::blocks::null_sink::make(sizeof(short));

	for (unsigned i = 0; i < nb_channels; i++) {

		hier_block2::connect(iio_blk,i,f2s,0);
		hier_block2::connect(f2s,0,dummy_copy,i);
		//hier_block2::connect(iio_blk,i, dummy_copy, i);

		hier_block2::connect(dummy_copy, i, dummy, i);
		//hier_block2::connect(iio_blk,i,self(),i);
		hier_block2::connect(f2s,i,self(),i);

	}

	hier_block2::connect(f2s,0,self(),1); // TEMP
	hier_block2::connect(f2s,0,self(),2); // TEMP
	hier_block2::connect(f2s,0,self(),3); // TEMP

	dummy_copy->set_enabled(true);

	timeout_b = gnuradio::get_initial_sptr(new adiscope::timeout_block("msg"));
	/*hier_block2::msg_connect(iio_blk, "msg", timeout_b, "msg");

	QObject::connect(&*timeout_b, SIGNAL(timeout()), this,
			 SLOT(got_timeout()));*/
	qDebug()<<"iio source created";
}

WavSrc_Experimental::~WavSrc_Experimental()
{
	qDebug()<<"iiosource destroyed";
}

WavSrc_Experimental::sptr WavSrc_Experimental::make(struct iio_context *ctx, std::string _dev)
{
	return gnuradio::get_initial_sptr(new WavSrc_Experimental(ctx, _dev));
}


void WavSrc_Experimental::set_buffer_size(unsigned long size)
{
	//std::unique_lock<std::mutex> lock(copy_mutex);
	buffer_size = size;
	update_buffer_size_unlocked();
}
void WavSrc_Experimental::update_buffer_size_unlocked()
{
	//iio_blk->set_buffer_size(buffer_size);
}

void WavSrc_Experimental::set_device_timeout(unsigned int mseconds)
{
	//iio_blk->set_timeout_ms(mseconds);
}
