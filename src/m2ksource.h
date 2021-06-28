#ifndef M2KSOURCE_H
#define M2KSOURCE_H


#include <m2k/analog_in_source.h>
#include <m2k/mixed_signal_source.h>
#include <gnuradio/hier_block2.h>
#include <frequency_compensation_filter.h>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <timeout_block.hpp>
#include <grsource.h>

/* 1k samples by default */
#define IIO_BUFFER_SIZE 0x400
static const int KERNEL_BUFFERS_DEFAULT = 4;

class M2kSource : public GRSource
{
	Q_OBJECT
private:

	gr::m2k::analog_in_source::sptr m2k_blk;
	gr::m2k::mixed_signal_source::sptr m_mixed_source;

	adiscope::timeout_block::sptr timeout_b;

	libm2k::analog::M2kAnalogIn *m_analogin;
	libm2k::context::M2k *m_context;


public:
	typedef boost::shared_ptr<M2kSource> sptr;
	adiscope::frequency_compensation_filter::sptr freq_comp_filt[2][2];

	M2kSource(struct iio_context *ctx, QObject *parent = nullptr);
	~M2kSource();
	static sptr make(struct iio_context *ctx);

	void set_filter_parameters(int channel, int index, bool enable, float TC, float gain, float sample_rate );
	/* Replace the iio_block source with the mixed source */
	void enableMixedSignal(gr::m2k::mixed_signal_source::sptr mixed_source);
	/* Bring back the data from the iio_block source */
	void disableMixedSignal(gr::m2k::mixed_signal_source::sptr mixed_source);

	void set_buffer_size(unsigned long size);
	void update_buffer_size_unlocked();

	void set_device_timeout(unsigned int mseconds);

Q_SIGNALS:
	void timeout();

};

#endif
