#include <QObject>
#include <m2ksource.h>
#include <gnuradio/blocks/copy.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/short_to_float.h>
#include <libm2k/m2kexceptions.hpp>
#include <logging_categories.h>

M2kSource::M2kSource(struct iio_context *ctx, QObject *parent) :
	GRSource(ctx),
	gr::hier_block2("m2ksource",
			gr::io_signature::make(0, 0, sizeof(short)) ,
			gr::io_signature::make(2, 2, sizeof(short)))
{

	qDebug(CAT_IIO_MANAGER) << "Creating m2ksource ";

	m_context = libm2k::context::m2kOpen(ctx, "");
	m_analogin = m_context->getAnalogIn();
	if (!ctx)
		throw std::runtime_error("IIO context not created");

	struct iio_device *dev = iio_context_find_device(ctx, "m2k-adc");
	if (!dev)
		throw std::runtime_error("Device not found");

	nb_channels = iio_device_get_channels_count(dev);

	m2k_blk = gr::m2k::analog_in_source::make_from(m_context,
						       buffer_size,
						       {1, 1},
						       {0, 0},
						       10000,
						       1,
						       KERNEL_BUFFERS_DEFAULT,
						       false,
						       false,
						       {0, 0},
						       {0, 0},
						       0,
						       0,
						       {0, 0},
						       false,
						       false);

	/* Avoid unconnected channel errors by connecting a dummy sink */
	auto dummy_copy = gr::blocks::copy::make(sizeof(short));
	auto dummy = gr::blocks::null_sink::make(sizeof(short));
	//gr::blocks::short_to_float::sptr s2f[nb_channels];


	//TODO - make dynamic
	freq_comp_filt[0][0] = adiscope::frequency_compensation_filter::make(false);
	freq_comp_filt[0][1] = adiscope::frequency_compensation_filter::make(false);
	freq_comp_filt[1][0] = adiscope::frequency_compensation_filter::make(false);
	freq_comp_filt[1][1] = adiscope::frequency_compensation_filter::make(false);

	for (unsigned i = 0; i < nb_channels; i++) {
	//	s2f[i] = gr::blocks::short_to_float::make();
		hier_block2::connect(m2k_blk,i,freq_comp_filt[i][0],0);
		hier_block2::connect(freq_comp_filt[i][0],0,freq_comp_filt[i][1],0);

		hier_block2::connect(freq_comp_filt[i][1], 0, dummy_copy, i);

		hier_block2::connect(dummy_copy, i, dummy, i);
	//	hier_block2::connect(dummy_copy,i,s2f[i],0);
	//	hier_block2::connect(s2f[i], 0, self(), i);
		hier_block2::connect(freq_comp_filt[i][1],0,self(),i);

	}

	dummy_copy->set_enabled(true);

	timeout_b = gnuradio::get_initial_sptr(new adiscope::timeout_block("msg"));
	hier_block2::msg_connect(m2k_blk, "msg", timeout_b, "msg");

	QObject::connect(&*timeout_b, SIGNAL(timeout()), this,
			 SLOT(got_timeout()));

}

M2kSource::~M2kSource() {
 qDebug()<<"m2ksource destroyed;";
}


M2kSource::sptr M2kSource::make(struct iio_context *ctx)
{
	return gnuradio::get_initial_sptr(new M2kSource(ctx));
}


void M2kSource::set_filter_parameters(int channel, int index, bool enable, float TC, float gain, float sample_rate )
{
	freq_comp_filt[channel][index]->set_enable(enable);
	freq_comp_filt[channel][index]->set_TC(TC);
	freq_comp_filt[channel][index]->set_filter_gain(gain);
	freq_comp_filt[channel][index]->set_sample_rate(sample_rate);
}



/* Replace the iio_block source with the mixed source */
void M2kSource::enableMixedSignal(gr::m2k::mixed_signal_source::sptr mixed_source) {
	for (uint i = 0; i < nb_channels; ++i) {
		hier_block2::disconnect(m2k_blk, i, freq_comp_filt[i][0], 0);
		hier_block2::connect(mixed_source, i, freq_comp_filt[i][0], 0);
	}

	hier_block2::msg_disconnect(m2k_blk, "msg", timeout_b, "msg");
	hier_block2::msg_connect(mixed_source, "msg", timeout_b, "msg");

	m_mixed_source = mixed_source;
}

/* Bring back the data from the iio_block source */
void M2kSource::disableMixedSignal(gr::m2k::mixed_signal_source::sptr mixed_source) {
	for (uint i = 0; i < nb_channels; ++i) {
		hier_block2::disconnect(mixed_source, i, freq_comp_filt[i][0], 0);
		hier_block2::connect(m2k_blk, i, freq_comp_filt[i][0], 0);
	}

	hier_block2::msg_disconnect(mixed_source, "msg", timeout_b, "msg");
	hier_block2::msg_connect(m2k_blk, "msg", timeout_b, "msg");

	m_mixed_source = nullptr;
	try {
		m_analogin->setKernelBuffersCount(KERNEL_BUFFERS_DEFAULT);
	} catch (libm2k::m2k_exception &e) {
		qDebug(CAT_IIO_MANAGER) << e.what();
	}
}

void M2kSource::set_buffer_size(unsigned long size)
{
	//std::unique_lock<std::mutex> lock(copy_mutex);
	buffer_size = size;
	update_buffer_size_unlocked();
}

void M2kSource::update_buffer_size_unlocked(){
	unsigned long size = buffer_size;

	if (size) {
		m2k_blk->set_buffer_size(size);
		if (m_mixed_source) {
			m_mixed_source->set_buffer_size(size);
		}
		this->buffer_size = size;
	}
}

void M2kSource::set_device_timeout(unsigned int mseconds)
{
	m2k_blk->set_timeout_ms(mseconds);
	if (m_mixed_source) {
		m_mixed_source->set_timeout_ms(mseconds);
	}
}

