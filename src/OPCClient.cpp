#include "OPCClient.h"
#include "cinder/app/App.h"
#include "cinder/Utilities.h"
#include "Resources.h"

using namespace std;
using namespace cinder;

OPCClient::OPCClient( ) : mConnecting(false)
{
	mHost		= "localhost";
	mPort		= 7840;
	mRequest	= "echo";
	mIo = shared_ptr<boost::asio::io_service>( new boost::asio::io_service() );
	
}
OPCClient::~OPCClient( ) 
{
	mSession->close();
	mSession.reset();
	
}
void OPCClient::update(){
	mIo->poll();
}
bool OPCClient::tryConnect()
{
	//check if we're already connected
	if(!isConnected())
	{
		return connect(mHost,mPort);
	}
	else {
		return true;
	}
}
bool OPCClient::connect(std::string pHost, int pPort){
	if(mConnecting == false){
		mConnecting = true;
		mHost = pHost;
		mPort = pPort;
		// Initialize a client by passing a boost::asio::io_service to it.
		// ci::App already has one that it polls on update, so we'll use that.
		// You can use your own io_service, but you will have to manage it 
		// manually (i.e., call poll(), poll_one(), run(), etc).
		mClient = TcpClient::create( *mIo );
		ci::app::console()<<"connect"<<endl;
		// Add callbacks to work with the client asynchronously.
		// Note that you can use lambdas.
		mClient->connectConnectEventHandler( &OPCClient::onConnect, this );
		mClient->connectErrorEventHandler( &OPCClient::onError, this );
		mClient->connectResolveEventHandler( [ & ]()
		{
			//ci::app::console()<< "Endpoint resolved"<< endl;
		
		} );
		mClient->connect( mHost, (uint16_t)mPort );
	}
	return false;
}
void OPCClient::write(std::string strBuffer)
{
	if ( mSession && mSession->getSocket()->is_open() ) {
		// Write data is packaged as a ci::Buffer. This allows 
		// you to send any kind of data. Because it's more common to
		// work with strings, the session object has static convenience 
		// methods for converting between std::string and ci::Buffer.
		Buffer buffer = TcpSession::stringToBuffer( strBuffer );
		mSession->write( buffer );
	} else {	
		// Before we can write, we need to establish a connection 
		// and create a session. Check out the onConnect method.
		connect(mHost,mPort);
	}
}
void OPCClient::write(std::vector<char> &data)
{
	if ( mSession && mSession->getSocket()->is_open()  ) {
		Buffer buffer = Buffer( &data[ 0 ], data.size() );
		mSession->write( buffer );
	} else {	
		// Before we can write, we need to establish a connection 
		// and create a session. Check out the onConnect method.
		connect(mHost,mPort);
	}
}
bool OPCClient::isConnected()
{
    return ( mSession && mSession->getSocket()->is_open() );
}
void OPCClient::onError( std::string err, size_t bytesTransferred ){
	ci::app::console()<< "OPCClient::onError "<< err << endl;
	//if(err == "An existing connection was forcibly closed by the remote host")
	mConnecting = false;
	if(isConnected())
		mSession->close();
}
void OPCClient::onConnect( TcpSessionRef session ){
	ci::app::console()<< "OPCClient::onConnect "<< endl;
	// Get the session from the argument and set callbacks.
	// Note that you can use lambdas.
	mSession = session;
	mSession->connectCloseEventHandler( [ & ]()
	{
		ci::app::console()<< "Disconnected"<< endl;
	} );
	mSession->connectErrorEventHandler( &OPCClient::onError, this );
	mSession->connectReadCompleteEventHandler( [ & ]()
	{
		ci::app::console()<< "Read complete"<< endl;
	} );
	//mSession->connectReadEventHandler( &OPCClient::onRead, this );
	//mSession->connectWriteEventHandler( &OPCClient::onWrite, this );
	mConnecting = false;
}