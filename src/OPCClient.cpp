/*
*
* Copyright (c) 2014, James Hurlbut
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or
* without modification, are permitted provided that the following
* conditions are met:
*
* Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
*
* Neither the name of James Hurlbut nor the names of its
* contributors may be used to endorse or promote products
* derived from this software without specific prior written
* permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "OPCClient.h"
#include "cinder/app/App.h"
#include "cinder/Utilities.h"
//#include "Resources.h"

using namespace std;
using namespace cinder;

OPCClient::OPCClient( ) : mConnecting(false)
{
	mHost		= "localhost";
	mPort		= 7840;
	mRequest	= "echo";
	mIo = shared_ptr<boost::asio::io_service>( new boost::asio::io_service() );
	mClient = TcpClient::create( *mIo );
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
		
		mClient->connectResolveEventHandler( [ & ]()
		{
			
		} );
		mClient->connect( mHost, (uint16_t)mPort );
	}
	return false;
}
void OPCClient::connectConnectEventHandler( const std::function<void( TcpSessionRef )>& eventHandler )
{
	mClient->connectConnectEventHandler(eventHandler);
}
void OPCClient::connectErrorEventHandler( const std::function<void( std::string, size_t )>& eventHandler )
{
	mClient->connectErrorEventHandler(eventHandler);
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