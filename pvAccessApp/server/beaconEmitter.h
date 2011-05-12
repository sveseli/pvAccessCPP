/*
 * beaconEmitter.h
 */

#ifndef BEACONEMITTER_H
#define BEACONEMITTER_H

#include <timer.h>
#include <timeStamp.h>
#include <remote.h>

#include <osiSock.h>

#include <beaconServerStatusProvider.h>
//#include <serverContext.h>
#include <sharedPtr.h>

namespace epics { namespace pvAccess {

    class ServerContextImpl;

	/**
	 * BeaconEmitter
	 *
	 * @author gjansa
	 */
	class BeaconEmitter:
	   public TransportSender,
	   public epics::pvData::TimerCallback,
	   public std::tr1::enable_shared_from_this<BeaconEmitter>
	{
	public:
        typedef std::tr1::shared_ptr<BeaconEmitter> shared_pointer;
        typedef std::tr1::shared_ptr<const BeaconEmitter> const_shared_pointer;

		/**
		 * Constructor.
		 * @param transport	transport to be used to send beacons.
		 * @param context CA context.
		 */
//		BeaconEmitter(Transport::shared_pointer& transport, ServerContextImpl::shared_pointer& context);
		BeaconEmitter(Transport::shared_pointer& transport, std::tr1::shared_ptr<ServerContextImpl>& context);
		
        /**
		 * Test Constructor (ohne context)
		 * @param transport	transport to be used to send beacons.
		 */
		BeaconEmitter(Transport::shared_pointer& transport, const osiSockAddr& serverAddress);
        
		virtual ~BeaconEmitter();

		void lock();
		void unlock();
		
		void send(ByteBuffer* buffer, TransportSendControl* control);

		void timerStopped();

		/**
		 * Start emitting.
		 */
		void start();
		
        /**
		 * Reschedule timer.
		 */
		void reschedule();
		
        /**
		 * Timer callback.
		 */
		void callback();

		void destroy();
	
    private:

		/**
		 * Minimal (initial) CA beacon period (in seconds).
		 */
		static const float EPICS_CA_MIN_BEACON_PERIOD;

		/**
		 * Minimal CA beacon count limit.
		 */
		static const float EPICS_CA_MIN_BEACON_COUNT_LIMIT;

		/**
		 * Transport.
		 */
        Transport::shared_pointer _transport;

		/**
		 * Beacon sequence ID.
		 */
		int16 _beaconSequenceID;

		/**
		 * Startup timestamp (when clients detect a change, they will consider server restarted).
		 */
		epics::pvData::TimeStamp _startupTime;

		/**
		 * Fast (at startup) beacon period (in sec).
		 */
		double _fastBeaconPeriod;

		/**
		 * Slow (after beaconCountLimit is reached) beacon period (in sec).
		 */
		double _slowBeaconPeriod;

		/**
		 * Limit on number of beacons issued.
		 */
		int16 _beaconCountLimit;

		/**
		 * Server address.
		 */
		const osiSockAddr _serverAddress;

		/**
		 * Server port.
		 */
		int32 _serverPort;

		/**
		 * Server status provider implementation (optional).
		 */
        BeaconServerStatusProvider::shared_pointer _serverStatusProvider;

		/**
		 * Timer.
		 */
        Timer::shared_pointer _timer;

		/**
		 * Timer task node.
		 */
		epics::pvData::TimerNode _timerNode;
	};

}}

#endif  /* BEACONEMITTER_H */